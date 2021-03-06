/* DynaRole C code generated for SnakeToQuadruped_8M */
/* DynaRole C statemachine generated for SnakeToQuadruped_8M */
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <ase/AtronApi.h>
#include <ase/SnakeToQuadruped_8M.h>
#include <ase/DistributedStateManager.h>
#include <ase/IDManager.h>

#define CENTER_ERROR_TOLERANCE	5

static uint8_t token = 255;
static int myID=-1;
static bool isDone = false; 
static bool doneRotatingTo(int goal) { 
	if(atronApi_isRotating()) return false;
	
	int error = (atronApi_getAngularPositionDegrees()-goal)%360;
	if(fabs(error)>CENTER_ERROR_TOLERANCE) return false;
	
	return true;
}


static void stateMachine() {
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
      atronApi_rotateToDegreeInDegrees(180);
      token = 2;
      break;
    case 2:
      DSManager_sendState(4,2);
#ifdef PENDING_STATE
      token = 3; /* fall-through */
    case 3:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(1);
#endif
      token = 255;
      break;
    case 4: /* Module M3 */
#ifdef PENDING_STATE
      DSManager_addPendingState(4);
#endif
      atronApi_rotateToDegreeInDegrees(180);
      token = 5;
      break;
    case 5:
      DSManager_sendState(7,5);
#ifdef PENDING_STATE
      token = 6; /* fall-through */
    case 6:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(4);
#endif
      token = 255;
      break;
    case 7: /* Module M6 */
#ifdef PENDING_STATE
      DSManager_addPendingState(7);
#endif
      atronApi_rotateToDegreeInDegrees(180);
      token = 8;
      break;
    case 8:
      DSManager_sendState(10,6);
#ifdef PENDING_STATE
      token = 9; /* fall-through */
    case 9:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(7);
#endif
      token = 255;
      break;
    case 10: /* Module M7 */
      atronApi_rotateToDegreeInDegrees(180);
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
      if(!doneRotatingTo(180)) break;
      DSManager_sendState(12,0);
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
      DSManager_sendState(14,4);
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
      DSManager_sendState(16,2);
      token = 255;
      break;
    case 16: /* Module M3 */
#ifdef PENDING_STATE
      DSManager_addPendingState(16);
#endif
      atronApi_disconnect(6 );
      token = 17;
      break;
    case 17:
      DSManager_sendState(19,6);
#ifdef PENDING_STATE
      token = 18; /* fall-through */
    case 18:
      if(atronApi_isConnected(6 )) break;
      DSManager_removePendingState(16);
#endif
      token = 255;
      break;
    case 19: /* Module M7 */
      atronApi_disconnect(6 );
      token = 20;
      break;
    case 20:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(atronApi_isConnected(6 )) break;
      DSManager_sendState(21,3);
      token = 255;
      break;
    case 21: /* Module M4 */
#ifdef PENDING_STATE
      DSManager_addPendingState(21);
#endif
      atronApi_rotateToDegreeInDegrees(270);
      token = 22;
      break;
    case 22:
      DSManager_sendState(24,0);
#ifdef PENDING_STATE
      token = 23; /* fall-through */
    case 23:
      if(!doneRotatingTo(270)) break;
      DSManager_removePendingState(21);
#endif
      token = 255;
      break;
    case 24: /* Module M1 */
#ifdef PENDING_STATE
      DSManager_addPendingState(24);
#endif
      atronApi_rotateToDegreeInDegrees(90);
      token = 25;
      break;
    case 25:
      DSManager_sendState(27,4);
#ifdef PENDING_STATE
      token = 26; /* fall-through */
    case 26:
      if(!doneRotatingTo(90)) break;
      DSManager_removePendingState(24);
#endif
      token = 255;
      break;
    case 27: /* Module M5 */
#ifdef PENDING_STATE
      DSManager_addPendingState(27);
#endif
      atronApi_rotateToDegreeInDegrees(90);
      token = 28;
      break;
    case 28:
      DSManager_sendState(30,7);
#ifdef PENDING_STATE
      token = 29; /* fall-through */
    case 29:
      if(!doneRotatingTo(90)) break;
      DSManager_removePendingState(27);
#endif
      token = 255;
      break;
    case 30: /* Module M8 */
      atronApi_rotateToDegreeInDegrees(270);
      token = 31;
      break;
    case 31:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(270)) break;
	token = 32;
	break;
     case 32:
	DSManager_sendState(33, 1);
	token = 255;
	break;
    case 33:
	token = 255;
	break;
     case 255:
	if(DSManager_getGlobalState()==33) {
		if(!isDone) printf("%i: i am done...\n",myID);
		isDone = true;
	}
}
}

void SnakeToQuadruped_8M_reset() {
    if ( myID == 1 ) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }    
    DSManager_reset();
}

void SnakeToQuadruped_8M_init(int self_id) {
    uint32_t address;
    if((self_id==1)) address = 0;
    else if((self_id==2)) address = 1;
    else if((self_id==3)) address = 2;
    else if((self_id==4)) address = 3;
    else if((self_id==5)) address = 4;
    else if((self_id==6)) address = 5;
    else if((self_id==7)) address = 6;
    else if((self_id==8)) address = 7;

    else address = 127;
	myID = address;
	DSManager_init(myID);
    atronApi_setLeds(myID);
	SnakeToQuadruped_8M_reset();
	isDone = false;
}

	
int SnakeToQuadruped_8M_act() {
	stateMachine(); 
	return 0;
}

bool SnakeToQuadruped_8M_isDone() {
	return isDone;
}
