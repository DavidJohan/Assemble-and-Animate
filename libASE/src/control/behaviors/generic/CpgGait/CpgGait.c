#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpg.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpgManager.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <math.h>
#define GLOBAL_CPG_LABEL	1

//\todo rewrite as Behavior_t

static HCCpg_t* cpgGlobal;
static HCCpg_t* cpgLocal[MAX_ACTUATORS];
static CpgSuperCoupling_t *g2g, *g2lCoupling[MAX_ACTUATORS], *l2gCoupling[MAX_ACTUATORS];
static float feedbackTerms[MAX_ACTUATORS][CPGGAIT_N_FEEDBACK_TERMS];
static int useFeedback=1;
static bool isStarted=false;
static void (*actuatorPosHandler)(HCCpg_t*, int);

static void setActuatorPos(HCCpg_t* cpg, int actuator) {
	if(actuatorPosHandler==NULL) {
		float goal = CpgSuper_getX((CpgSuper_t*)cpg);
		setActuatorPosition(goal,actuator);
	}
	else {
		actuatorPosHandler(cpg, actuator);
	}
}
HCCpg_t* CpgGait_getClockCpg() {
	return cpgGlobal;
}

HCCpg_t* CpgGait_getLocalCpg(int index) {
	return cpgLocal[index];
}
void CpgGait_setActuatorPosHandler( void (handler)(HCCpg_t*, int)) {
	actuatorPosHandler =  handler;
}

void CpgGait_setUseFeedback(int feedback) {
	useFeedback = feedback;
}

void CpgGait_setAmplitude(float amplitude, int index) {
	CpgSuper_setAmplitude((CpgSuper_t*)cpgLocal[index], amplitude);
}

float CpgGait_getAmplitude(int index) {
	return CpgSuper_getAmplitude((CpgSuper_t*)cpgLocal[index]);
}

void CpgGait_setOffset(float offset, int index) {
	CpgSuper_setOffset((CpgSuper_t*)cpgLocal[index], offset, 0);
}
float CpgGait_getOffset(int index) {
	return CpgSuper_getOffsetX((CpgSuper_t*)cpgLocal[index]);
}

void CpgGait_setPhaseShift(float phaseShift, int index) {
	//ase_printf("PhaseShift of %i is set to %f\n", index, phaseShift);
	CpgSuper_setPhaseDifference(l2gCoupling[index],phaseShift);
	CpgSuper_setPhaseDifference(g2lCoupling[index],6.28318531f-phaseShift);
}

float CpgGait_getPhaseShift(int index) {
	return CpgSuper_getPhaseDifference(l2gCoupling[index]);
}

static int getNewUpdateRate(float hz) {
	int minUR = 25; int maxUR = 200;
	int minHz = 0; int maxHz = 2;

	int newUpdateRate = maxUR-(maxUR-minUR)/(maxHz-minHz)*hz;
	newUpdateRate = (newUpdateRate<=minUR)?minUR:newUpdateRate;
	newUpdateRate = (newUpdateRate>maxUR)?maxUR:newUpdateRate;

	/*if(fabs(hz)>0.01f) {
		//newUpdateRate = (int)(100.0f/hz);
		newUpdateRate = (newUpdateRate<=minUR)?minUR:newUpdateRate;
		newUpdateRate = (newUpdateRate>maxUR)?maxUR:newUpdateRate;
	}
	else {
		newUpdateRate = maxUR;
	}*/
	return 25;//newUpdateRate;
}
void CpgGait_setGlobalFrequency(float hz) {
	int newUpdateRate = getNewUpdateRate(hz);
	HCCpg_setUpdateRate(newUpdateRate, cpgGlobal);
	for(int index=0;index<getNumberOfActuators();index++) {
		if(HCCpg_getUpdateRate(cpgLocal[index])!=newUpdateRate) {
			HCCpg_setUpdateRate(newUpdateRate, cpgLocal[index]);
		}
		if(CpgSuper_getFrequency((CpgSuper_t*)cpgLocal[index]) != hz) {
			CpgSuper_setFrequency(hz, (CpgSuper_t*) cpgLocal[index]);
		}
	}
	if(CpgSuper_getFrequency((CpgSuper_t*)cpgGlobal) != hz) {
		CpgSuper_setFrequency(hz, (CpgSuper_t*) cpgGlobal);
	}
	if(HCCpg_getUpdateRate(cpgGlobal)!=newUpdateRate) {
		HCCpg_setUpdateRate(newUpdateRate, cpgGlobal);
	}
	//ase_printf("New update rate = %i\n", newUpdateRate);

}

void CpgGait_setFrequency(float hz, int index) {
	int newUpdateRate = getNewUpdateRate(hz);
	if(HCCpg_getUpdateRate(cpgLocal[index])!=newUpdateRate) {
		ase_printf("new update rate = %i\n", newUpdateRate);
		HCCpg_setUpdateRate(newUpdateRate, cpgLocal[index]);
		HCCpg_setUpdateRate(newUpdateRate, cpgGlobal);
	}
	CpgSuper_setFrequency(hz, (CpgSuper_t*) cpgLocal[index]);
	CpgSuper_setFrequency(hz, (CpgSuper_t*) cpgGlobal);
}

float CpgGait_getFrequency(int index) {
	return CpgSuper_getFrequency((CpgSuper_t*) cpgGlobal);
}

void CpgGait_setFeedbackTerm(float term, int nr, int actuatorIndex) {
	feedbackTerms[actuatorIndex][nr] = term;
}

float CpgGait_getFeedbackTerm(int nr, int index) {
	return feedbackTerms[index][nr];
}

void CpgGait_setController(float* controller) {
	int i,j, index=0;
	for(i=0;i<getNumberOfActuators();i++) {
		CpgGait_setFrequency(controller[index++], i);
		CpgGait_setAmplitude(controller[index++], i);
		CpgGait_setPhaseShift(controller[index++], i);
		CpgGait_setOffset(controller[index++], i);
	}
	for(i=0;i<getNumberOfActuators();i++) {
		for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
			CpgGait_setFeedbackTerm(controller[index++], j, i);
		}
	}
}

void CpgGait_getController(float* controller) {
	int i,j,index=0;
	for(i=0;i<getNumberOfActuators();i++) {
		controller[index++] = CpgSuper_getAmplitude((CpgSuper_t*) cpgLocal[i]);
		controller[index++] = CpgSuper_getPhaseDifference(l2gCoupling[i]);
		controller[index++] = CpgSuper_getFrequency((CpgSuper_t*) cpgLocal[i]);
		controller[index++] = CpgSuper_getOffsetX((CpgSuper_t*) cpgLocal[i]);
	}
	for(i=0;i<getNumberOfActuators();i++) {
		for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
			controller[index++] = feedbackTerms[i][j];
		}
	}
}

static void addSensorFeedback() {
	int i=0;
	for(i=0;i<getNumberOfActuators();i++) {
		float encoderVal = getActuatorPosition(i);
		float encoderError = CpgSuper_getX((CpgSuper_t*)cpgLocal[i])-encoderVal;
		HCCpg_coupleDirectlyX(feedbackTerms[i][0]*encoderVal,cpgLocal[i]);
		HCCpg_coupleDirectlyX(feedbackTerms[i][1]*encoderError,cpgLocal[i]);
		HCCpg_coupleDirectlyY(feedbackTerms[i][2]*encoderVal,cpgLocal[i]);
		HCCpg_coupleDirectlyY(feedbackTerms[i][3]*encoderError,cpgLocal[i]);
	}
}


static void cpg_updated(char* topic, Event_t* event)  {
	int i;
	for(i=0;i<getNumberOfActuators();i++)  {
		if(event->val_prt==cpgLocal[i]) {
			setActuatorPos(cpgLocal[i],i);
		}

	}
	if(useFeedback) {
		addSensorFeedback();
	}
}
void CpgGait_setCouplingStrenghts(float g2gStrength, float g2lStrength, float l2gStrength) {
	CpgSuper_setStrength(g2g, g2gStrength);
	int i;
	for(i=2;i<2+getNumberOfActuators();i++) {
		CpgSuper_setStrength(g2lCoupling[i-2],g2lStrength);
		CpgSuper_setStrength(l2gCoupling[i-2],l2gStrength);
	}
}

void CpgGait_start() {
	int i;
	for(i=2;i<2+getNumberOfActuators();i++) {
		Strategy_start((Strategy_t*)cpgLocal[i-2],NULL);
	}
	Strategy_start((Strategy_t*)cpgGlobal,NULL);
	isStarted=true;
}
void CpgGait_stop() {
	int i;
	for(i=2;i<2+getNumberOfActuators();i++) {
		Strategy_stop((Strategy_t*)cpgLocal[i-2],NULL);
	}
	Strategy_stop((Strategy_t*)cpgGlobal,NULL);
	isStarted=false;
}
bool CpgGait_isStarted() {
	return isStarted;
}

void CpgGait_init() {
	EventManager_subscribe(CPG_UPDATED_EVENT,cpg_updated);
	cpgGlobal = HCCpgManager_createCpg(GLOBAL_CPG_LABEL, 0.2f);
	g2g = HCCpgManager_createCoupling(GLOBAL_CPG_LABEL, GLOBAL_CPG_LABEL, 5.0f, 0);//5.0f
	int i,j;
	for(i=2;i<2+getNumberOfActuators();i++) {
		cpgLocal[i-2] = HCCpgManager_createCpg(i, 0.2f);
		l2gCoupling[i-2] = HCCpgManager_createCoupling(i, GLOBAL_CPG_LABEL, 0*1.0f, 0); //5.0f
		g2lCoupling[i-2] = HCCpgManager_createCoupling(GLOBAL_CPG_LABEL, i, 2.0f, 0);//1.0f
		for(j=0;j<MAX_NODE_DEGREE;j++)  {
			l2gCoupling[i-2]->apply_to_channels[j] = 0;
			g2lCoupling[i-2]->apply_to_channels[j] = 0;
		}
		CpgGait_setFrequency(0.05f,i-2);
		CpgGait_setAmplitude(0.50f,i-2);
		CpgGait_setOffset(0,i-2);
	}
}

void CpgGait_logController(int id, FILE* file) {
	#ifdef FPRINTF
	if(file!=0) {
		float var;
		fprintf(file, "%i ",id);
		int i,j;
		for(i=0;i<getNumberOfActuators();i++) {
			var = CpgSuper_getFrequency((CpgSuper_t*) cpgLocal[i]);
			fprintf(file, "%f ",var);

			var = CpgSuper_getAmplitude((CpgSuper_t*) cpgLocal[i]);
			fprintf(file, "%f ",var);

			var = l2gCoupling[i]->phaseDiff;
			fprintf(file, "%f ",var);

			var = CpgSuper_getOffsetX((CpgSuper_t*) cpgLocal[i]);
			fprintf(file, "%f ",var);
		}
		for(i=0;i<getNumberOfActuators();i++) {
			for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
				var = feedbackTerms[i][j];
				fprintf(file, "%f ",var);
			}
		}
		fprintf(file, "\n");
		fflush(file);
	}
	#endif
}

void CpgGait_loadController(int id, FILE* file) {
	#ifdef FPRINTF
	int cID;
	int nActu = getNumberOfActuators();
	float f[nActu],a[nActu],p[nActu],o[nActu], t[nActu][CPGGAIT_N_FEEDBACK_TERMS];
	char line[2000];
	while (fgets(line, 2000, file) != NULL) {
		sscanf(line,"%i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",&cID,&f[0],&a[0],&p[0],&o[0],&f[1],&a[1],&p[1],&o[1],&f[2],&a[2],&p[2],&o[2],&t[0][0],&t[0][1],&t[0][2],&t[0][3],&t[1][0],&t[1][1],&t[1][2],&t[1][3],&t[2][0],&t[2][1],&t[2][2],&t[2][3]);
		//sscanf(line,"%i %f %f %f %f %f %f %f %f %f %f %f %f\n",&cID,&f[0],&a[0],&p[0],&o[0],&f[1],&a[1],&p[1],&o[1],&f[2],&a[2],&p[2],&o[2]);
		if(cID==id) {
			//printf("%i: Found controller: %i %f %f %f %f %f %f %f %f %f %f %f %f\n",getHardwareID(),cID,f[0],a[0],p[0],o[0],f[1],a[1],p[1],o[1],f[2],a[2],p[2],o[2]);
			#ifdef PRINTF
			ase_printf("%i: Found controller: %i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",getHardwareID(),cID,f[0],a[0],p[0],o[0],f[1],a[1],p[1],o[1],f[2],a[2],p[2],o[2],t[0][0],t[0][1],t[0][2],t[0][3],t[1][0],t[1][1],t[1][2],t[1][3],t[2][0],t[2][1],t[2][2],t[2][3]);
			ase_printf("********** %i: %f",getHardwareID(),p[1]);
			#endif
			int i,j;
			for(i=0;i<nActu;i++) {
				CpgGait_setAmplitude(a[i], i);
				CpgGait_setPhaseShift(p[i], i);
				CpgGait_setOffset(o[i], i);
				CpgGait_setFrequency(f[i], i);
				for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
					feedbackTerms[i][j] = t[i][j];
				}
			}
			return;
		}
	}
	#ifdef PRINTF
	ase_printf("%i: Did not find controller with ID=%i\n",getHardwareID(),id);
	#endif
	#endif
}
void CpgGait_loadAverageController(int id, int nControllers, FILE* file) {
	#ifdef FPRINTF
	int cID;
	int nActu = getNumberOfActuators();
	float f[nActu],a[nActu],p[nActu],o[nActu], t[nActu][CPGGAIT_N_FEEDBACK_TERMS];
	float fsum[nActu],asum[nActu],psum[nActu],osum[nActu], tsum[nActu][CPGGAIT_N_FEEDBACK_TERMS];

	char line[2000];
	int i,n,j, foundControllers=0;
	for(i=0;i<nActu;i++) {
		asum[i]=0; psum[i]=0;osum[i]=0;fsum[i]=0;
		for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
			tsum[i][j] =0;
		}
	}
	for(n=0;n<nControllers;n++) {
		while (fgets(line, 2000, file) != NULL) {
			sscanf(line,"%i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",&cID,&f[0],&a[0],&p[0],&o[0],&f[1],&a[1],&p[1],&o[1],&f[2],&a[2],&p[2],&o[2],&f[3],&a[3],&p[3],&o[3],&f[4],&a[4],&p[4],&o[4],&f[5],&a[5],&p[5],&o[5],&f[6],&a[6],&p[6],&o[6],&f[7],&a[7],&p[7],&o[7],&t[0][0],&t[0][1],&t[0][2],&t[0][3],&t[1][0],&t[1][1],&t[1][2],&t[1][3],&t[2][0],&t[2][1],&t[2][2],&t[2][3]);
			ase_printf("%f %f %f %f %f %f %f %f\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
			if(cID==(id+n)) {
				foundControllers++;
				for(i=0;i<nActu;i++) {
					asum[i] += a[i];
					psum[i] += p[i];
					osum[i] += o[i];
					fsum[i] += f[i];
					for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
						tsum[i][j] += t[i][j];
					}
				}
				break;
			}
		}
	}
	if(nControllers==foundControllers) {
		#ifdef PRINTF
		ase_printf("%i: Found all controller!!!\n",getHardwareID());
		#endif
		int i,j;
		for(i=0;i<nActu;i++) {
			CpgGait_setAmplitude(asum[i]/nControllers, i);
			CpgGait_setPhaseShift(psum[i]/nControllers, i);
			CpgGait_setOffset(osum[i]/nControllers, i);
			CpgGait_setFrequency(fsum[i]/nControllers, i);
			for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
				feedbackTerms[i][j] = tsum[i][j]/nControllers;
			}
		}
	}
	else {
		#ifdef PRINTF
		ase_printf("%i: Did not find one or more controllers (%i found vs %i required)\n",getHardwareID(),foundControllers,nControllers);
		#endif
	}
	#endif
}

void CpgGait_loadAverageController_old(int id1, int id2, FILE* file) {
	#ifdef FPRINTF
	int cID;
	int nActu = getNumberOfActuators();
	float f[nActu],a[nActu],p[nActu],o[nActu], t[nActu][CPGGAIT_N_FEEDBACK_TERMS];
	float f2[nActu],a2[nActu],p2[nActu],o2[nActu], t2[nActu][CPGGAIT_N_FEEDBACK_TERMS];
	char line[2000];
	FILE* startOfFile = file;
	int nControllers =0;
	while (fgets(line, 2000, file) != NULL) {
		sscanf(line,"%i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",&cID,&f[0],&a[0],&p[0],&o[0],&f[1],&a[1],&p[1],&o[1],&f[2],&a[2],&p[2],&o[2],&t[0][0],&t[0][1],&t[0][2],&t[0][3],&t[1][0],&t[1][1],&t[1][2],&t[1][3],&t[2][0],&t[2][1],&t[2][2],&t[2][3]);
		if(cID==id1) {
			nControllers++;
			break;
		}
	}
	file = startOfFile;
	while (fgets(line, 2000, file) != NULL) {
		sscanf(line,"%i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",&cID,&f2[0],&a2[0],&p2[0],&o2[0],&f2[1],&a2[1],&p2[1],&o2[1],&f2[2],&a2[2],&p2[2],&o2[2],&t2[0][0],&t2[0][1],&t2[0][2],&t2[0][3],&t2[1][0],&t2[1][1],&t2[1][2],&t2[1][3],&t2[2][0],&t2[2][1],&t2[2][2],&t2[2][3]);
		if(cID==id2) {
			nControllers++;
			break;
		}
	}
	if(nControllers==2) {
		#ifdef PRINTF
		ase_printf("%i: Found controller!!!\n",getHardwareID());
		#endif
		int i,j;
		for(i=0;i<nActu;i++) {
			CpgGait_setAmplitude((a[i]+a2[i])/2.0f, i);
			CpgGait_setPhaseShift((p[i]+p2[i])/2.0f, i);
			CpgGait_setOffset((o[i]+o2[i])/2.0f, i);
			CpgGait_setFrequency((f[i]+f2[i])/2.0f, i);
			for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
				feedbackTerms[i][j] = (t[i][j]+t2[i][j])/2.0f;
			}
		}
	}
	else {
		#ifdef PRINTF
		ase_printf("%i: Did not find one or more controllers ID=%i and ID=%i \n",getHardwareID(),id1,id2);
		#endif
	}
	#endif
}

void CpgGait_logCpg(FILE* log) {
	#ifdef FPRINTF
	fprintf(log,"%f ", getLocalTime());
	fprintf(log,"%f ", CpgSuper_getX((CpgSuper_t*) cpgGlobal));
	int i;
	for(i=0;i<getNumberOfActuators();i++) {
		fprintf(log,"%f ", CpgSuper_getX((CpgSuper_t*) cpgLocal[i]));
		fprintf(log,"%f ", getActuatorPosition(i));
	}
	fprintf(log,"\n");
	fflush(log);
	#endif
}
