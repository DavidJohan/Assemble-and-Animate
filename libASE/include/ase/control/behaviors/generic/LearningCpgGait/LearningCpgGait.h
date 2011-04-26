/**
 * \file LearningCpgGait.h
 * The generic/shared parts of CpgGaits optimized using some learning algorithm
 *
 *
 * Created on: Jan 7, 2010
 *
 * \see SPSACpgGait.h
 * \see RandomSearchCpgGait.h
 * \see CpgGait.h
 * \see HopfCartesianCpg.h
 * \author David Johan Christensen
 */

#ifndef LEARNINGCPGGAIT_H_

#define LEARNING_CPG_GAIT_UPDATED_EVENT	"LEARNING_CPG_GAIT_UPDATED_EVENT"

typedef struct {
	float upperlimit;
	float lowerlimit;
	float home;
} LearningGpgGaitInterval_t;


void LearningGpgGait_init();

/**
 * Set the interval (upper and lower limit) and home (initial value) of the frequency (for all
 * cpg's in module)
 */
void LearningCpgGait_setFrequencyInterval(float lowerlimit, float home, float upperlimit);

/**
 * Set the interval (upper and lower limit) and home (initial value) of the amplitude for a specific
 * actuator given with a index
 */
void LearningCpgGait_setAmplitudeInterval(float lowerlimit, float home, float upperlimit, int index);

/**
 * Set the interval (upper and lower limit) and home (initial value) of the phase difference to the
 * global clock for a specific actuator given with a index
 */
void LearningCpgGait_setPhaseshiftInterval(float lowerlimit, float home, float upperlimit, int index);

/**
 * Set the interval (upper and lower limit) and home (initial value) of the offset for a specific actuator given with a index
 */
void LearningCpgGait_setOffsetInterval(float lowerlimit, float home, float upperlimit, int index);

/**
 * Set the interval (upper and lower limit) and home (initial value) of the feedback term with number nr
 * used in CpgGait.h for a specific actuator given with a index
 */
void LearningCpgGait_setFeedbackTermInterval(float lowerlimit, float home, float upperlimit, int nr, int index);

/**
 * Log controller (parameters optimized using learning) to given file (must be opened)
 */
void LearningCpgGait_logController(int id, FILE* log, float* parameters);

void LearningCpgGait_getScaledController(float* controller);
void LearningCpgGait_updateGait(float* parameters);
void LearningCpgGait_signalUpdated();

#define LEARNINGCPGGAIT_H_

#endif /* LEARNINGCPGGAIT_H_ */
