#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/DynamixelApi.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/control/behaviors/generic/LearningCpgGait/LearningCpgGait.h>
#include <ase/control/behaviors/generic/LearningCpgGait/SPSACpgGait/SPSACpgGait.h>
#include <ase/control/strategies/SPSALearning/SPSALearning.h>
#include <ase/control/behaviors/generic/LearningCpgGait/RandomSearchCpgGait/RandomSearchCpgGait.h>
#include <ase/RotationController.h>
#include <ase/KeyController.h>
#include <ase/PositionTracker.h>

#define PLAYBACK	1
#define LEARNING 	0
#define TRACKPOS 	1
#define IGNORE_TIME_MS		10*1428
//#define LEARNING_ITT_MS	 	5*1428
#define LEARNING_ITT_MS	 	20*1428

static bool handDesigned=false;
static int state = 0;
static int delay = 0;
static long lastUpdate = 0;
static float oldUpdateTime;
static float oldPos;
static int nItterations=0;
static float ignoreUntilTime=0;
static bool learning_init = true;
static SPSALearningMsg_t msg;
static SPSALearningMsg_t* msg_prt = &msg;
static FILE* rewardLog;
static FILE* controllerLog;
static FILE* debugLog;
static bool restartWait = false;
Timer_t* periodicPosTimer;

void act(char* topic, Event_t* event)  {
	if(getLocalMsTime()<=(delay+lastUpdate)) {
		return;
	}
	else {
		lastUpdate = getLocalMsTime();
	}
	if(state==0) {
		printf("Set pos mode at %f sec.\n",getLocalTime());
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			RotationController_getRotation(i)->running = false;
			RotationController_setMode(i, false);
		}
		msg_prt->stateCount=0;
		delay = 500; state++;
	}
	else if(state==1) {
		printf("Init: Homing Robot at %f sec.\n",getLocalTime());
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			dynamixelApi_setGoalPos(i,512);
		}
		delay = 4000; state++;
	}
	else if(state==2) {
		printf("Changing rotation mode at %f sec.\n",getLocalTime());
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			dynamixelApi_wheelMove(i, 0, true);
			RotationController_setMode(i, true);
		}
		delay=500; state++;
	}
	else if(state==3) {
		if(!LEARNING && !PLAYBACK) {
			int i;
			for(i=0;i<getNumberOfActuators();i++) {
				RotationController_getRotation(i)->running = true;
				RotationController_getRotation(i)->pause = false;
				RotationController_applyRotation(i);
			}
			delay=0;
		}
		if(LEARNING||PLAYBACK) {
			CpgGait_start();
			delay=0; state++;
			ignoreUntilTime = getLocalTime()+IGNORE_TIME_MS/1000.0f;
		}
	}
	else if(state==4) {
		if(!restartWait && PositionTracker_isPaused()) {
			printf("************************\n");
			printf("Setup now paused at pos %i!!\n",PositionTracker_getCurrentPos());
			printf("Click START when ready\n");
			CpgGait_stop();
			int i;
			for(i=0;i<getNumberOfActuators();i++) {
				dynamixelApi_wheelMove(i, 0, true);
			}
			restartWait = true;
		}
		if(restartWait && !PositionTracker_isPaused()) {
			printf("Done now at pos %i\n",PositionTracker_getCurrentPos());
			printf("************************\n");
			CpgGait_start();
			ignoreUntilTime = getLocalTime()+IGNORE_TIME_MS/1000.0f;
			restartWait = false;
		}
	}
}

void periodic_pos_timer_fired(int id) {
	if(TRACKPOS) {
		if(PositionTracker_isReady()) {
			int currentPos = PositionTracker_getCurrentPos();
			float updateTime = PositionTracker_getLastUpdateTime();
			float deltaTime = updateTime - oldUpdateTime;
			float deltaPos = -(currentPos - oldPos);
			//printf("Reward: deltaTime = %f sec. and deltaPos = %f cm.\n", deltaTime, deltaPos);
			if(!learning_init) {
				if(getLocalTime()>=ignoreUntilTime&&!PositionTracker_isPaused()) {
					msg_prt->super.msgType = LEARNING_MESSAGE;
					msg_prt->super.label = 1;
					float reward;
					if(deltaTime!=0) {
						reward = deltaPos/deltaTime/2.1505883f;
					}
					else {
						reward = 0;
					}
					msg_prt->reward = reward;
					//msg_prt->reward = 15.0f*((float)rand())/(((float)RAND_MAX)+0.1f); //hack to inject random reward into the learning system
					msg_prt->stateCount++;
					printf("Measured velocity %f cm/sec at time %f\n", reward, getLocalTime());
					ase_handleMessage((char*) msg_prt, sizeof(msg), 0);
					if(rewardLog==0) {
						char name[100];
						sprintf(name, "rewards.log");
						rewardLog = fopen(name,"w");
					}
					fprintf(rewardLog,"%i \t%f \t %f\n",nItterations,getLocalTime(), reward);
					fflush(rewardLog);
					nItterations++;
				}
				else {
					if(getLocalTime()<ignoreUntilTime) printf("Ignoring updates, since %f < %f\n",getLocalTime(),ignoreUntilTime);
					if(PositionTracker_isPaused()) printf("Ignoring updates, since paused %f\n",getLocalTime());
				}
			}
			else {
				learning_init = false;
			}
			oldPos = currentPos;
			oldUpdateTime = updateTime;
		}
	}
}
static float pi = 3.14159265;
void handleCPG(HCCpg_t* cpg, int actuator) {
	float x = CpgSuper_getX((CpgSuper_t*)cpg);
	float y = CpgSuper_getY((CpgSuper_t*)cpg);
	float pd = CpgGait_getPhaseShift(actuator);
	float freq = CpgSuper_getFrequency((CpgSuper_t*)cpg);

	bool dir;
	if(handDesigned) {
		if(actuator<4) dir = false;
		else dir = true;
	}
	else {
		if(actuator<4) dir = false;
		else dir = true;
	}


	float angle = (pi-atan2(x,y));
	float percentPos = angle/(2*pi);
	RotationController_setRotationControl(dir, freq, pd, actuator);
	RotationController_getRotation(actuator)->running = true;
	RotationController_getRotation(actuator)->pause = false;
	if(actuator==0) {
		float t = getLocalTime();
		float f =  freq;
		float pd2 = pd/6.28f; //0-1
		float percentPos2 = f*t+pd2;
		int goalPos = ((int)(percentPos*1194))%1194;
		int goalPos2 = ((int)(percentPos2*1194))%1194;
		float cx = CpgSuper_getX((CpgSuper_t*)CpgGait_getClockCpg());
		float cy = CpgSuper_getY((CpgSuper_t*)CpgGait_getClockCpg());
		int g[10];
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			HCCpg_t* cpg = CpgGait_getLocalCpg(i);
			float x_temp = CpgSuper_getX((CpgSuper_t*)cpg);
			float y_temp = CpgSuper_getY((CpgSuper_t*)cpg);
			float angle_temp = (pi-atan2(x_temp,y_temp));
			float percentPos_temp = angle_temp/(2*pi);
			g[i] = ((int)(percentPos_temp*1194))%1194;
		}
		g[8] = ((int)((f*t+0.0/6.28f)*1194))%1194;
		g[9] = ((int)((f*t+pi/6.28f)*1194))%1194;

		if(debugLog==NULL) {
			debugLog = fopen("debug.log", "w");
		}
		//fprintf(debugLog,"%i \t %i \t %f \t %f \t %f \t %f\n",goalPos,goalPos2,x,y,cx,cy);
		fprintf(debugLog,"%f \t %i \t %i \t %i \t %i \t %i \t %i \t %i \t %i \t %i \t %i\n",getLocalTime(),g[0],g[1],g[2],g[3],g[4],g[5],g[6],g[7],g[8],g[9]);
		fflush(debugLog);
	}
	bool home=false;
	//if(nItterations>10&&(actuator==0||actuator==1)) { //works improves 3cm/sec from 7 to 10 cm/sec
	//if(nItterations>10&&(actuator==0||actuator==1||actuator==8-2||actuator==9-2)) { //not fully tested (probably not working)
	//if(nItterations>10&&(actuator==2||actuator==3||actuator==4||actuator==5)) {  //works a little 0 to 2.5 cm/sec
	/*if(nItterations>10&&(actuator==0||actuator==3||actuator==4||actuator==7)) { //knee actuators
		home = true;
	}*/
	if(!home) {
		//RotationController_applyRotation(actuator); //equation based
		RotationController_applyRotationTo(actuator,percentPos); //cpg based
	}
	else {
		RotationController_setMode(actuator, false);
		dynamixelApi_setGoalPos(actuator,512);
	}
}

void init_SPSA_Learning() {
	SPSACpgGait_init();
	CpgGait_setUseFeedback(0);
	SPSACpgGait_setLearningParameters(0.0015f, 0.025f);
	int i;
	for(i=0;i<getNumberOfActuators();i++) {
		dynamixelApi_setWheelMode(i, false);
	}
	//float initPhaseShift[] = {-1,-1,0,0,pi,pi,pi,pi};
	//setting interval of open parameters
	for(i=0;i<getNumberOfActuators();i++) {
		LearningCpgGait_setFrequencyInterval(0.7f, 0.7f, 0.7f);
		LearningCpgGait_setAmplitudeInterval(1.0f, 1.0f, 1.0f,i);
		LearningCpgGait_setPhaseshiftInterval(-pi, 0, pi,i);
		LearningCpgGait_setOffsetInterval(0.0, 0.0, 0.0, i);
	}

	int id = 1;
	int n=1;
	char* filename = "handcoded-initial.gait";
	//char* filename = "learned-continued.gait"; n=2;id =117-n;
	//char* filename = "manual-learned-exp6.gait"; n=10; id = 38-n+1;
	FILE* controller = fopen(filename, "r");
	if (controller == NULL ) {
		printf("Error: I couldn't open file %s for writing.\n",filename);
	}
	printf("Now Loading Initial Controller: %s...\n",filename);
	CpgGait_loadAverageController(id,n, controller);

	//SPSACpgGait_reset(0);
	SPSACpgGait_setThetaFromCpgGait();
	//SPSACpgGait_setStateExploit();
	CpgGait_stop();
	CpgGait_setActuatorPosHandler(handleCPG);
}

void init_Rotation_Controller() {
	float freq = 0.7f;
	float pi= 3.14;
	int i;
	for(i=0;i<getNumberOfActuators();i++) {
		RotationController_initActuator(i, false);
	}
	RotationController_setRotationControl(false, freq, -1, 0); //2
	RotationController_setRotationControl(false, freq, -1, 1); //3
	RotationController_setRotationControl(false, freq, 0, 2); //4
	RotationController_setRotationControl(false, freq, 0, 3); //5
	RotationController_setRotationControl(true, freq, pi, 4); //6
	RotationController_setRotationControl(true, freq, pi, 5); //7
	RotationController_setRotationControl(true, freq, pi, 6); //8
	RotationController_setRotationControl(true, freq, pi, 7); //9
}
void connect_dynamixels() {
	dynamixelApi_connect(2);
	dynamixelApi_connect(3);
	dynamixelApi_connect(4);
	dynamixelApi_connect(5);
	dynamixelApi_connect(6);
	dynamixelApi_connect(7);
	dynamixelApi_connect(8);
	dynamixelApi_connect(9);
	dynamixelApi_setIsLoggingPos(true);
}

static void learningUpdated(char* topic, Event_t* event) {
	CpgGait_logController(nItterations,controllerLog);
}
void init_controllerLog() {
	char name[100];
	sprintf(name, "cpgController.log");
	controllerLog = fopen(name,"w");
}
void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(ACT_EVENT,act);
	EventManager_subscribe(LEARNING_CPG_GAIT_UPDATED_EVENT, learningUpdated);
	periodicPosTimer = TimerManager_createPeriodicTimer(LEARNING_ITT_MS, 0, periodic_pos_timer_fired);
	RotationController_initialize();
	init_controllerLog();
	connect_dynamixels();
	if(TRACKPOS) {
		PositionTracker_start(789,"localhost");
	}
	if(!PLAYBACK) {
		//KeyController_startKeyInputControl();
		if(LEARNING) {
			init_Rotation_Controller();
			init_SPSA_Learning(); //add pause to cpg gait and spsa-cpg gait
		}
		else {
			init_Rotation_Controller();
		}
	}
	else { //PLAYBACK
		init_Rotation_Controller();
		CpgGait_init();
		CpgGait_setUseFeedback(0);
		FILE* controller = fopen("handcoded-clean.gait", "r"); int id=1; int n=1;
		//FILE* controller = fopen("auto-handcoded-clean.gait", "r"); int id=1; int n=1;
		//FILE* controller = fopen("handcoded-initial.gait", "r"); int id=1; int n=1;
		//FILE* controller = fopen("learned.gait", "r");  int n=10; int id = 206-n+1;
		//FILE* controller = fopen("manual-learned-exp2.gait", "r");  int n=1; int id = 204-n+1;
		//FILE* controller = fopen("manual-learned-exp3.gait", "r");  int n=10; int id = 165-n+1;
		//FILE* controller = fopen("manual-learned-exp4.gait", "r");  int n=10; int id = 181-n+1;
		//FILE* controller = fopen("manual-learned-exp5.gait", "r");  int n=10; int id = 95-n+1;
		//FILE* controller = fopen("manual-learned-exp6.gait", "r");  int n=10; int id = 38-n+1;
		//FILE* controller = fopen("manual-learned-exp6p3.gait", "r");  int n=2; int id = 12-n+1;
		//FILE* controller = fopen("manual-learned-fault-exp1.gait", "r");  int n=10; int id = 103-n+1;
		//FILE* controller = fopen("manual-learned-fault-exp2.gait", "r");  int n=10; int id = 106-n+1;
		//FILE* controller = fopen("optimized-learned-exp1.gait", "r");  int n=1; int id = 111;


		if (controller == NULL ) {
			printf("Error: I couldn't open file for reading.\n");
		}
		printf("Now Loading Controller...\n");
		CpgGait_loadAverageController(id, n, controller);
		CpgGait_setActuatorPosHandler(handleCPG);
		printf("...done Loading Controller...\n");
	}
}
