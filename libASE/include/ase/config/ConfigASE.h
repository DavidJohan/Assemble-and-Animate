/**
 * \file ConfigASE.h
 * ConfigASE.h contains defines that configures how ASE is compiled.
 * All memory allocation is at compile time, therefore you must here specify how
 * many instances of different classes must be allocated. E.g. how many Gradients are
 * will the application use?
 *
 * \author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
 * \note This software is distributed under the BSD open-source license.
 */

/**
 * Static memory allocation in ASE
 */
#define ASE_MEMORY_ALLOCATED		2000	//bytes of ram statically allocated but dynamically assigned at runtime in the initialization phase

#define PRINTF						//if defined target is assumed to have "printf" avaliable, comment out to reduce ram usage

#ifdef USSR
#define FPRINTF						//"fprintf" avaliable (load )

#elif WEBOTS
#define FPRINTF

#elif DYNAMIXEL
//#define FPRINTF
#endif

/**
 * Module Definitions
 */
#ifdef ATRON
#define MAX_NODE_DEGREE			8	//Max number of communication channels per module
#define MAX_ACTUATORS			1   //atron=1, mtran=2, roombot=3
#endif

#ifdef MTRAN
#define MAX_NODE_DEGREE			6	//Max number of communication channels per module
#define MAX_ACTUATORS			2   //atron=1, mtran=2, roombot=3
#endif

#ifdef ROOMBOT
#define MAX_NODE_DEGREE			10	//Max number of communication channels per module
#define MAX_ACTUATORS			3   //atron=1, mtran=2, roombot=3
#endif

#ifdef ODIN
#define MAX_NODE_DEGREE			2	//Max number of communication channels per module
#define MAX_ACTUATORS			1   //atron=1, mtran=2, roombot=3
#endif

#ifdef KHEPERA
#define MAX_NODE_DEGREE			0	//Max number of communication channels per module
#define MAX_ACTUATORS			2   //atron=1, mtran=2, roombot=3
#endif

#ifdef DYNAMIXEL
#define MAX_NODE_DEGREE			0	//Max number of communication channels per module
#define MAX_ACTUATORS			10  //atron=1, mtran=2, roombot=3
#endif

#ifdef LPF
#define MAX_NODE_DEGREE			4	//Max number of communication channels per module
#define MAX_ACTUATORS			4  //atron=1, mtran=2, roombot=3
#define MAX_DEVICES				25  //is this a local or a global value?
#endif

#ifdef LMF
#define MAX_NODE_DEGREE			0	//Max number of communication channels per module
#define MAX_ACTUATORS			0  //atron=1, mtran=2, roombot=3
#endif

#ifdef DUMMY
#define MAX_NODE_DEGREE			0	//Max number of communication channels per module
#define MAX_ACTUATORS			0  //atron=1, mtran=2, roombot=3
#endif


/**
 * Static memory allocation which cannot be assigned in a semi-dynamic way
 * but can be configured and other definitions
 */
#define BROADCASTER_MSG_CACHE_SIZE	3	//Typical=3
#define BROADCASTER_MAX_MSG_SIZE	20	//Typical=20

#define DSM_MAX_PENDING_STATES		6	//Typical=6 max no. of states which could all be pending at a point
#define PENDING_STATE					//Defined to enable pending states


/**
 * Misc which will be replaced with semi-dynamic memory allocation
 */
#define GRADIENT_MAX			1	//Typical=5

#define MC_MAX_PAYLOAD			25 	//Typical=25 max bytes in payload of a modular commander message
#define MC_MAX_HANDLERS			25 	//Typical=25 max number of handlers of modular commander messages


#define	LEARNING_MAX_ACTIONS	10	//Typical=10
#define LEARNING_PROCESSES_MAX	10	//Typical=10
#define SPSA_N_PARAMETERS_MAX	500

#define GOSSIP_MAX				3	//numbers of gossip algorithms
#define GOSSIP_MAX_MSG_SIZE		5  	//max number of bytes communicated through gossiping


