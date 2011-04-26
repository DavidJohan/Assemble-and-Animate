/* DynaRole C code generated for robodays_mini */
/* DynaRole C statemachine generated for robodays_mini */
#include <stdbool.h> 
#include <stdio.h> 
#include <stdint.h>
#include <math.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/robodays_mini.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>

#define CENTER_ERROR_TOLERANCE	5
#define TRUE	1
#define FALSE	0

static uint8_t token = 255;
static int myID=-1;
static bool isDone = false; 

void robodays_mini_reset_sequence();
void robodays_mini_reset_state();

static bool doneRotatingTo(int goal) { 
	int error = (atronApi_getAngularPositionDegrees()-goal);
	while(error>360) error -=360; 
	while(error<-360) error +=360;  
	if(error>180) error = 360-error;
	if(error<-180) error = -(380+error);
	
	if(atronApi_isRotating()) {
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

static void rotateDirTo(int to, int direction) {
	#ifdef PRINTF
	printf("%i: Rotating from %i to %i direction %i...\n", myID, atronApi_getAngularPositionDegrees(), to, direction);
	#endif
	//atronApi_rotateToDegreeInDegrees(to);
	atronApi_rotateDirToInDegrees(to, direction);
}


static void connect(int connector) {
	#ifdef PRINTF
	printf("%i: Connecting to connector %i...\n",myID,connector);
	#endif
		
	atronApi_connect(connector);
}

static void disconnect(int connector) {
	#ifdef PRINTF
	printf("%i: Disconnecting connector %i...\n",myID,connector);
	#endif
	atronApi_disconnect(connector);
}

static int notDoneConnecting(int connector) {
	if(!atronApi_isOtherConnectorNearby(connector)) {
		#ifdef PRINTF
		printf("  %i: no connector nearby!\n",myID);
		#endif
	}
	return !atronApi_isConnected(connector);
} 
 
static int notDoneDisconnecting(int connector) {
	#ifdef PRINTF
	printf("%i: Disconnecting from connector %i...\n",myID,connector);
	#endif	
	if(!atronApi_isOtherConnectorNearby(connector)) {
		#ifdef PRINTF
		printf("  %i: no connector nearby!\n",myID);
		#endif	
	}
	return atronApi_isConnected(connector);
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
    case 1: /* Module M1 */
      disconnect(0);
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
      if(notDoneDisconnecting(0)) break;
      DSManager_sendState(3,0);
      token = 255;
      break;
    case 3: /* Module M0 */
#ifdef PENDING_STATE
      DSManager_addPendingState(3);
#endif
      rotateDirTo(180,TRUE);
      token = 4;
      break;
    case 4:
      DSManager_sendState(6,2);
#ifdef PENDING_STATE
      token = 5; /* fall-through */
    case 5:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(3);
#endif
      token = 255;
      break;
    case 6: /* Module M2 */
      rotateDirTo(180,TRUE);
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
      if(!doneRotatingTo(180)) break;
      DSManager_sendState(8,0);
      token = 255;
      break;
    case 8: /* Module M0 */
#ifdef PENDING_STATE
      DSManager_addPendingState(8);
#endif
      rotateDirTo(0,TRUE);
      token = 9;
      break;
    case 9:
      DSManager_sendState(11,2);
#ifdef PENDING_STATE
      token = 10; /* fall-through */
    case 10:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(8);
#endif
      token = 255;
      break;
    case 11: /* Module M2 */
      rotateDirTo(0,TRUE);
      token = 12;
      break;
    case 12:
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
      DSManager_sendState(13,1);
      token = 255;
      break;
    case 13: /* Module M1 */
      connect(0);
      token = 14;
      break;
    case 14:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneConnecting(0)) break;
      DSManager_sendState(15,0);
      token = 255;
      break;
    case 15: /* Module M0 */
      disconnect(6);
      token = 16;
      break;
    case 16:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneDisconnecting(6)) break;
      DSManager_sendState(17,1);
      token = 255;
      break;
    case 17: /* Module M1 */
#ifdef PENDING_STATE
      DSManager_addPendingState(17);
#endif
      rotateDirTo(180,TRUE);
      token = 18;
      break;
    case 18:
      DSManager_sendState(20,2);
#ifdef PENDING_STATE
      token = 19; /* fall-through */
    case 19:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(17);
#endif
      token = 255;
      break;
    case 20: /* Module M2 */
      rotateDirTo(180,TRUE);
      token = 21;
      break;
    case 21:
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
      DSManager_sendState(22,1);
      token = 255;
      break;
    case 22: /* Module M1 */
#ifdef PENDING_STATE
      DSManager_addPendingState(22);
#endif
      rotateDirTo(0,TRUE);
      token = 23;
      break;
    case 23:
      DSManager_sendState(25,2);
#ifdef PENDING_STATE
      token = 24; /* fall-through */
    case 24:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(22);
#endif
      token = 255;
      break;
    case 25: /* Module M2 */
      rotateDirTo(0,TRUE);
      token = 26;
      break;
    case 26:
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
      DSManager_sendState(27,0);
      token = 255;
      break;
    case 27: /* Module M0 */
      connect(6);
      token = 28;
      break;
    case 28:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneConnecting(6)) break;
      token = 29; /* fall-through */
    case 29: /* Module M0 */
      disconnect(6);
      token = 30;
      break;
    case 30:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneDisconnecting(6)) break;
      DSManager_sendState(31,2);
      token = 255;
      break;
    case 31: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(31);
#endif
      rotateDirTo(180,FALSE);
      token = 32;
      break;
    case 32:
      DSManager_sendState(34,1);
#ifdef PENDING_STATE
      token = 33; /* fall-through */
    case 33:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(31);
#endif
      token = 255;
      break;
    case 34: /* Module M1 */
      rotateDirTo(180,FALSE);
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
      if(!doneRotatingTo(180)) break;
      DSManager_sendState(36,2);
      token = 255;
      break;
    case 36: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(36);
#endif
      rotateDirTo(0,FALSE);
      token = 37;
      break;
    case 37:
      DSManager_sendState(39,1);
#ifdef PENDING_STATE
      token = 38; /* fall-through */
    case 38:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(36);
#endif
      token = 255;
      break;
    case 39: /* Module M1 */
      rotateDirTo(0,FALSE);
      token = 40;
      break;
    case 40:
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
      DSManager_sendState(41,0);
      token = 255;
      break;
    case 41: /* Module M0 */
      connect(6);
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
      if(notDoneConnecting(6)) break;
      DSManager_sendState(43,1);
      token = 255;
      break;
    case 43: /* Module M1 */
      disconnect(0);
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
      if(notDoneDisconnecting(0)) break;
      DSManager_sendState(45,2);
      token = 255;
      break;
    case 45: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(45);
#endif
      rotateDirTo(180,FALSE);
      token = 46;
      break;
    case 46:
      DSManager_sendState(48,0);
#ifdef PENDING_STATE
      token = 47; /* fall-through */
    case 47:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(45);
#endif
      token = 255;
      break;
    case 48: /* Module M0 */
      rotateDirTo(180,FALSE);
      token = 49;
      break;
    case 49:
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
      DSManager_sendState(50,2);
      token = 255;
      break;
    case 50: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(50);
#endif
      rotateDirTo(0,FALSE);
      token = 51;
      break;
    case 51:
      DSManager_sendState(53,0);
#ifdef PENDING_STATE
      token = 52; /* fall-through */
    case 52:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(50);
#endif
      token = 255;
      break;
    case 53: /* Module M0 */
      rotateDirTo(0,FALSE);
      token = 54;
      break;
    case 54:
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
      DSManager_sendState(55,1);
      token = 255;
      break;
    case 55: /* Module M1 */
      connect(0);
      token = 56;
      break;
    case 56:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneConnecting(0)) break;

#ifdef LOOP
      token = 255;
      robodays_mini_reset_sequence();
      break;
#endif//LOOP
	
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

void robodays_mini_init(int self_id) {
    uint32_t address;
    if((self_id==0)) address = 0;
    else if((self_id==1)) address = 1;
    else if((self_id==2)) address = 2;
    else if((self_id==3)) address = 3;

    else address = 127;
	myID = address;
	DSManager_init(myID);
	
    atronApi_setLeds(myID);
	robodays_mini_reset_state();
	isDone = false;
}

void robodays_mini_reset_sequence() {
    if ( myID == 1 ) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }    
    DSManager_reset_sequence();
}

void robodays_mini_reset_state() {
    if ( myID == 1 ) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }    
    DSManager_reset_state();
}

	
int robodays_mini_act() {
	stateMachine(); 
	return 0;
}

bool robodays_mini_isDone() {
	return isDone;
}
