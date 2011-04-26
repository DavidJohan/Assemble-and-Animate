/*
 * SnakeGait.h
 *
 *  Created on: March 11, 2009
 *  Author: David Johan Christensen
 */


#ifndef SNAKEGAIT_H_
#include <stdbool.h>
void SnakeGait_init();
void SnakeGait_act();
void SnakeGait_setAmplitude(float amplitude);
void SnakeGait_setOffset(float offset);
void SnakeGait_setPhaseShift(float phaseShift);
void SnakeGait_setFrequency(float hz);

#define SNAKEGAIT_H_

#endif /* SNAKEGAIT_H_ */
