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
    case 1: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(1);
#endif
      atronApi_rotateToDegreeInDegrees(0);
      token = 2;
      break;
    case 2:
      DSManager_sendState(4,3);
#ifdef PENDING_STATE
      token = 3; /* fall-through */
    case 3:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(1);
#endif
      token = 255;
      break;
    case 4: /* Module M3 */
#ifdef PENDING_STATE
      DSManager_addPendingState(4);
#endif
      atronApi_rotateToDegreeInDegrees(0);
      token = 5;
      break;
    case 5:
      DSManager_sendState(7,6);
#ifdef PENDING_STATE
      token = 6; /* fall-through */
    case 6:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(4);
#endif
      token = 255;
      break;
    case 7: /* Module M6 */
#ifdef PENDING_STATE
      DSManager_addPendingState(7);
#endif
      atronApi_rotateToDegreeInDegrees(0);
      token = 8;
      break;
    case 8:
      DSManager_sendState(10,7);
#ifdef PENDING_STATE
      token = 9; /* fall-through */
    case 9:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(7);
#endif
      token = 255;
      break;
    case 10: /* Module M7 */
      atronApi_rotateToDegreeInDegrees(0);
      token = 11;
      break;
    case 11:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(0)) break;
      DSManager_sendState(12,1);
      token = 255;
      break;
    case 12: /* Module M1 */
      /* { */
        atronApi_connect(4 );
        atronApi_connect(6 );
      /* } */
      token = 13;
      break;
    case 13:
        // local blocking handlers:
        if(!atronApi_isConnected(4 )) break;
        if(!atronApi_isConnected(6 )) break;
      DSManager_sendState(14,5);
      token = 255;
      break;
    case 14: /* Module M5 */
      atronApi_connect(4 );
      token = 15;
      break;
    case 15:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!atronApi_isConnected(4 )) break;

		isDone = true;
		break;
    }
}

void SnakeToQuadruped_reset() {
    if ( myID == 2 ) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }    
    DSManager_reset();
}

void SnakeToQuadruped_init(int self_id) {
    uint32_t address;
    if((self_id==0)) address = 0;
    else if((self_id==1)) address = 1;
    else if((self_id==2)) address = 2;
    else if((self_id==3)) address = 3;
    else if((self_id==4)) address = 4;
    else if((self_id==5)) address = 5;
    else if((self_id==6)) address = 6;
    else if((self_id==7)) address = 7;

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
