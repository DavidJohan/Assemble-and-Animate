/**
\file LearningManager.h

LearningManager.h manages learning processes.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/


#ifndef LEARNINGMANAGER_H_
#include <ase/control/strategies/Learning/Learning.h>
typedef struct {
	float deltaTime;
	float nextUpdateTime;
} LearningUpdate_t;

void LearningManager_init();
int LearningManager_act();
Learning_t* LearningManager_createLearningProcess(short label, float deltaTime);
#define LEARNINGMANAGER_H_

#endif /* LEARNINGMANAGER_H_ */
