
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpg.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/tools/Runge-Kutta/RK4.h>
#include <math.h>
#include <stdio.h>


void HCCpg_init() {

}

static void reset(void* data) {
	HCCpg_resetState((HCCpg_t*) data);
}

void HCCpg_initialize(HCCpg_t *cpg) {
	cpg->my = 1.0f;
	cpg->gamma = 1.0f;
	cpg->omega = 1.0f;
	cpg->omegaEffective = 1.0f;
	cpg->h = 6.28318531f/(cpg->omega*cpg->parent.updatesPerperiode);
	HCCpg_resetState(cpg);
	Strategy_setRestartHandler((Strategy_t*) cpg,reset);
}

void HCCpg_resetState(HCCpg_t *cpg) {
	CpgSuper_resetState((CpgSuper_t*) cpg);
	cpg->S = 0.0f;
	cpg->x_add = 0;
	cpg->y_add = 0;
	cpg->omega_add = 0.0f;
	cpg->parent.nCouplings = 0;
}

/*
 * Computes the function:
 * x'(t,x,y) = gamma*(my - r^2)*x - omega*y
 */
float f1(float t, float x, void* pCpg) {
	HCCpg_t* cpg = (HCCpg_t*) pCpg;
	float y = cpg->parent.y;
	float r = sqrt(pow(x,2) + pow(y,2));
	float omega = cpg->omega+cpg->omega_add;
	return cpg->gamma*(cpg->my - pow(r,2))*x - omega*y + cpg->x_add;
}

/*
 * Computes the function:
 * y'(t,y,x) = gamma*(my - r^2)*y - omega*x
 */
float f2(float t, float y, void* pCpg) {
	HCCpg_t* cpg = (HCCpg_t*) pCpg;
	float x =cpg->parent.x;
	float r = sqrt(pow(x,2) + pow(y,2));
	float omega = cpg->omega+cpg->omega_add;
	return cpg->gamma*(cpg->my - pow(r,2))*y + omega*x + cpg->y_add;
}

void HCCpg_coupleDirectlyX(float signal,HCCpg_t *cpg) {
	cpg->x_add += signal;
}

void HCCpg_coupleDirectlyY(float signal, HCCpg_t *cpg) {
	cpg->y_add += signal;
}

void HCCpg_iterate(HCCpg_t *cpg) {
	if(Strategy_isPaused((Strategy_t*)cpg) || !Strategy_isStarted((Strategy_t*)cpg)) return;
	if(cpg->parent.nCouplings!=0) cpg->omega_add=cpg->omega_add/cpg->parent.nCouplings;
	float x_next = RK4_itterate(0, cpg->parent.x, cpg->h, cpg, f1); //time=0 since f1 and f2 are independent from t
	float y_next = RK4_itterate(0, cpg->parent.y, cpg->h, cpg, f2);
	cpg->parent.x = x_next;
	cpg->parent.y = y_next;

	if(fabs(cpg->parent.x)>5||fabs(cpg->parent.y)>5)  {
		#ifdef PRINTF
		ase_printf("ERROR: CPG UNSTABLE! x=%f y=%f (module %i) \n", cpg->parent.x, cpg->parent.y, getHardwareID());
		#endif
		HCCpg_resetState(cpg);
	}

	float p = cpg->h/6.283f;
	cpg->omegaEffective =(1-p)*cpg->omegaEffective + p*(cpg->omega+cpg->omega_add);
	cpg->x_add = 0;
	cpg->y_add = 0;
	cpg->omega_add=0;
	cpg->parent.nCouplings=0;
}

void HCCpg_addCoupling(float xj, float yj, CpgSuperCoupling_t *coupling, HCCpg_t *cpg) {
	if(Strategy_isPaused((Strategy_t*)cpg) || !Strategy_isStarted((Strategy_t*)cpg)) return;
	float xi = cpg->parent.x;
	float yi = cpg->parent.y;
	float ri = sqrt(pow(xi,2)+pow(yi,2));
	float wij = coupling->strength;
	float pij = coupling->phaseDiff;
	if(fabs(ri)<0.001f) return;
	float cij = wij/ri*((xi*yi-xj*yi)*cos(pij)-(xi*xj+yi*yj)*sin(pij));
	cpg->omega_add += cij;
	cpg->parent.nCouplings++;
}

void HCCpg_setAmplitudeAttraction(HCCpg_t *cpg, float amplAttStrength) {
	cpg->gamma = amplAttStrength;
}


void HCCpg_setUpdateRate(float updatesPerPeriode, HCCpg_t *cpg) {
	cpg->parent.updatesPerperiode = updatesPerPeriode;
	cpg->h = 6.28318531f/(cpg->omega*updatesPerPeriode);
}
