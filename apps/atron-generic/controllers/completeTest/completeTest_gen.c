/* DynaRole C code generated for completeTest */
/* DynaRole C statemachine generated for completeTest */
#include <stdbool.h> 
#include <stdio.h> 
#include <stdint.h>
#include <math.h>
#include <ase/completeTest.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>

#define CENTER_ERROR_TOLERANCE	5
#define COMPLETETEST_DONE_EVENT "COMPLETETEST_DONE_EVENT"
#define TRUE	1
#define FALSE	0

static uint8_t token = 255;
static int myID=-1;
static bool isDone = false;
static bool isPaused = true;  

void completeTest_reset_sequence();
void completeTest_reset_state();

static bool doneRotatingTo(int goal) { 
	int error = (atronApi_getAngularPositionDegrees()-goal);
	while(error>360) error -=360; 
	while(error<-360) error +=360;  
	if(error>180) error = 360-error;
	if(error<-180) error = -(380+error);
	
	if(atronApi_isRotating()) {
		#ifdef PRINTF
//		ase_printf("%i: Is rotating (error = %f)\n",myID, fabs(error));
		#endif
		return false; 
	}
	
	if(fabs(error)>CENTER_ERROR_TOLERANCE) {
		#ifdef PRINTF
	//	ase_printf("%i: Rotation error too large (error = %f)\n",myID, fabs(error));
		#endif		
		return false;
	}
	#ifdef PRINTF
	ase_printf("%i: Rotation Done :-) (error = %f)\n",myID, fabs(error));
	#endif	
	return true;
}

static void rotateDirTo(int to, int direction) {
	#ifdef PRINTF
	ase_printf("%i: Rotating from %i to %i direction %i...\n", myID, atronApi_getAngularPositionDegrees(), to, direction);
	#endif
	//atronApi_rotateToDegreeInDegrees(to);
	atronApi_rotateDirToInDegrees(to, direction);
}


static void connect(int connector) {
	#ifdef PRINTF
	ase_printf("%i: Connecting to connector %i...\n",myID,connector);
	#endif
		
	atronApi_connect(connector);
}

static void disconnect(int connector) {
	#ifdef PRINTF
	ase_printf("%i: Disconnecting connector %i...\n",myID,connector);
	#endif
	atronApi_disconnect(connector);
}

static int notDoneConnecting(int connector) {
	if(!atronApi_isOtherConnectorNearby(connector)) {
		#ifdef PRINTF
		ase_printf("  %i: no connector nearby!\n",myID);
		#endif
	}
	return !atronApi_isConnected(connector);
} 
 
static int notDoneDisconnecting(int connector) {
	#ifdef PRINTF
	ase_printf("%i: Disconnecting from connector %i...\n",myID,connector);
	#endif	
	if(!atronApi_isOtherConnectorNearby(connector)) {
		#ifdef PRINTF
		ase_printf("  %i: no connector nearby!\n",myID);
		#endif	
	}
	return atronApi_isConnected(connector);
}

static void stateMachine() { 
    if(token == 255) { /* try to see if there's a new state for me */
      token = DSManager_getMyNewState();
	  if(token!=255) {
	  	#ifdef PRINTF
		ase_printf("%i: Now performing state %i\n", myID, token);
		#endif	   
	  }
    }
	
	switch(token) {
	case 0: token = 1; /* fall-through */
    case 1: /* Module M0 */
      rotateDirTo(90,TRUE);
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
      if(!doneRotatingTo(90)) break;
      token = 3; /* fall-through */
    case 3: /* Module M0 */
      rotateDirTo(180,TRUE);
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
      if(!doneRotatingTo(180)) break;
      token = 5; /* fall-through */
    case 5: /* Module M0 */
      rotateDirTo(270,TRUE);
      token = 6;
      break;
    case 6:
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
      token = 7; /* fall-through */
    case 7: /* Module M0 */
      rotateDirTo(0,TRUE);
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
      if(!doneRotatingTo(0)) break;
      token = 9; /* fall-through */
    case 9: /* Module M0 */
      /* { */
        connect(0);
        connect(2);
        connect(4);
        connect(6);
      /* } */
      token = 10;
      break;
    case 10:
        // local blocking handlers:
        if(notDoneConnecting(0)) break;
        if(notDoneConnecting(2)) break;
        if(notDoneConnecting(4)) break;
        if(notDoneConnecting(6)) break;
      token = 11; /* fall-through */
    case 11: /* Module M0 */
      /* { */
        disconnect(0);
        disconnect(2);
        disconnect(4);
        disconnect(6);
      /* } */
      token = 12;
      break;
    case 12:
        // local blocking handlers:
        if(notDoneDisconnecting(0)) break;
        if(notDoneDisconnecting(2)) break;
        if(notDoneDisconnecting(4)) break;
        if(notDoneDisconnecting(6)) break;

#ifdef LOOP
      token = 255;
      completeTest_reset_sequence();
      break;
#endif//LOOP
	
	DSManager_sendState(254,-1);
	token=255;
	break;
	
	case 255:
		if(DSManager_getGlobalState()==254) {
			if(!isDone) {
			  	#ifdef PRINTF
				ase_printf("%i: i am done...\n",myID);
				#endif
				Event_t event;
				EventManager_publish("COMPLETETEST_DONE_EVENT",&event);
					   
			}
			isDone = true;
		}
		break;
	}
}

void completeTest_reset_sequence() {
    if ( myID == 0 ) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }    
    DSManager_reset_sequence();
}

void completeTest_reset_state() {
    if ( myID == 0 ) {
      token = 0; // Tolerate reset in the middle of reconfiguration
    }    
    DSManager_reset_state();
}

	
void completeTest_setPaused(bool paused) {
	isPaused = paused;
}

bool completeTest_isPaused() {
	return isPaused;
}

bool completeTest_isDone() {
	return isDone;
}

void completeTest_act(char* topic, Event_t* event) {
	if(!isPaused) {
		stateMachine();
	} 
}

void completeTest_init(int self_id) {
    uint32_t address;
    if((TRUE==TRUE)) address = 0;

    else address = 127;
	myID = address;
	DSManager_init(myID);
	
    atronApi_setLeds(myID);
	completeTest_reset_state();
	isDone = false;
	EventManager_subscribe(ACT_EVENT,completeTest_act);
	EventManager_registerTopic("COMPLETETEST_DONE_EVENT");
}
