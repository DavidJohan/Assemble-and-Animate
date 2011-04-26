#include <ase/tools/Runge-Kutta/RK4.h>
#include <stdio.h>

float RK4_itterate(float t, float y, float h, void* params, float (*function)(float, float, void*)) {
	float k1 = function(t, y, params);
	float k2 = function(t+0.5*h, y + 0.5*h*k1, params);
	float k3 = function(t+0.5*h, y + 0.5*h*k2, params);
	float k4 = function(t, y + h*k3, params);

	float y_n = y + 0.166666667f*h* (k1 + 2*k2 + 2* k3 + k4);
	return y_n;
}
