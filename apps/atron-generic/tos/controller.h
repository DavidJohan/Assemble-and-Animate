/*
 * CONTROLLER.h
 *
 *  Created on: Oct 2, 2008
 *  Author: David Johan Christensen
 */

#ifndef CONTROLLER_H_
void controller_init();
void controller_act();
void handleMessage(char* message, char messageSize, char channel);
#define CONTROLLER_H_

#endif /* CONTROLLER */
