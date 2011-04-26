#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/behaviors/generic/IDManager_MC/IDManager_MC.h>
#include <ase/communication/RemoteControl/ModularCommander/MCManager.h>
#include <ase/tools/IDContainer/IDContainer.h>

#define MC_TYPE_ID_ASSIGNMENT	99

void IDManager_MC_SoftIDAssignment(uint8_t mcType, uint8_t* payload, uint8_t length, uint8_t channel) {
	if(mcType==MC_TYPE_ID_ASSIGNMENT) {
		int i, hardID = getHardwareID(), success=0;
		for(i=0;i<length;i++) {
			if(payload[i]==hardID) {
				IDContainer_setSoftID(i);
				success=1;
				#ifdef PRINTF
				ase_printf("%i: Found myself, new ID = %i \n",hardID, i);
				#endif
			}
		}
		if(success==0) {
			#ifdef PRINTF
			ase_printf("%i: I did not find myself \n",hardID);
			#endif
		}
	}
	else {
		#ifdef PRINTF
			ase_printf("Error: #1 in IDManager_MC!\n");
		#endif
	}
}

void IDManager_MC_init() {
	MCManager_init();
	IDContainer_init();
	MCManager_installHandler(MC_TYPE_ID_ASSIGNMENT, IDManager_MC_SoftIDAssignment);
}
