/*
 * UART1.c
 *
 *  Created on: Oct 29, 2020
 *      Author: jprr2
 */

#include "derivative.h" /* include peripheral declarations */
#include "System.h"
#include "UART0.h"

char buffer[500] = "";
char to_parse[100] = "";
unsigned char found_char = 0;
unsigned char buff_ind = 0;
unsigned char messageComplete = 0;

void UART1_init(){
	SIM_SCGC4 |= (1<<11);                       //clk UART1
	
	UART1_BDH = 0;                              //baud rate = 115200
	UART1_BDL = 13;                            
	
	//Baud rate = CLK/(OSR+1)*SBR
	
	UART1_C1 = 0;                               //8 data bits, no parity, 1 bit stop
	UART1_C2 |= (3<<2);                         //TE = 1, transmitter enabled
												//RE = 1, receiver enabled
												//TIE = 0, RIE = 0, 	interrupts disabled
	NVIC_ISER |= (1<<13);                       //Intr NVIC UART1
}

void UART1_write(char string[]){
	UART0_write(string);
	unsigned char i = 0;
	do{
		while((UART1_S1 & (1<<7)) == 0);
		UART1_D = string[i++];
	}while(string[i] != '\0');
	while((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\r';
	while((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\n';
}

void UART1_writeChar(unsigned char c){
	while ((UART1_S1 & (1<<7)) == 0);
	UART1_D = c;
	while ((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\r';
	while ((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\n';
}

void UART1_read(char string[]){
	unsigned char i = 0;
	while(string[i] != '\0'){
		to_parse[i] = string[i];
		i++;
	}
	to_parse[i] = '\0';
	messageComplete = 0;
	buff_ind = 0;
	UART1_C2 |= (1<<5);                      //intr enabled
}

void UART1_IRQHandler(){
	if (UART1_S1 & (1<<5)){
		char temp = UART1_D;                     //Erasing flag by reading
		UART0_writeChar(temp);
		buffer[buff_ind] = temp;                 //Saving read message into buffer
		buff_ind++;
		if (temp == to_parse[found_char]){       //Coincidence found
			found_char++;
			if (to_parse[found_char] == '\0'){   //Whole string found
				buffer[buff_ind] = '\0';
				messageComplete = 1;
				UART1_C2 &= ~(1<<5);             //intr disabled
			}
		}
		else{
			found_char = 0;                      //Looking up again
		}
	}
}
