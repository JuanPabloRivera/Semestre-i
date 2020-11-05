/*
 * UART0.h
 *
 *  Created on: Oct 29, 2020
 *      Author: jprr2
 */

#ifndef UART0_H_
#define UART0_H_

void UART0_init();
void UART0_write(char string[]);
void UART0_writeInt(int data);
void UART0_writeChar(unsigned char c);

#endif /* UART0_H_ */
