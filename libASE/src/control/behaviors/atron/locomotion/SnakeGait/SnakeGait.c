#include <stdio.h>
#include <ase/control/strategies/Cpg/CnoCpg/CnoCpg.h>
#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <ase/control/strategies/Cpg/CnoCpg/CnoCpgManager.h>
#include <ase/control/behaviors/atron/locomotion/SnakeGait/SnakeGait.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Entity/Entity.h>
#define SNAKE_CPG_LABEL		1
#define GAIT_LABEL			0

static SnakeGait_t g;
static SnakeGait_t* gait = &g;

void setActuatorPos(CnoCpg_t* cpg) {
	int goal = (int)(CpgSuper_getX((CpgSuper_t*)cpg)); //scale to -amplitude to amplitude
	if(goal<0) goal = 360+goal; //scale to 360-amplitude to amplitude
	atronApi_rotateToDegreeInDegrees(goal);
}

void SnakeGait_setAmplitude(float amplitude) {
	CpgSuper_setAmplitude((CpgSuper_t*) gait->cpg, amplitude);
}

void SnakeGait_setOffset(float offset) {
	CpgSuper_setOffset((CpgSuper_t*) gait->cpg, offset, 0);
}

void SnakeGait_setPhaseShift(float phaseShift) {
	CpgSuper_setPhaseDifference((CpgSuperCoupling_t*) gait->h2tCoupling, phaseShift);
	CpgSuper_setPhaseDifference((CpgSuperCoupling_t*) gait->t2hCoupling, 6.283f-phaseShift);
}

void SnakeGait_setFrequency(float hz) {
	CpgSuper_setFrequency(hz, (CpgSuper_t*)gait->cpg);
}

static void cpg_updated(char* topic, Event_t* event) {
	if(!Strategy_isPaused((Strategy_t*) gait) && Strategy_isStarted((Strategy_t*) gait)) {
		if(event->val_prt==gait->cpg) {
			setActuatorPos(gait->cpg);
		}
	}
}

static void start(void* data) {	Strategy_start((Strategy_t*)gait->cpg, gait->cpg);}
static void stop(void* data)  { Strategy_stop((Strategy_t*)gait->cpg, gait->cpg);}
static void pause(void* data) { Strategy_pause((Strategy_t*)gait->cpg, gait->cpg);}
static void resume(void* data){ Strategy_resume((Strategy_t*)gait->cpg, gait->cpg);}
static void reset(void* data) { Strategy_restart((Strategy_t*)gait->cpg, gait->cpg);}


void SnakeGait_init(short label) {
	Strategy_init((Strategy_t*) gait, label);
	Strategy_setStartHandler((Strategy_t*) gait, start);
	Strategy_setStopHandler((Strategy_t*) gait, stop);
	Strategy_setPauseHandler((Strategy_t*) gait, pause);
	Strategy_setResumeHandler((Strategy_t*) gait, resume);
	Strategy_setRestartHandler((Strategy_t*) gait, reset);

	gait->cpg = CnoCpgManager_createCpg(SNAKE_CPG_LABEL, 1);
	gait->h2tCoupling = CnoCpgManager_createCoupling(SNAKE_CPG_LABEL, SNAKE_CPG_LABEL, 1, 0);
	gait->t2hCoupling = CnoCpgManager_createCoupling(SNAKE_CPG_LABEL, SNAKE_CPG_LABEL, 1, 0);
	int i;
	for(i=0;i<4;i++)  {
		CpgSuper_setApplyToChannel((CpgSuperCoupling_t*) gait->h2tCoupling, i, 1);
		CpgSuper_setApplyToChannel((CpgSuperCoupling_t*) gait->t2hCoupling, i, 0);
	}
	for(i=4;i<8;i++)  {
		CpgSuper_setApplyToChannel((CpgSuperCoupling_t*) gait->h2tCoupling, i, 0);
		CpgSuper_setApplyToChannel((CpgSuperCoupling_t*) gait->t2hCoupling, i, 1);
	}
	SnakeGait_setAmplitude(60.0f);
	SnakeGait_setOffset(0.0f);
	SnakeGait_setPhaseShift(2.464f);
	SnakeGait_setFrequency(0.20f);//0.15f
	EventManager_subscribe(CPG_UPDATED_EVENT,cpg_updated);
	Strategy_start((Strategy_t*) gait, NULL);
}
