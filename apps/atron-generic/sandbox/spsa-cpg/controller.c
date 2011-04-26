#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpgManager.h>
#include <ase/control/behaviors/generic/LearningCpgGait/SPSACpgGait/SPSACpgGait.h>
#include <ase/control/strategies/SPSALearning/SPSALearning.h>

void controller_init() {
	printf("%i: Running\n",getHardwareID());
	srand (getRandomSeed());
	SPSACpgGait_init();
	int i;
	for(i=0;i<getNumberOfActuators();i++) {
		float initPos = getActuatorPosition(i);
		SPSACpgGait_setFrequencyInterval(0.15f, 0.15f, 0.15f);
		SPSACpgGait_setAmplitudeInterval(0.1f, 0.1f, 0.10f,i);
		SPSACpgGait_setPhaseshiftInterval(-3.1415f, 0.0f, 3.1415f,i);
		SPSACpgGait_setOffsetInterval(initPos-0.0f, initPos, initPos+0.0f ,i);
		/*SPSACpgGait_setFeedbackTermInterval(-1, 0, 1, 0, i); //x -encoder
		SPSACpgGait_setFeedbackTermInterval(-5, 0, 5, 1, i); //x -encoder error
		SPSACpgGait_setFeedbackTermInterval(-1, 0, 1, 2, i); //y -encoder
		SPSACpgGait_setFeedbackTermInterval(-5, 0, 5, 3, i); //y -encoder error*/
		SPSACpgGait_updateGait();
	}
	SPSACpgGait_setLearningParameters(0.00025f,0.025f);
}

void controller_act()  {
	malloc(100);
	if(SPSACpgGait_act()>0) {

	}
}

void handleMessage(char* message, char messageSize, char channel) {
	if(message[0]==HCCPG_MESSAGE) {
		HCCpgManager_handleMessage(message, messageSize, channel);
	}
	else if(message[0]==LEARNING_MESSAGE) {
		SPSACpgGait_handleMessage(message, messageSize, channel);
	}
	else {
		printf("Warning: Unrecognized message type received %i \n",message[0]);
	}
}
