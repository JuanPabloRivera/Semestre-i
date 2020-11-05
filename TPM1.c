/*
 * TPM1.c
 *
 *  Created on: Oct 30, 2020
 *      Author: jprr2
 */

#include "derivative.h"  //include peripheral declarations

unsigned long periodo = 50000;
unsigned short duty_cycle1 = 0;
unsigned short duty_cycle2 = 0;

void TPM1_init (void){	
	SIM_SOPT2 |= (3<<24);                           //TPM clock MCGIRCLK 4MHz
	SIM_SCGC6|=(1<<25);		                    //TPM1
	
	TPM1_SC=(1<<3)+3;			                //preescaler 8:1, reloj interno (4 MHz)
	TPM1_MOD=periodo;	
	
	//CHANNEL 0
	TPM1_C0SC=(9<<2); 			                //EPWM set on match
	TPM1_C0V=((periodo*(100-duty_cycle1))/100);	//tiempo pin en cero
	//CHANNEL 1
	TPM1_C1SC=(9<<2); 			                //EPWM set on match
	TPM1_C1V=((periodo*(100-duty_cycle2))/100);	//tiempo pin en cero
}

void TPM1_update(){
	TPM1_C0V=((periodo*(100-duty_cycle1))/100);	//tiempo pin en cero
	TPM1_C1V=((periodo*(100-duty_cycle2))/100);	//tiempo pin en cero
}
