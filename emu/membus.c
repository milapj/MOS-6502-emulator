#include <rc.h>
#include <base.h>
#include <membus.h>
#include <reset_manager.h>

static void
deinit (membus_t * nonnull bus)
{
	for (size_t pagenum = 0; pagenum < MEMBUS_NPAGES; pagenum++) {
		membus_clear_page(bus, pagenum);
	}
}

static void
reset (membus_t * nonnull bus)
{
	(void)bus; // stop clang/gcc from complaining about `bus` being unused
#ifndef OPEN_BUS_TO_VCC
	bus->data_lanes = 0xFF;
#endif
}

membus_t *
membus_new (reset_manager_t * rm)
{
	membus_t * bus = rc_alloc(sizeof(membus_t), deinit);
	reset_manager_add_device(rm, bus, reset);
	return bus;
}

uint8_t
membus_read (membus_t * bus, uint16_t addr)
{
	size_t pagenum = addr / MEMBUS_PAGESIZE;
	ASSERT(pagenum < MEMBUS_NPAGES);

	void * obj = bus->read_mappings[pagenum].obj;
	uint8_t * data = bus->read_mappings[pagenum].data;
	size_t offset_p1 = bus->read_mappings[pagenum].offset_p1;
	uint8_t (* handler)(void * nonnull, size_t, uint8_t *) = (__typeof(handler))bus->read_mappings[pagenum].handler;

	uint8_t unmixed_val;
	uint8_t lane_mask = 0xFF;

	if (LIKELY(obj)) {
		if (UNLIKELY(offset_p1)) {
			size_t final_addr = (size_t)addr % MEMBUS_PAGESIZE + offset_p1 - 1;
			unmixed_val = handler(obj, final_addr, &lane_mask);
		}
		else {
			unmixed_val = data[addr % MEMBUS_PAGESIZE];
		}
	}
	else {
		lane_mask = 0x00;
		unmixed_val = 0x00;
	}

#ifndef OPEN_BUS_TO_VCC
	bus->data_lanes &= ~lane_mask;
	bus->data_lanes |= unmixed_val;
	return bus->data_lanes;
#else
	return unmixed_val | ~lane_mask;
#endif
}

void
membus_write (membus_t * bus, uint16_t addr, uint8_t val)
{
	size_t pagenum = addr / MEMBUS_PAGESIZE;
	ASSERT(pagenum < MEMBUS_NPAGES);

	void * obj = bus->write_mappings[pagenum].obj;
	uint8_t * data = bus->write_mappings[pagenum].data;
	size_t offset_p1 = bus->write_mappings[pagenum].offset_p1;
	void (* handler)(void * nonnull, size_t, uint8_t) = (__typeof(handler))bus->write_mappings[pagenum].handler;

#ifndef OPEN_BUS_TO_VCC
	bus->data_lanes = val;
#endif

	if (UNLIKELY(!obj)) {
		return;
	}

	if (UNLIKELY(offset_p1)) {
		size_t final_addr = (size_t)addr % MEMBUS_PAGESIZE + offset_p1 - 1;
		handler(obj, final_addr, val);
		return;
	}

	data[addr % MEMBUS_PAGESIZE] = val;
}

void
membus_clear_page (membus_t * bus, size_t pagenum)
{
	if (bus->read_mappings[pagenum].obj) {
		rc_release((void * nonnull)bus->read_mappings[pagenum].obj);
		bus->read_mappings[pagenum].obj = NULL;
	}
	if (bus->write_mappings[pagenum].obj) {
		rc_release((void * nonnull)bus->write_mappings[pagenum].obj);
		bus->write_mappings[pagenum].obj = NULL;
	}
}

void
membus_set_read_handler (membus_t * bus, size_t pagenum, void * obj, size_t offset, void * handler)
{
	if (bus->read_mappings[pagenum].obj) {
		rc_release((void * nonnull)bus->read_mappings[pagenum].obj);
	}
	bus->read_mappings[pagenum].obj = rc_retain(obj);
	bus->read_mappings[pagenum].handler = handler;
	bus->read_mappings[pagenum].offset_p1 = offset + 1;
}

void
membus_set_write_handler (membus_t * bus, size_t pagenum, void * obj, size_t offset, void * handler)
{
	if (bus->write_mappings[pagenum].obj) {
		rc_release((void * nonnull)bus->write_mappings[pagenum].obj);
	}
	bus->write_mappings[pagenum].obj = rc_retain(obj);
	bus->write_mappings[pagenum].handler = handler;
	bus->write_mappings[pagenum].offset_p1 = offset + 1;
}

void
membus_set_read_memory (membus_t * bus, size_t pagenum, void * obj, void * data)
{
	if (bus->read_mappings[pagenum].obj) {
		rc_release((void * nonnull)bus->read_mappings[pagenum].obj);
	}
	bus->read_mappings[pagenum].obj = rc_retain(obj);
	bus->read_mappings[pagenum].data = data;
	bus->read_mappings[pagenum].offset_p1 = 0;
}

void
membus_set_write_memory (membus_t * bus, size_t pagenum, void * obj, void * data)
{
	if (bus->write_mappings[pagenum].obj) {
		rc_release((void * nonnull)bus->write_mappings[pagenum].obj);
	}
	bus->write_mappings[pagenum].obj = rc_retain(obj);
	bus->write_mappings[pagenum].data = data;
	bus->write_mappings[pagenum].offset_p1 = 0;
}
