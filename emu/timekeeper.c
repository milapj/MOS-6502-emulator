#include <SDL2/SDL.h>
#include <base.h>
#include <rc.h>
#include <timekeeper.h>

void
timekeeper_advance_clk (timekeeper_t * tk, uint64_t ncycles)
{
	uint64_t mincount = ncycles;

	for (size_t i = 0; i < tk->ntimers; i++) {
		if (tk->timers[i].countdown[0] < mincount) {
			mincount = tk->timers[i].countdown[0];
		}
	}

	tk->clk_cyclenum += mincount;

	for (size_t i = 0; i < tk->ntimers; i++) {
		tk->timers[i].countdown[0] -= mincount;
		if (tk->timers[i].countdown[0] == 0) {
			tk->timers[i].fire((void * nonnull)tk->timers[i].obj);
		}
	}

	if (mincount != ncycles) {
		timekeeper_advance_clk(tk, ncycles - mincount);
	}
}

void
timekeeper_add_timer (timekeeper_t * tk, void * timer, void * fire, uint64_t * countdown)
{
	ASSERT(tk->ntimers < TIMEKEEPER_MAX_TIMERS);

	tk->timers[tk->ntimers].fire = fire;
	tk->timers[tk->ntimers].obj = rc_retain(timer);
	tk->timers[tk->ntimers].countdown = countdown;

	tk->ntimers++;
}

void
timekeeper_sync (timekeeper_t * tk)
{
	uint32_t t_target = (uint32_t)(tk->clk_period * tk->clk_cyclenum * 1e3 + tk->t_ref);
	uint32_t t_now = SDL_GetTicks();
	if (t_now < t_target) {
		SDL_Delay(t_target - t_now);
	}
}

void
timekeeper_pause (timekeeper_t * tk)
{
	tk->t_pause = SDL_GetTicks();
}

void
timekeeper_resume (timekeeper_t * tk)
{
	tk->t_ref += SDL_GetTicks() - tk->t_pause;
}

static void
deinit (timekeeper_t * tk)
{
	for (size_t i = 0; i < tk->ntimers; i++) {
		rc_release((void * nonnull)tk->timers[i].obj);
	}
}

static void
reset (timekeeper_t * tk)
{
	tk->clk_cyclenum = 0;
	tk->t_ref = SDL_GetTicks();
}

timekeeper_t *
timekeeper_new (reset_manager_t * rm, double clk_period)
{
	timekeeper_t * tk = rc_alloc(sizeof(timekeeper_t), deinit);
	reset_manager_add_device(rm, tk, reset);
	tk->clk_period = clk_period;
	return tk;
}
