/* DynaRole C code generated for QuadrupedToSnake */
/* DynaRole C statemachine generated for QuadrupedToSnake */
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/QuadrupedToSnake.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>
#include <ase/IDManager.h>

#define CENTER_ERROR_TOLERANCE	5

uint8_t token = 255;
int myID=-1;
bool isDone = false; 


void stateMachine() {
    if(token == 255) { /* try to see if there's a new state for me */
      token = DSManager_getMyNewState();
	  if(token!=255) printf("%i: Now performing state %i\n", myID, token);
    }
	
	switch(token) {
	case 0: token = 1; /* fall-through */
    case 1: /* Module M1 */
      /* { */
        atronApi_disconnect(4 );
        atronApi_disconnect(6 );
      /* } */
      token = 2;
      break;
    case 2:
        // local blocking handlers:
        if(atronApi_isConnected(4 )) break;
        if(atronApi_isConnected(6 )) break;
      DSManager_sendState(3,5);
      token = 255;
      break;
    case 3: /* Module M5 */
      atronApi_disconnect(4 );
      token = 4;
      break;
    case 4:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(atronApi_isConnected(4 )) break;
      DSManager_sendState(5,2);
      token = 255;
      break;
    case 5: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(5);
#endif
      atronApi_rotateToDegreeInDegrees(180);
      token = 6;
      break;
    case 6:
      DSManager_sendState(8,3);
#ifdef PENDING_STATE
      token = 7; /* fall-through */
    case 7:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(5);
#endif
      token = 255;
      break;
    case 8: /* Module M3 */
#ifdef PENDING_STATE
      DSManager_addPendingState(8);
#endif
      atronApi_rotateToDegreeInDegrees(180);
      token = 9;
      break;
    case 9:
      DSManager_sendState(11,6);
#ifdef PENDING_STATE
      token = 10; /* fall-through */
    case 10:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(8);
#endif
      token = 255;
      break;
    case 11: /* Module M6 */
#ifdef PENDING_STATE
      DSManager_addPendingState(11);
#endif
      atronApi_rotateToDegreeInDegrees(180);
      token = 12;
      break;
    case 12:
      DSManager_sendState(14,7);
#ifdef PENDING_STATE
      token = 13; /* fall-through */
    case 13:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(11);
#endif
      token = 255;
      break;
    case 14: /* Module M7 */
      atronApi_rotateToDegreeInDegrees(180);
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
      if(!doneRotatingTo(180)) break;

		isDone = true;
		break;
    }
}

void QuadrupedToSnake_reset() {
    if ( myID == 1 ) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }    
    DSManager_reset();
}

void QuadrupedToSnake_init(int self_id) {
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
	QuadrupedToSnake_reset();
	isDone = false;
}

	
int QuadrupedToSnake_act() {
	stateMachine(); 
	return 0;
}

bool QuadrupedToSnake_isDone() {
	return isDone;
}
