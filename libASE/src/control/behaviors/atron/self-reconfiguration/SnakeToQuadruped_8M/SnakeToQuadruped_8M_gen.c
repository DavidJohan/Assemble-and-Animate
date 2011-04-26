/* DynaRole C code generated for SnakeToQuadruped_8M */
/* DynaRole C statemachine generated for SnakeToQuadruped_8M */
#include <stdbool.h> 
#include <stdio.h> 
#include <stdint.h>
#include <math.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/control/behaviors/atron/self-reconfiguration/SnakeToQuadruped_8M/SnakeToQuadruped_8M.h>
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
	printf("%i: Rotating from %i to %i...\n", myID, atronApi_getAngularPositionDegrees(), to);
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
    case 1: /* Module M2 */
#ifdef PENDING_STATE
      DSManager_addPendingState(1);
#endif
      rotateDirTo(180,FALSE);
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
      rotateDirTo(180,FALSE);
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
      rotateDirTo(180,TRUE);
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
      rotateDirTo(180,TRUE);
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
        connect(4 );
        connect(6 );
      /* } */
      token = 13;
      break;
    case 13:
        // local blocking handlers:
        if(notDoneConnecting(4 )) break;
        if(notDoneConnecting(6 )) break;
      DSManager_sendState(14,4);
      token = 255;
      break;
    case 14: /* Module M5 */
      connect(4 );
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
      if(notDoneConnecting(4 )) break;
      DSManager_sendState(16,2);
      token = 255;
      break;
    case 16: /* Module M3 */
#ifdef PENDING_STATE
      DSManager_addPendingState(16);
#endif
      disconnect(6 );
      token = 17;
      break;
    case 17:
      DSManager_sendState(19,6);
#ifdef PENDING_STATE
      token = 18; /* fall-through */
    case 18:
      if(notDoneDisconnecting(6 )) break;
      DSManager_removePendingState(16);
#endif
      token = 255;
      break;
    case 19: /* Module M7 */
      disconnect(6 );
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
      if(notDoneDisconnecting(6 )) break;
      DSManager_sendState(21,3);
      token = 255;
      break;
    case 21: /* Module M4 */
#ifdef PENDING_STATE
      DSManager_addPendingState(21);
#endif
      rotateDirTo(270,TRUE);
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
      rotateDirTo(90,TRUE);
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
      rotateDirTo(90,TRUE);
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
      rotateDirTo(270,TRUE);
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
