/* DynaRole C code generated for robodays_sculpture */
/* DynaRole C statemachine generated for robodays_sculpture */
#include <stdbool.h> 
#include <stdio.h> 
#include <stdint.h>
#include <math.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/robodays_sculpture.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>

#define CENTER_ERROR_TOLERANCE	5
#define TRUE	1
#define FALSE	0

static uint8_t token = 255;
static int myID=-1;
static bool isDone = false; 
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
    case 1: /* Module M6 */
#ifdef PENDING_STATE
      DSManager_addPendingState(1);
#endif
      rotateDirTo(180,TRUE);
      token = 2;
      break;
    case 2:
      DSManager_sendState(4,0);
#ifdef PENDING_STATE
      token = 3; /* fall-through */
    case 3:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(1);
#endif
      token = 255;
      break;
    case 4: /* Module M0 */
#ifdef PENDING_STATE
      DSManager_addPendingState(4);
#endif
      rotateDirTo(90,TRUE);
      token = 5;
      break;
    case 5:
      DSManager_sendState(7,3);
#ifdef PENDING_STATE
      token = 6; /* fall-through */
    case 6:
      if(!doneRotatingTo(90)) break;
      DSManager_removePendingState(4);
#endif
      token = 255;
      break;
    case 7: /* Module M3 */
      rotateDirTo(90,TRUE);
      token = 8;
      break;
    case 8:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(90)) break;
      token = 9; /* fall-through */
    case 9: /* Module M3 */
      rotateDirTo(90,TRUE);
      token = 10;
      break;
    case 10:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(!doneRotatingTo(90)) break;
      DSManager_sendState(11,1);
      token = 255;
      break;
    case 11: /* Module M1 */
      connect(0);
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
      if(notDoneConnecting(0)) break;
      DSManager_sendState(13,3);
      token = 255;
      break;
    case 13: /* Module M3 */
      disconnect(6);
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
      if(notDoneDisconnecting(6)) break;
      DSManager_sendState(15,6);
      token = 255;
      break;
    case 15: /* Module M6 */
#ifdef PENDING_STATE
      DSManager_addPendingState(15);
#endif
      rotateDirTo(0,TRUE);
      token = 16;
      break;
    case 16:
      DSManager_sendState(18,1);
#ifdef PENDING_STATE
      token = 17; /* fall-through */
    case 17:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(15);
#endif
      token = 255;
      break;
    case 18: /* Module M1 */
#ifdef PENDING_STATE
      DSManager_addPendingState(18);
#endif
      rotateDirTo(180,TRUE);
      token = 19;
      break;
    case 19:
      DSManager_sendState(21,0);
#ifdef PENDING_STATE
      token = 20; /* fall-through */
    case 20:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(18);
#endif
      token = 255;
      break;
    case 21: /* Module M0 */
#ifdef PENDING_STATE
      DSManager_addPendingState(21);
#endif
      rotateDirTo(270,FALSE);
      token = 22;
      break;
    case 22:
      DSManager_sendState(24,4);
#ifdef PENDING_STATE
      token = 23; /* fall-through */
    case 23:
      if(!doneRotatingTo(270)) break;
      DSManager_removePendingState(21);
#endif
      token = 255;
      break;
    case 24: /* Module M4 */
#ifdef PENDING_STATE
      DSManager_addPendingState(24);
#endif
      rotateDirTo(90,TRUE);
      token = 25;
      break;
    case 25:
      DSManager_sendState(27,3);
#ifdef PENDING_STATE
      token = 26; /* fall-through */
    case 26:
      if(!doneRotatingTo(90)) break;
      DSManager_removePendingState(24);
#endif
      token = 255;
      break;
    case 27: /* Module M3 */
      rotateDirTo(180,TRUE);
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
      if(!doneRotatingTo(180)) break;
      DSManager_sendState(29,1);
      token = 255;
      break;
    case 29: /* Module M1 */
#ifdef PENDING_STATE
      DSManager_addPendingState(29);
#endif
      connect(2);
      token = 30;
      break;
    case 30:
      DSManager_sendState(32,2);
#ifdef PENDING_STATE
      token = 31; /* fall-through */
    case 31:
      if(notDoneConnecting(2)) break;
      DSManager_removePendingState(29);
#endif
      token = 255;
      break;
    case 32: /* Module M2 */
      connect(4);
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
      if(notDoneConnecting(4)) break;
      DSManager_sendState(34,4);
      token = 255;
      break;
    case 34: /* Module M4 */
#ifdef PENDING_STATE
      DSManager_addPendingState(34);
#endif
      disconnect(6);
      token = 35;
      break;
    case 35:
      DSManager_sendState(37,1);
#ifdef PENDING_STATE
      token = 36; /* fall-through */
    case 36:
      if(notDoneDisconnecting(6)) break;
      DSManager_removePendingState(34);
#endif
      token = 255;
      break;
    case 37: /* Module M1 */
      disconnect(0);
      token = 38;
      break;
    case 38:
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
      DSManager_sendState(39,5);
      token = 255;
      break;
    case 39: /* Module M5 */
#ifdef PENDING_STATE
      DSManager_addPendingState(39);
#endif
      rotateDirTo(180,TRUE);
      token = 40;
      break;
    case 40:
      DSManager_sendState(42,2);
#ifdef PENDING_STATE
      token = 41; /* fall-through */
    case 41:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(39);
#endif
      token = 255;
      break;
    case 42: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(42);
#endif
      rotateDirTo(180,TRUE);
      token = 43;
      break;
    case 43:
      DSManager_sendState(45,1);
#ifdef PENDING_STATE
      token = 44; /* fall-through */
    case 44:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(42);
#endif
      token = 255;
      break;
    case 45: /* Module M1 */
#ifdef PENDING_STATE
      DSManager_addPendingState(45);
#endif
      rotateDirTo(0,FALSE);
      token = 46;
      break;
    case 46:
      DSManager_sendState(48,4);
#ifdef PENDING_STATE
      token = 47; /* fall-through */
    case 47:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(45);
#endif
      token = 255;
      break;
    case 48: /* Module M4 */
      rotateDirTo(180,TRUE);
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
      connect(6);
      token = 51;
      break;
    case 51:
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
      DSManager_sendState(52,1);
      token = 255;
      break;
    case 52: /* Module M1 */
      disconnect(2);
      token = 53;
      break;
    case 53:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneDisconnecting(2)) break;
      DSManager_sendState(54,5);
      token = 255;
      break;
    case 54: /* Module M5 */
#ifdef PENDING_STATE
      DSManager_addPendingState(54);
#endif
      rotateDirTo(0,TRUE);
      token = 55;
      break;
    case 55:
      DSManager_sendState(57,2);
#ifdef PENDING_STATE
      token = 56; /* fall-through */
    case 56:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(54);
#endif
      token = 255;
      break;
    case 57: /* Module M2 */
      rotateDirTo(270,TRUE);
      token = 58;
      break;
    case 58:
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
      DSManager_sendState(59,0);
      token = 255;
      break;
    case 59: /* Module M0 */
      rotateDirTo(0,TRUE);
      token = 60;
      break;
    case 60:
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
      token = 61; /* fall-through */
    case 61: /* Module M0 */
      rotateDirTo(270,FALSE);
      token = 62;
      break;
    case 62:
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
      DSManager_sendState(63,2);
      token = 255;
      break;
    case 63: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(63);
#endif
      rotateDirTo(180,FALSE);
      token = 64;
      break;
    case 64:
      DSManager_sendState(66,5);
#ifdef PENDING_STATE
      token = 65; /* fall-through */
    case 65:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(63);
#endif
      token = 255;
      break;
    case 66: /* Module M5 */
      rotateDirTo(180,FALSE);
      token = 67;
      break;
    case 67:
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
      DSManager_sendState(68,1);
      token = 255;
      break;
    case 68: /* Module M1 */
      connect(2);
      token = 69;
      break;
    case 69:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneConnecting(2)) break;
      DSManager_sendState(70,2);
      token = 255;
      break;
    case 70: /* Module M2 */
      disconnect(6);
      token = 71;
      break;
    case 71:
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
      DSManager_sendState(72,4);
      token = 255;
      break;
    case 72: /* Module M4 */
#ifdef PENDING_STATE
      DSManager_addPendingState(72);
#endif
      rotateDirTo(90,FALSE);
      token = 73;
      break;
    case 73:
      DSManager_sendState(75,1);
#ifdef PENDING_STATE
      token = 74; /* fall-through */
    case 74:
      if(!doneRotatingTo(90)) break;
      DSManager_removePendingState(72);
#endif
      token = 255;
      break;
    case 75: /* Module M1 */
#ifdef PENDING_STATE
      DSManager_addPendingState(75);
#endif
      rotateDirTo(180,TRUE);
      token = 76;
      break;
    case 76:
      DSManager_sendState(78,2);
#ifdef PENDING_STATE
      token = 77; /* fall-through */
    case 77:
      if(!doneRotatingTo(180)) break;
      DSManager_removePendingState(75);
#endif
      token = 255;
      break;
    case 78: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(78);
#endif
      rotateDirTo(0,FALSE);
      token = 79;
      break;
    case 79:
      DSManager_sendState(81,5);
#ifdef PENDING_STATE
      token = 80; /* fall-through */
    case 80:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(78);
#endif
      token = 255;
      break;
    case 81: /* Module M5 */
      rotateDirTo(0,FALSE);
      token = 82;
      break;
    case 82:
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
      DSManager_sendState(83,1);
      token = 255;
      break;
    case 83: /* Module M1 */
#ifdef PENDING_STATE
      DSManager_addPendingState(83);
#endif
      connect(0);
      token = 84;
      break;
    case 84:
      DSManager_sendState(86,4);
#ifdef PENDING_STATE
      token = 85; /* fall-through */
    case 85:
      if(notDoneConnecting(0)) break;
      DSManager_removePendingState(83);
#endif
      token = 255;
      break;
    case 86: /* Module M4 */
      connect(6);
      token = 87;
      break;
    case 87:
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
      DSManager_sendState(88,2);
      token = 255;
      break;
    case 88: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(88);
#endif
      disconnect(4);
      token = 89;
      break;
    case 89:
      DSManager_sendState(91,1);
#ifdef PENDING_STATE
      token = 90; /* fall-through */
    case 90:
      if(notDoneDisconnecting(4)) break;
      DSManager_removePendingState(88);
#endif
      token = 255;
      break;
    case 91: /* Module M1 */
      disconnect(2);
      token = 92;
      break;
    case 92:
#ifdef PENDING_STATE
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = TRUE;
# endif
      if(DSManager_pendingStatesPresent()) break;
# ifdef PENDING_STATE_TRACK_WAITING
      waitingForPendingState = FALSE;
# endif
#endif
      if(notDoneDisconnecting(2)) break;
      DSManager_sendState(93,3);
      token = 255;
      break;
    case 93: /* Module M3 */
#ifdef PENDING_STATE
      DSManager_addPendingState(93);
#endif
      rotateDirTo(90,FALSE);
      token = 94;
      break;
    case 94:
      DSManager_sendState(96,4);
#ifdef PENDING_STATE
      token = 95; /* fall-through */
    case 95:
      if(!doneRotatingTo(90)) break;
      DSManager_removePendingState(93);
#endif
      token = 255;
      break;
    case 96: /* Module M4 */
#ifdef PENDING_STATE
      DSManager_addPendingState(96);
#endif
      rotateDirTo(0,FALSE);
      token = 97;
      break;
    case 97:
      DSManager_sendState(99,0);
#ifdef PENDING_STATE
      token = 98; /* fall-through */
    case 98:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(96);
#endif
      token = 255;
      break;
    case 99: /* Module M0 */
#ifdef PENDING_STATE
      DSManager_addPendingState(99);
#endif
      rotateDirTo(90,TRUE);
      token = 100;
      break;
    case 100:
      DSManager_sendState(102,1);
#ifdef PENDING_STATE
      token = 101; /* fall-through */
    case 101:
      if(!doneRotatingTo(90)) break;
      DSManager_removePendingState(99);
#endif
      token = 255;
      break;
    case 102: /* Module M1 */
#ifdef PENDING_STATE
      DSManager_addPendingState(102);
#endif
      rotateDirTo(0,FALSE);
      token = 103;
      break;
    case 103:
      DSManager_sendState(105,6);
#ifdef PENDING_STATE
      token = 104; /* fall-through */
    case 104:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(102);
#endif
      token = 255;
      break;
    case 105: /* Module M6 */
      rotateDirTo(180,FALSE);
      token = 106;
      break;
    case 106:
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
      DSManager_sendState(107,3);
      token = 255;
      break;
    case 107: /* Module M3 */
      connect(6);
      token = 108;
      break;
    case 108:
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
      DSManager_sendState(109,1);
      token = 255;
      break;
    case 109: /* Module M1 */
      disconnect(0);
      token = 110;
      break;
    case 110:
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
      DSManager_sendState(111,3);
      token = 255;
      break;
    case 111: /* Module M3 */
      rotateDirTo(0,FALSE);
      token = 112;
      break;
    case 112:
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
      token = 113; /* fall-through */
    case 113: /* Module M3 */
#ifdef PENDING_STATE
      DSManager_addPendingState(113);
#endif
      rotateDirTo(0,FALSE);
      token = 114;
      break;
    case 114:
      DSManager_sendState(116,0);
#ifdef PENDING_STATE
      token = 115; /* fall-through */
    case 115:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(113);
#endif
      token = 255;
      break;
    case 116: /* Module M0 */
#ifdef PENDING_STATE
      DSManager_addPendingState(116);
#endif
      rotateDirTo(0,FALSE);
      token = 117;
      break;
    case 117:
      DSManager_sendState(119,6);
#ifdef PENDING_STATE
      token = 118; /* fall-through */
    case 118:
      if(!doneRotatingTo(0)) break;
      DSManager_removePendingState(116);
#endif
      token = 255;
      break;
    case 119: /* Module M6 */
      rotateDirTo(0,FALSE);
      token = 120;
      break;
    case 120:
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

#ifdef LOOP
      reset();
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

void robodays_sculpture_reset() {
    if ( myID == 6 ) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }    
    DSManager_reset();
}

void robodays_sculpture_init(int self_id) {
    uint32_t address;
    if((self_id==0)) address = 0;
    else if((self_id==1)) address = 1;
    else if((self_id==2)) address = 2;
    else if((self_id==3)) address = 3;
    else if((self_id==4)) address = 4;
    else if((self_id==5)) address = 5;
    else if((self_id==6)) address = 6;

    else address = 127;
	myID = address;
	DSManager_init(myID);
	
    atronApi_setLeds(myID);
	robodays_sculpture_reset();
	isDone = false;
}

	
int robodays_sculpture_act() {
	stateMachine(); 
	return 0;
}

bool robodays_sculpture_isDone() {
	return isDone;
}
