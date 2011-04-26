#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ase/KheperaApi.h>

void controller_init() {
	printf("ASE Controller started\n");
	uint16_t sw_version[2];
	kheperaApi_readSoftwareVersion((uint16_t*)&sw_version);
	printf("Software version: %i %i \n ",sw_version[0],sw_version[1]);
}
void controller_act()  {
	uint16_t prox[8], i;
	kheperaApi_readProximitySensors(prox);
	printf("proximity: %d,%d,%d,%d,%d,%d,%d,%d \n",prox[0],prox[1],prox[2],prox[3],prox[4],prox[5],prox[6],prox[7]);
	uint16_t left_prox_sum = prox[0]+prox[1]+prox[2];
	uint16_t right_prox_sum = prox[3]+prox[4]+prox[5];
	int16_t left_speed = 10-right_prox_sum/10;//rand()%100-50;
	int16_t right_speed = 10-left_prox_sum/10;//rand()%100-50;

	kheperaApi_setSpeed(left_speed, right_speed);
	printf("Speed set to %i and %i\n",left_speed,right_speed);
}
