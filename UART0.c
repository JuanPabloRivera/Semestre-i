/*
 * UART0.c
 *
 *  Created on: Oct 29, 2020
 *      Author: jprr2
 */

#include "derivative.h" /* include peripheral declarations */
#include "System.h"

void UART0_init(){
	SIM_SOPT2 |= (1<<26);                       //clk source for UART0 - MCGFLLCLK (48MHz)
	SIM_SCGC4 |= (1<<10);                       //clk UART0
	
	//Configure UART0
	UART0_C4 = 15;
	UART0_BDH = 0;
	UART0_BDL = 26;                            //baud rate = 115200
			
	//Baud rate = CLK/(OSR+1)*SBR
	
	UART0_C1 = 0;                               //8 data bits, no parity, 1 bit stop
	UART0_C2 |= (3<<2);                          //TE = 1, transmitter enabled
											    //RE = 1, receiver enabled
											    //TIE = 0, RIE = 0, 	interrupts disabled
	NVIC_ISER |= (1<<12);                       //Intr NVIC UART0
}

void UART0_write(char string[]){
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\n';
	unsigned char i = 0;
	do{
		while((UART0_S1 & (1<<7)) == 0);
		UART0_D = string[i++];
	}while(string[i] != '\0');
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\r';
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\n';
}

void UART0_writeInt(int data){
	int indx = 0;
	unsigned char string[4] = {0};
	
	//ITOA
	string[3] = (data%10) + '0';
	data /= 10;
	string[2] = (data%10) + '0';
	data /= 10;
	string[1] = (data%10) + '0';
	string[0] = (data/10) + '0';
	
	do{
		while((UART0_S1 & (1<<7)) == 0);         //Wait until the transmission buffer is empty
		UART0_D = string[indx++];
	}while(string[indx] != '\0');                //Waiting for null character
	
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\r';                                //Return
	while((UART0_S1 & (1<<7)) == 0);             
	UART0_D = '\n';                                //New line
}

void UART0_writeChar(unsigned char c){
	while ((UART0_S1 & (1<<7)) == 0);
	UART0_D = c;
}

/*void UART0_IRQHandler(){
	if (UART0_S1 & (1<<5)){
		char temp = UART0_D;
		if (temp != 13){
			dato_read[ir_u0] = temp;
			ir_u0++;
		}else{
			dato_read[ir_u0] = '\0';
			ir_u0 = 0;
			messageComplete = 1;
		}
	}
}*/
