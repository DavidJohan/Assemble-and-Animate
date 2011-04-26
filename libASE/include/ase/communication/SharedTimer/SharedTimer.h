typedef struct {
	long timeMs;
} sharedTime_t;




void SharedTimer_init(long msDelay);
long SharedTimer_getSharedMsTime();
float SharedTimer_getSharedTime();
void SharedTimer_reset();
