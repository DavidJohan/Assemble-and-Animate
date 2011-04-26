/* DynaRole C code generated for SnakeShiftHead */
/* DynaRole C statemachine generated for SnakeShiftHead */
#include <stdbool.h> 
#include <stdio.h> 
#include <stdint.h>
#include <math.h>
#include <ase/MTRANApi.h>
#include <ase/SnakeShiftHead.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>

#define CENTER_ERROR_TOLERANCE	5
#define TRUE	1
#define FALSE	0

static uint8_t token = 255;
static int myID=-1;
static bool isDone = false; 
static bool doneRotatingTo(int actuator, int goal) { 
	int error = (mtranApi_getAngularPositionDegrees(actuator)-goal);
	while(error>360) error -=360; 
	while(error<-360) error +=360;  
	if(error>180) error = 360-error;
	if(error<-180) error = -(380+error);
	
	if(mtranApi_isRotating(actuator)) {
		#ifdef PRINTF
//		printf("%i: Is rotating (error = %f)\n",myID, fabs(error));
		#endif
		return false; 
	}
	
	if(fabs(error)>CENTER_ERROR_TOLERANCE) {
		#ifdef PRINTF
	//	printf("%i: Rotation error too large (error = %f)\n",myID, fabs(error));
		#endif		
		return false;
	}
	#ifdef PRINTF
	printf("%i: Rotation Done :-) (error = %f)\n",myID, fabs(error));
	#endif	
	return true;
}

static void rotateDirTo(int actuator, int to, int direction) {
	#ifdef PRINTF
	printf("%i: Rotating from %i to %i direction %i...\n", myID, mtranApi_getAngularPositionDegrees(actuator), to, direction);
	#endif
	//mtranApi_rotateToDegreeInDegrees(to);
	mtranApi_rotateDirToInDegrees(actuator,to, direction);
}


static void connect(int connector) {
	#ifdef PRINTF
	printf("%i: Connecting to connector %i...\n",myID,connector);
	#endif
		
	mtranApi_connect(connector);
}

static void disconnect(int connector) {
	#ifdef PRINTF
	printf("%i: Disconnecting connector %i...\n",myID,connector);
	#endif
	mtranApi_disconnect(connector);
}

static int notDoneConnecting(int connector) {
	if(!mtranApi_isOtherConnectorNearby(connector)) {
		#ifdef PRINTF
		printf("  %i: no connector nearby!\n",myID);
		#endif
	}
	return !mtranApi_isConnected(connector);
} 
 
static int notDoneDisconnecting(int connector) {
	#ifdef PRINTF
	printf("%i: Disconnecting from connector %i...\n",myID,connector);
	#endif	
	if(!mtranApi_isOtherConnectorNearby(connector)) {
		#ifdef PRINTF
		printf("  %i: no connector nearby!\n",myID);
		#endif	
	}
	return mtranApi_isConnected(connector);
}

static void stateMachine() { 
    if(token == 255) { /* try to see if there's a new state for me */
      token = DSManager_getMyNewState();
	  if(token!=255) {
	  	#ifdef PRINTF
		printf("%i: Now performing state %i\n", myID, token);
		#endif	   
	  }
    }

	
	switch(token) {
	case 0: token = 1; /* fall-through */
    case 1: /* Module M2 */
      rotateDirTo(0,180,TRUE);
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
      if(!doneRotatingTo(0,180)) break;
      token = 3; /* fall-through */
    case 3: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(3);
#endif
      rotateDirTo(1,0,FALSE);
      token = 4;
      break;
    case 4:
      DSManager_sendState(6,6);
#ifdef PENDING_STATE
      token = 5; /* fall-through */
    case 5:
      if(!doneRotatingTo(1,0)) break;
      DSManager_removePendingState(3);
#endif
      token = 255;
      break;
    case 6: /* Module M6 */
      rotateDirTo(0,180,TRUE);
      token = 7;
      break;
    case 7:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(0,180)) break;
      token = 8; /* fall-through */
    case 8: /* Module M6 */
      rotateDirTo(1,0,FALSE);
      token = 9;
      break;
    case 9:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(1,0)) break;
      DSManager_sendState(10,0);
      token = 255;
      break;
    case 10: /* Module M0 */
      connect(0 );
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
      if(notDoneConnecting(0 )) break;
      DSManager_sendState(12,1);
      token = 255;
      break;
    case 12: /* Module M1 */
      disconnect(0 );
      token = 13;
      break;
    case 13:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneDisconnecting(0 )) break;
      DSManager_sendState(14,2);
      token = 255;
      break;
    case 14: /* Module M2 */
      rotateDirTo(0,90,FALSE);
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
      if(!doneRotatingTo(0,90)) break;
      token = 16; /* fall-through */
    case 16: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(16);
#endif
      rotateDirTo(1,90,TRUE);
      token = 17;
      break;
    case 17:
      DSManager_sendState(19,6);
#ifdef PENDING_STATE
      token = 18; /* fall-through */
    case 18:
      if(!doneRotatingTo(1,90)) break;
      DSManager_removePendingState(16);
#endif
      token = 255;
      break;
    case 19: /* Module M6 */
      rotateDirTo(0,90,FALSE);
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
      if(!doneRotatingTo(0,90)) break;
      token = 21; /* fall-through */
    case 21: /* Module M6 */
      rotateDirTo(1,90,TRUE);
      token = 22;
      break;
    case 22:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(1,90)) break;
      token = 23; /* fall-through */
    case 23: /* Module M6 */
      rotateDirTo(1,0,FALSE);
      token = 24;
      break;
    case 24:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(1,0)) break;
      token = 25; /* fall-through */
    case 25: /* Module M6 */
#ifdef PENDING_STATE
      DSManager_addPendingState(25);
#endif
      rotateDirTo(0,180,TRUE);
      token = 26;
      break;
    case 26:
      DSManager_sendState(28,2);
#ifdef PENDING_STATE
      token = 27; /* fall-through */
    case 27:
      if(!doneRotatingTo(0,180)) break;
      DSManager_removePendingState(25);
#endif
      token = 255;
      break;
    case 28: /* Module M2 */
      rotateDirTo(1,0,FALSE);
      token = 29;
      break;
    case 29:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(1,0)) break;
      token = 30; /* fall-through */
    case 30: /* Module M2 */
      rotateDirTo(0,180,TRUE);
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
      if(!doneRotatingTo(0,180)) break;
      DSManager_sendState(32,1);
      token = 255;
      break;
    case 32: /* Module M1 */
      connect(0 );
      token = 33;
      break;
    case 33:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneConnecting(0 )) break;
      DSManager_sendState(34,0);
      token = 255;
      break;
    case 34: /* Module M0 */
      disconnect(0 );
      token = 35;
      break;
    case 35:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneDisconnecting(0 )) break;
      DSManager_sendState(36,6);
      token = 255;
      break;
    case 36: /* Module M6 */
      rotateDirTo(1,90,TRUE);
      token = 37;
      break;
    case 37:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(1,90)) break;
      token = 38; /* fall-through */
    case 38: /* Module M6 */
#ifdef PENDING_STATE
      DSManager_addPendingState(38);
#endif
      rotateDirTo(0,90,FALSE);
      token = 39;
      break;
    case 39:
      DSManager_sendState(41,2);
#ifdef PENDING_STATE
      token = 40; /* fall-through */
    case 40:
      if(!doneRotatingTo(0,90)) break;
      DSManager_removePendingState(38);
#endif
      token = 255;
      break;
    case 41: /* Module M2 */
      rotateDirTo(1,90,TRUE);
      token = 42;
      break;
    case 42:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(1,90)) break;
      token = 43; /* fall-through */
    case 43: /* Module M2 */
      rotateDirTo(0,90,FALSE);
      token = 44;
      break;
    case 44:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(0,90)) break;

	
	DSManager_sendState(254,-1);
	token=255;
	break;
	
	case 255:
		if(DSManager_getGlobalState()==254) {
			if(!isDone) {
			  	#ifdef PRINTF
				printf("%i: i am done...\n",myID);
				#endif	   
			}
			isDone = true;
		}
		break;
	}
}

void SnakeShiftHead_reset() {
    if ( myID == 2 ) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }    
    DSManager_reset();
}

void SnakeShiftHead_init(int self_id) {
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
	
	SnakeShiftHead_reset();
	isDone = false;
}

	
int SnakeShiftHead_act() {
	stateMachine(); 
	return 0;
}

bool SnakeShiftHead_isDone() {
	return isDone;
}
