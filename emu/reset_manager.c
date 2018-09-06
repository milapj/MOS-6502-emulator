#include <base.h>
#include <rc.h>
#include <reset_manager.h>

static void
deinit (reset_manager_t * rm)
{
	for (size_t i = 0; i < rm->ndevices; i++) {
		if (rm->devices[i].dev) {
			rc_weak_release((void * nonnull)rm->devices[i].dev);
		}
	}
}

reset_manager_t *
reset_manager_new (void)
{
	return rc_alloc(sizeof(reset_manager_t), deinit);
}

void
reset_manager_add_device (reset_manager_t * rm, void * dev, void * handler)
{
	ASSERT(rm->ndevices < RESET_MANAGER_MAX_DEVICES);
	rm->devices[rm->ndevices].handler = handler;
	rm->devices[rm->ndevices].dev = rc_weak_retain(dev);
	rm->ndevices++;
}

void
reset_manager_issue_reset (reset_manager_t * rm)
{
	for (size_t i = 0; i < rm->ndevices; i++) {
		if (rm->devices[i].dev && rc_weak_check(&rm->devices[i].dev)) {
			rm->devices[i].handler((void * nonnull)rm->devices[i].dev);
		}
	}
}
