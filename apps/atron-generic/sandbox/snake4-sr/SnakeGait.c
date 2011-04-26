#include <stdio.h>
#include <ase/CentralPatternGenerator.h>
#include <ase/CPGManager.h>
#include <ase/control/behaviors/atron/locomotion/SnakeGait/SnakeGait.h>
#include <ase/CPGManager.h>
#include <ase/targets/atron/AtronApi.h>

#define SNAKE_CPG_LABEL		1
cpg_struct* cpg1;
cpgCoupling_struct *h2tCoupling, *t2hCoupling;
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
	//printf("Actuator pos = %i\n", goal);
}

void SnakeGait_setAmplitude(float amplitude) {
	cpg1->amplitude = amplitude;
}

void SnakeGait_setOffset(float offset) {
	cpg1->offset = offset;
}

void SnakeGait_setPhaseShift(float phaseShift) {
	h2tCoupling->phaseDiff = phaseShift;
	t2hCoupling->phaseDiff = 6.283f-phaseShift;
}

void SnakeGait_setFrequency(float hz) {
	CPGManager_setFrequency(SNAKE_CPG_LABEL, hz);
}

void SnakeGait_init() {
	atronApi_yield();
	
	cpg1 = CPGManager_createCPG(SNAKE_CPG_LABEL, 1);
	h2tCoupling = CPGManager_createCoupling(SNAKE_CPG_LABEL, SNAKE_CPG_LABEL, 1, 0);
	t2hCoupling = CPGManager_createCoupling(SNAKE_CPG_LABEL, SNAKE_CPG_LABEL, 1, 0);
	int i;
	for(i=0;i<4;i++)  {
		h2tCoupling->apply_to_channels[i] = 1;
		t2hCoupling->apply_to_channels[i] = 0;
	}
	for(i=4;i<8;i++)  {	
		h2tCoupling->apply_to_channels[i] = 0;
		t2hCoupling->apply_to_channels[i] = 1;
	}	
	SnakeGait_setAmplitude(60.0f);
	SnakeGait_setOffset(0.0f);
	SnakeGait_setPhaseShift(2.464f);
	SnakeGait_setFrequency(0.15f);
}

void SnakeGait_act()  {
	if(CPGManager_act()>0) {
		int cpgX = (int) (255.0*(1.0+cpg_getX(cpg1))/(2.0*cpg1->amplitude)); //scale to 0-255
		atronApi_setLeds(toRunningLeds(cpgX));
		setActuatorPos(cpg1); 
	}
}
