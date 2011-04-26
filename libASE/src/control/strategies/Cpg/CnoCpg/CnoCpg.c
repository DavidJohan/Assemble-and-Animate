#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <ase/control/strategies/Cpg/CnoCpg/CnoCpg.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/communication/CommDefs.h>
#include <math.h>
#include <stdio.h>

static void reset(void* data) {
	CnoCpg_resetState((CnoCpg_t*) data);
}

void CnoCpg_initialize(CnoCpg_t* cpg) {
	cpg->E = 1;
	cpg->tau = cpg->super.updatesPerperiode/(6.283f);
	cpg->alpha = 1;
	CnoCpg_resetState(cpg);
	Strategy_setRestartHandler((Strategy_t*) cpg,reset);
}

void CnoCpg_setUpdateRate(float updatesPerPeriode, CnoCpg_t* cpg) {
	cpg->tau = updatesPerPeriode/6.283f;
	cpg->super.updatesPerperiode = updatesPerPeriode;
}

void CnoCpg_resetState(CnoCpg_t *cpg) {
	CpgSuper_resetState((CpgSuper_t*) cpg);
	cpg->S = 0.0f;
}

void CnoCpg_iterate(CnoCpg_t *cpg) {
	if(Strategy_isPaused((Strategy_t*)cpg) || !Strategy_isStarted((Strategy_t*)cpg)) return;
	if(cpg->super.nCouplings!=0) cpg->S=cpg->S/cpg->super.nCouplings; //is this a hack?
	float vNext = cpg->super.y+(-cpg->alpha*cpg->super.y*((cpg->super.x*cpg->super.x+cpg->super.y*cpg->super.y-cpg->E)/cpg->E)-cpg->super.x+cpg->S)/cpg->tau;
	float xNext = cpg->super.x+cpg->super.y/cpg->tau;
	cpg->super.y = vNext;
	cpg->super.x = xNext;
	if(fabs((int)cpg->super.y)>1000||fabs((int)cpg->super.x)>5*cpg->E)  {
		#ifdef PRINTF
		ase_printf("ERROR: CPG UNSTABLE! x=%f v=%f (module %i) \n", (double)cpg->super.x, (double)cpg->super.y, getHardwareID());
		#endif
		CnoCpg_resetState(cpg);
	}
	cpg->S=0;
	cpg->super.nCouplings=0;
}

// \todo no need to do a heavy cos and sin calculation here...
void CnoCpg_addCoupling(float x1, float v1, CpgSuperCoupling_t *coupling, CnoCpg_t *cpg) {
	if(Strategy_isPaused((Strategy_t*)cpg) || !Strategy_isStarted((Strategy_t*)cpg)) return;
	if(sqrt(x1*x1+v1*v1)==0.0f) return;
	cpg->S += coupling->strength*((sin(coupling->phaseDiff)*x1+cos(coupling->phaseDiff)*v1)/(sqrt(x1*x1+v1*v1))-cpg->super.y/(sqrt(cpg->super.x*cpg->super.x+cpg->super.y*cpg->super.y)));
	cpg->super.nCouplings++;
}

void CnoCpg_init() {

}
