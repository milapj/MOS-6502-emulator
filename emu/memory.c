#include <rc.h>
#include <base.h>
#include <memory.h>
#include <string.h>

static void
reset (memory_t * memory)
{
	memset(memory->bytes, 0xFF, memory->size);
}

memory_t *
memory_new (reset_manager_t * rm, size_t size, bool writeable)
{
	memory_t * mem = rc_alloc(sizeof(memory_t) + size, NULL);
	mem->size = size;
	mem->writeable = writeable;
	if (writeable) {
		reset_manager_add_device(rm, mem, reset);
	}
	return mem;
}

void
memory_map (memory_t * mem, membus_t * bus, uint16_t bus_start, uint16_t size, size_t start)
{
	ASSERT(bus_start % MEMBUS_PAGESIZE == 0);
	ASSERT(size % MEMBUS_PAGESIZE == 0);
	ASSERT(start + size <= mem->size);

	size_t start_page = bus_start / MEMBUS_PAGESIZE;
	size_t npages = size / MEMBUS_PAGESIZE;
	for (size_t i = 0; i < npages; i++) {
		uint8_t * data = mem->bytes + start + i * MEMBUS_PAGESIZE;
		membus_set_read_memory(bus, i + start_page, mem, data);
		if (mem->writeable) {
			membus_set_write_memory(bus, i + start_page, mem, data);
		}
	}
}

void
memory_map_mirroring (memory_t * mem, membus_t * bus, uint16_t bus_start, uint16_t size, size_t start, size_t nmirrors)
{
	for (size_t i = 0; i < nmirrors; i++) {
		memory_map(mem, bus, (uint16_t)(bus_start + i * size), size, start);
	}
}
