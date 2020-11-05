/*
 * PIT.h
 *
 *  Created on: Oct 29, 2020
 *      Author: jprr2
 */

#ifndef PIT_H_
#define PIT_H_

void PIT_init();
void PIT_IRQHandler();
void pulso_sensor_us();
void proximity_sensor();

#endif /* PIT_H_ */
