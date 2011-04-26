/*
 * SnakeGait.h
 *
 *  Created on: March 11, 2009
 *  Author: David Johan Christensen
 */


#ifndef SNAKEGAIT_H_
#include <stdbool.h>
#include <ase/infrastructure/Entity/Strategy/Strategy.h>
#include <ase/control/strategies/Cpg/CnoCpg/CnoCpg.h>

/**
 * CPG based control strategy which...
 * \extends Strategy_t
 */
typedef struct {
	Strategy_t strategy;
	CnoCpg_t* cpg;
	CpgSuperCoupling_t *h2tCoupling;
	CpgSuperCoupling_t *t2hCoupling;
} SnakeGait_t;



void SnakeGait_init(short label);
void SnakeGait_act();
void SnakeGait_setAmplitude(float amplitude);
void SnakeGait_setOffset(float offset);
void SnakeGait_setPhaseShift(float phaseShift);
void SnakeGait_setFrequency(float hz);

#define SNAKEGAIT_H_

#endif /* SNAKEGAIT_H_ */
