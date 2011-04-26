#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/infrastructure/MsgManager/MsgManager.h>
#include <ase/control/behaviors/generic/LearningCpgGait/LearningCpgGait.h>
#include <ase/config/ConfigASE.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <math.h>

LearningGpgGaitInterval_t offsetInterval[MAX_ACTUATORS];
LearningGpgGaitInterval_t phaseshiftInterval[MAX_ACTUATORS];
LearningGpgGaitInterval_t amplitudeInterval[MAX_ACTUATORS];
LearningGpgGaitInterval_t feedbackTermInterval[MAX_ACTUATORS][CPGGAIT_N_FEEDBACK_TERMS];
LearningGpgGaitInterval_t frequencyInterval;


static float scale(float x, LearningGpgGaitInterval_t* interval) {
	if(x==-1.0f) {
		return interval->home;
	}
	else {
		float a = interval->upperlimit;
		float b = interval->lowerlimit;
		return (a-b)*x+b;
	}
}

static float invertScale(float s, LearningGpgGaitInterval_t* interval) {
	float a = interval->upperlimit;
	float b = interval->lowerlimit;
	if(a==b) return 0.5;
	return (s-b)/(a-b);
}

static void updateGait(float* parameters) {
	int i, j, index=0;
	for(i=0;i<getNumberOfActuators();i++) {
		float phaseShift = scale(parameters[index], &phaseshiftInterval[i]);
		CpgGait_setPhaseShift(phaseShift, i);
		index++;

		float amplitude = scale(parameters[index], &amplitudeInterval[i]);
		CpgGait_setAmplitude(amplitude, i);
		index++;

		float offset = scale(parameters[index], &offsetInterval[i]);
		CpgGait_setOffset(offset, i);
		index++;

		for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
			float term = scale(parameters[index], &feedbackTermInterval[i][j]);
			CpgGait_setFeedbackTerm(term, j, i);
			index++;
		}
	}
	float freq = scale(parameters[index], &frequencyInterval);
	index++;
	for(i=0;i<getNumberOfActuators();i++) {
		CpgGait_setFrequency(freq, i);
	}
}

void LearningCpgGait_getScaledController(float* controller) {
	int i, j, index=0;
	for(i=0;i<getNumberOfActuators();i++) {
		controller[index] = invertScale(CpgGait_getPhaseShift(i),&phaseshiftInterval[i]); index++; //phase
		controller[index] = invertScale(CpgGait_getAmplitude(i), &amplitudeInterval[i]); index++; //amplitude
		controller[index] = invertScale(CpgGait_getOffset(i), &offsetInterval[i]); index++; //offset
		for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
			controller[index] = invertScale(CpgGait_getFeedbackTerm(j,i),&feedbackTermInterval[i][j]); index++; // feedback terms
		}
	}
	controller[index] = invertScale(CpgGait_getFrequency(0), &frequencyInterval); index++; //frequency
}

void LearningCpgGait_signalUpdated() {
	Event_t event;
	EventManager_publish(LEARNING_CPG_GAIT_UPDATED_EVENT,&event);
}

void LearningGpgGait_init() {
	int i,j;
	for(i=0;i<getNumberOfActuators();i++) {
		offsetInterval[i].lowerlimit = getActuatorPosition(i);
		offsetInterval[i].upperlimit = offsetInterval[i].lowerlimit;

		phaseshiftInterval[i].lowerlimit =0.0f;
		phaseshiftInterval[i].upperlimit =3.1415f;

		amplitudeInterval[i].lowerlimit = 0.0f;
		amplitudeInterval[i].upperlimit = 0.5f;

		for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) { //feedback disabled as default
			feedbackTermInterval[i][j].lowerlimit = 0;
			feedbackTermInterval[i][j].upperlimit = 0;
		}
	}
	frequencyInterval.lowerlimit = 1.0f;
	frequencyInterval.upperlimit = 1.0f;
}

void LearningCpgGait_setFrequencyInterval(float lowerlimit, float home, float upperlimit) {
	frequencyInterval.lowerlimit = lowerlimit;
	frequencyInterval.upperlimit = upperlimit;
	frequencyInterval.home = home;
}

void LearningCpgGait_setAmplitudeInterval(float lowerlimit, float home, float upperlimit, int index) {
	amplitudeInterval[index].lowerlimit = lowerlimit;
	amplitudeInterval[index].upperlimit = upperlimit;
	amplitudeInterval[index].home = home;
}

void LearningCpgGait_setPhaseshiftInterval(float lowerlimit, float home, float upperlimit, int index) {
	phaseshiftInterval[index].lowerlimit = lowerlimit;
	phaseshiftInterval[index].upperlimit = upperlimit;
	phaseshiftInterval[index].home = home;
}

void LearningCpgGait_setOffsetInterval(float lowerlimit, float home, float upperlimit, int index) {
	offsetInterval[index].lowerlimit = lowerlimit;
	offsetInterval[index].upperlimit = upperlimit;
	offsetInterval[index].home = home;
}

void LearningCpgGait_setFeedbackTermInterval(float lowerlimit, float home, float upperlimit, int nr, int index) {
	feedbackTermInterval[index][nr].lowerlimit = lowerlimit;
	feedbackTermInterval[index][nr].upperlimit = upperlimit;
	feedbackTermInterval[index][nr].home = home;
}



void LearningCpgGait_updateGait(float* parameters) {
	updateGait(parameters);
}


void LearningCpgGait_logController(int id, FILE* log, float* parameters) {
	#ifdef FPRINTF
	fprintf(log,"%i ",id);
	int i,j,index=0;
	for(i=0;i<getNumberOfActuators();i++) {
		float freq = scale(parameters[index], &frequencyInterval); index++;
		fprintf(log,"%f ", freq);
		float amplitude = scale(parameters[index], &amplitudeInterval[i]); index++;
		fprintf(log,"%f ", amplitude);
		float phaseShift = scale(parameters[index], &phaseshiftInterval[i]); index++;
		fprintf(log,"%f ", phaseShift);
		float offset = scale(parameters[index], &offsetInterval[i]); index++;
		fprintf(log,"%f ", offset);
	}
	for(i=0;i<getNumberOfActuators();i++) {
		for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
			float term = scale(parameters[index], &feedbackTermInterval[i][j]); index++;
			fprintf(log,"%f ", term);
		}
	}
	fprintf(log,"\n");
	fflush(log);
	#endif
}

