/**
 * \file RK4.h
 * RK4.h provides the Runge-Kutta (RTK4) method for numerical solving of differential equations.
 * It is used by HopfCartesianCpg.h
 *
 * \date July 3, 2009
 * \author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
 * \note This software is distributed under the BSD open-source license.
 */


#ifndef RK4_H_
#define RK4_H_

/**
 * Computes an estimation of y(t+h) given y(t) by using the Runge-Kutta (RTK4) method.
 * Needed parameters are: time t, estimation of y(t), a step size h, a function
 * pointer to compute y'(t, y, *params). Use a void pointer to pass other
 * parameters to call-back function.
 */
float RK4_itterate(float t, float y, float h, void* params, float (*function)(float, float, void*));

#endif /* RKT4_H_ */
