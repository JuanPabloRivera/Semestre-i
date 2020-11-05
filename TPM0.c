/*
 * TPM0.c
 *
 *  Created on: Oct 30, 2020
 *      Author: jprr2
 */

#include "derivative.h"  //include peripheral declarations

unsigned long fs1A_count = 0;
unsigned long fs1B_count = 0;
unsigned long fs2A_count = 0;
unsigned long fs2B_count = 0;

void TPM0_init(){	
	SIM_SOPT2 |= (3<<24);                           //TPM clock MCGIRCLK 4MHz
	SIM_SCGC6|=(1<<24);				//TPM0
	
	TPM0_SC=(1<<3);					//reloj interno (4 MHz)
	TPM0_C0SC=(3<<2)+(1<<6); 		//input capture rising/falling edge, hab intr channel
	TPM0_C1SC=(3<<2)+(1<<6); 		//input capture rising/falling edge, hab intr channel
	TPM0_C2SC=(3<<2)+(1<<6); 		//input capture rising/falling edge, hab intr channel
	TPM0_C3SC=(3<<2)+(1<<6); 		//input capture rising/falling edge, hab intr channel
	NVIC_ISER|=(1<<17);
} 

void FTM0_IRQHandler(){
	if (((TPM0_C0SC & (1<<7)) >> 7) == 1){
		TPM0_C0SC|=(1<<7);		//apagar bandera
		fs1A_count++;
	}
	
	if (((TPM0_C1SC & (1<<7)) >> 7) == 1){
		TPM0_C1SC|=(1<<7);		//apagar bandera
		fs1B_count++;
	}
	
	if (((TPM0_C2SC & (1<<7)) >> 7) == 1){
		TPM0_C2SC|=(1<<7);		//apagar bandera
		fs2A_count++;
	}
	
	if (((TPM0_C3SC & (1<<7)) >> 7) == 1){
		TPM0_C3SC|=(1<<7);		//apagar bandera
		fs2B_count++;
	}
		
}
