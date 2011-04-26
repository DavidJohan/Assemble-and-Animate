/* DynaRole C code generated for SnakeToQuadruped */
/* DynaRole C statemachine generated for SnakeToQuadruped */
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
    case 1: /* Module M1 */
      atronApi_rotateToDegreeInDegrees(180);
      token = 2;
      break;
    case 2:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(180)) break;
      token = 3; /* fall-through */
    case 3: /* Module M1 */
      /* { */
        atronApi_connect(6 );
        atronApi_connect(4 );
      /* } */
      token = 4;
      break;
    case 4:
        // local blocking handlers:
        if(!atronApi_isConnected(6 )) break;
        if(!atronApi_isConnected(4 )) break;
      token = 5; /* fall-through */
    case 5: /* Module M1 */
      /* { */
        atronApi_rotateToDegreeInDegrees(0);
        atronApi_disconnect(6 );
        atronApi_disconnect(4 );
      /* } */
      token = 6;
      break;
    case 6:
        // local blocking handlers:
        if(!doneRotatingTo(0)) break;
        if(atronApi_isConnected(6 )) break;
        if(atronApi_isConnected(4 )) break;

		isDone = true;
		break;
    }
}

void SnakeToQuadruped_reset() {
    if ( myID == 0 ) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }    
    DSManager_reset();
}

void SnakeToQuadruped_init(int self_id) {
    uint32_t address;
    if((self_id==1)) address = 0;

    else address = 127;
	myID = address;
	DSManager_init(myID);
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
