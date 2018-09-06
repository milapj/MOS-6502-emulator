#include <rc.h>

void *
rc_alloc (size_t size, void * deinit)
{
	rc_t * rc = calloc(1, size + sizeof(rc_t));
	rc->strong_count = 1;
	rc->deinit = deinit;
	return rc + 1;
}

void *
rc_retain (void * obj)
{
	rc_t * rc = (rc_t *)obj - 1;

	ASSERT(rc->strong_count != 0);

	rc->strong_count++;
	return obj;
}

void
rc_release (void * obj)
{
	rc_t * rc = (rc_t *)obj - 1;

	ASSERT(rc->strong_count != 0);

	rc->strong_count--;

	if (!rc->strong_count) {
		if (rc->deinit) {
			rc->deinit(obj);
		}
		if (!rc->weak_count) {
			free(rc);
		}
	}
}

void *
rc_weak_retain (void * obj)
{
	rc_t * rc = (rc_t *)obj - 1;
	rc->weak_count++;
	return obj;
}

void
rc_weak_release (void * nonnull obj)
{
	rc_t * rc = (rc_t *)obj - 1;

	ASSERT(rc->weak_count != 0);

	rc->weak_count--;

	if (!rc->strong_count && !rc->weak_count) {
		free(rc);
	}
}

void *
rc_weak_check (void ** obj_ptr)
{
	rc_t * rc = (rc_t *)(*obj_ptr) - 1;

	ASSERT(rc->weak_count != 0);

	if (!rc->strong_count) {
		rc_weak_release(*obj_ptr);
		*obj_ptr = NULL;
	}

	return *obj_ptr;
}
