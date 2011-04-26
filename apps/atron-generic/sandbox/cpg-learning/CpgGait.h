/*
 * CpgGait.h
 *
 *  Created on: May 4, 2009
 *  Author: David Johan Christensen
 */


#ifndef CPGGAIT_H_
#include <stdbool.h>
void CpgGait_init();
void CpgGait_act();
void CpgGait_setAmplitude(float amplitude);
void CpgGait_setOffset(float offset);
void CpgGait_setPhaseShift(float phaseShift);
void CpgGait_setFrequency(float hz);
#define CPGGAIT_H_

#endif /* CPGGAIT_H_ */
