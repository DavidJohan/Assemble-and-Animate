#ifndef __TIMER_H_
#define __TIMER_H_

#include <stdint.h>
#include <stdarg.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "stddefs.h"

#define SYSTEM_TIMER 0

typedef struct timer {
	uint32_t time;
} timer, *ptimer;

uint32_t read_system_clock(void);
void timer_reset(ptimer timer);
bool timer_compare(ptimer timer, uint32_t compare_time);
uint32_t timer_read(ptimer timer);

void start_system_timer(void);
void stop_system_timer(void);

#endif



