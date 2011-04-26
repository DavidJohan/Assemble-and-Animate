#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/SnakeToQuadruped.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>
#include <ase/IDManager.h>

#define CENTER_ERROR_TOLERANCE	5

uint8_t token = 255;
int myID=-1;
bool isDone = false; 
bool doneRotatingTo(int goal) { 
	if(atronApi_isRotating()) return false;
	
	int error = (atronApi_getAngularPositionDegrees()-goal)%360;
	if(fabs(error)>CENTER_ERROR_TOLERANCE) return false;
	
	return true;
}


void stateMachine() {
    if(token == 255) { /* try to see if there's a new state for me */
      token = DSManager_getMyNewState();
	  if(token!=255) printf("%i: Now performing state %i\n", myID, token);
    }
	
	switch(token) {
	case 0: token = 1; /* fall-through */
    case 1: /* Module 1 */
		DSManager_sendState(2, 2);
		token = 255;
		break;
	case 2: /* Module 2 */
		DSManager_addPendingState(2);
		atronApi_rotateToDegreeInDegrees(0);
		DSManager_sendState(4, 3);
		token = 3;
	case 3:
		if(!doneRotatingTo(0)) break;
		DSManager_removePendingState(2);
		token = 255;
		printf("%i: Module done rotating\n", myID);
		break;
	case 4: /* Module 3 */
		DSManager_addPendingState(4);
		atronApi_rotateToDegreeInDegrees(0);
		DSManager_sendState(6, 6);
		token = 5;
	case 5:
		if(!doneRotatingTo(0)) break;
		DSManager_removePendingState(4);
		token = 255;
		printf("%i: Module done rotating\n", myID);
		break;	
	case 6: /* Module 6 */
		DSManager_addPendingState(6);
		atronApi_rotateToDegreeInDegrees(0);
		DSManager_sendState(8, 7);
		token = 7;
	case 7:
		if(!doneRotatingTo(0)) break;
		DSManager_removePendingState(6);
		token = 255;
		printf("%i: Module done rotating\n", myID);
		break;	
	case 8: /* Module 7 */
		DSManager_addPendingState(8);
		atronApi_rotateToDegreeInDegrees(0);
		DSManager_sendState(10, 1);
		token = 9;
	case 9:
		if(!doneRotatingTo(0)) break;
		DSManager_removePendingState(8);
		token = 255;
		printf("%i: Module done rotating\n", myID);
		break;
	case 10: /* Module 1 */
		if(DSManager_pendingStatesPresent()) break;
		DSManager_addPendingState(10);
		int i=0;
		for(i=0;i<8;i++) {
		//	printf("%i: connector %i connected = %i and has neighbor= %i \n",myID,i, atronApi_isConnected(i),atronApi_isOtherConnectorNearby(i));
		}
		atronApi_connect(4);
		atronApi_connect(6);
		DSManager_sendState(12, 5);
		token = 11;
	case 11:
		if(!atronApi_isConnected(4) || !atronApi_isConnected(6)) break;
		DSManager_removePendingState(10);
		token = 255;
		printf("%i: Module done connecting\n", myID);
		break;
	case 12: /* Module 5 */
		DSManager_addPendingState(12);
		atronApi_connect(4);
		DSManager_sendState(8, 14);
		token = 13;
	case 13:
		if(!atronApi_isConnected(4)) break;
		DSManager_removePendingState(12);
		token = 255;
		printf("%i: Module done connecting\n", myID);
		break;
	case 14: /* Module 1 */
		if(DSManager_pendingStatesPresent()) break;
		printf("%i: Self-reconfiguration done...\n", myID);
		isDone = true;
		break;
	}
	
}

void SnakeToQuadruped_reset() {
	if(myID == 8) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }
    DSManager_reset();
}

void SnakeToQuadruped_init(int id) {
	myID = id;
	DSManager_init(id);
	SnakeToQuadruped_reset();
	isDone = false;
}

	
int SnakeToQuadruped_act() {
	stateMachine(); 
	return 0;
}

bool SnakeToQuadruped_isDone() {
	return isDone;
}
