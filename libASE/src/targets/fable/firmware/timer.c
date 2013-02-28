#include <avr/io.h>
#include <avr/iom2561.h>
#include <ase/targets/fable/firmware/timer.h>
#include <ase/targets/fable/firmware/macro.h>

volatile uint32_t system_clock;
static uint8_t current_timer_index;

void start_system_timer(void) {
	// initialize the timer
	current_timer_index = 1;

	system_clock = 0;

	OCR2A = 249;// 8000/2*32*125= 1 ms

	TCCR2A = (1<<WGM21)|(0<<WGM20);//clear on compare mode
	TIMSK2 = (1<<OCIE2A);//enable interrupt on compare
	TCCR2B = (0<<CS22)|(1<<CS21)|(1<<CS20);//clk_T2S/32
}

uint32_t read_system_clock(void) {
	return system_clock;
}

void stop_system_timer(void) {
	TCCR2B = (0<<CS22)|(0<<CS21)|(0<<CS20);
}

bool timer_compare(ptimer timer, uint32_t compare_time) {
	if((system_clock - timer->time) > compare_time)
		return true;
	return false;
}

void timer_reset(ptimer timer) {
	timer->time = system_clock;
}

uint32_t timer_read(ptimer timer) {
	return system_clock - timer->time;
}

ISR(TIMER2_COMPA_vect) {
	system_clock++;
	//heartbeat lead blink fcn... should not be here!
//	if((system_clock%1000)==0){
//#ifdef OLD
//		TOGGLE(PORTA,4);//for the old board
//#else
//		PORTA^=1;//for the new board
//#endif
//	}
}
