#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <ase/SnakeToQuadruped.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>
#include <ase/IDManager.h>


uint8_t token = 255;
int myID=-1;

bool doneRotatingTo(int goal) { 
	if(atronApi_isRotating()) return false;
	
	int error = (atronApi_getAngularPositionDegrees()-goal)%360;
	if(fabs(error)>5) return false;
	
	return true;
}

int stateMachine() {
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
		if(DSManager_pendingStatesPresent()) break;
		printf("%i: Pending state done\n", myID);
		token = 255;
		break;
	}
	return token!=255;
}

void SnakeToQuadruped_reset() {
	if(myID == 1) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }
    DSManager_reset();
}

void SnakeToQuadruped_init(int id) {
	myID = id;
	DSManager_init(id);
	SnakeToQuadruped_reset();
}
	
int SnakeToQuadruped_act() {
	return stateMachine();
}

bool SnakeToQuadruped_isDone() {
	return false;
}
