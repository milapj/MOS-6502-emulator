#pragma once

#include <base.h>
#include <reset_manager.h>

#include <stddef.h>
#include <stdint.h>

#define TIMEKEEPER_MAX_TIMERS 16

typedef struct timekeeper {
	struct {
		void (*nullable fire)(void * nonnull obj);
		uint64_t * nullable countdown;
		void * nullable /*strong*/ obj;
	} timers[TIMEKEEPER_MAX_TIMERS];
	size_t ntimers;

	uint32_t t_ref;
	uint32_t t_pause;

	double clk_period;
	uint64_t clk_cyclenum;
} timekeeper_t;

timekeeper_t * nullable timekeeper_new (reset_manager_t * nonnull rm, double clk_period);
void timekeeper_add_timer (timekeeper_t * nonnull tk, void * nonnull timer, void * nonnull fire, uint64_t * nonnull countdown);
void timekeeper_advance_clk (timekeeper_t * nonnull tk, uint64_t ncycles);
void timekeeper_sync (timekeeper_t * nonnull tk);
void timekeeper_pause (timekeeper_t * nonnull tk);
void timekeeper_resume (timekeeper_t * nonnull tk);
