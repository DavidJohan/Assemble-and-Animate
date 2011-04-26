#include <stdio.h>
#include <stdlib.h>
#include <ase/CentralPatternGenerator.h>
#include <ase/CPGManager.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/CPGManager.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/targets/AbstractModuleApi.h>

#define GLOBAL_CPG_LABEL	1
#define LOCAL_CPG_LABEL		2

cpg_struct* cpgGlobal;
cpg_struct* cpgLocal;
cpgCoupling_struct *g2lCoupling, *l2gCoupling;

int toRunningLeds(int val) {
	if(val<=0) return 0x00;
	else if(val<=32) return 0x01;
	else if(val<=64) return 0x03;
	else if(val<=95) return 0x07;
	else if(val<=127) return 0x0F;
	else if(val<=159) return 0x1F;
	else if(val<=191) return 0x3F;
	else if(val<=223) return 0x7F;
	else return 0xFF;
}

void setActuatorPos(cpg_struct* cpg) {
	int goal = (int)(cpg_getX(cpg)); //scale to -amplitude to amplitude
	if(goal<0) goal = 360+goal; //scale to 360-amplitude to amplitude
	atronApi_rotateToDegreeInDegrees(goal);
	//if(getHardwareID()==2) printf("%f: Actuator pos = %i \n", getLocalTime(),goal);
}

void CpgGait_setAmplitude(float amplitude) {
	cpgLocal->amplitude = amplitude;
}

void CpgGait_setOffset(float offset) {
	cpgLocal->offset = offset;
}

void CpgGait_setPhaseShift(float phaseShift) {
	l2gCoupling->phaseDiff = phaseShift;
	g2lCoupling->phaseDiff = 6.283f-phaseShift;
}

void CpgGait_setFrequency(float hz) {
	CPGManager_setFrequency(GLOBAL_CPG_LABEL, hz);
	CPGManager_setFrequency(LOCAL_CPG_LABEL, hz);
}

void CpgGait_init() {
	atronApi_yield();
	cpgGlobal = CPGManager_createCPG(GLOBAL_CPG_LABEL, 0.2f);
	cpgLocal = CPGManager_createCPG(LOCAL_CPG_LABEL, 0.2f);

	CPGManager_createCoupling(GLOBAL_CPG_LABEL, GLOBAL_CPG_LABEL, 1, 0);

	l2gCoupling = CPGManager_createCoupling(LOCAL_CPG_LABEL, GLOBAL_CPG_LABEL, 1, 0);
	g2lCoupling = CPGManager_createCoupling(GLOBAL_CPG_LABEL, LOCAL_CPG_LABEL, 1, 0);
	int i;
	for(i=0;i<8;i++)  {
		l2gCoupling->apply_to_channels[i] = 0;
		g2lCoupling->apply_to_channels[i] = 0;
	}
	CpgGait_setFrequency(0.20f);//0.15f
}

void CpgGait_act()  {
	if(CPGManager_act()>0) {
		//int cpgX = (int) (255.0*(1.0+cpg_getX(cpg1))/(2.0*cpg1->amplitude)); //scale to 0-255
		//atronApi_setLeds(toRunningLeds(cpgX));
		setActuatorPos(cpgLocal);
	}
}
