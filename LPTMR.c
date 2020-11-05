/*
 * LPTMR.c

 *
 *  Created on: Oct 29, 2020
 *      Author: jprr2
 */

#include "derivative.h" /* include peripheral declarations */
#include "System.h"
#include "WiFi.h"
#include "TPM2.h"
#include "TPM0.h"

unsigned long rpm1_promedio = 0;
unsigned long rpm2_promedio = 0;

extern unsigned long fs1A_count;
extern unsigned long fs1B_count;
extern unsigned long fs2A_count;
extern unsigned long fs2B_count;

void LPTMR0_init(void){
	SIM_SCGC5|=1;
	
	LPTMR0_CMR=20-1;		// Lectura de flancos de subida cada 20 ms
	LPTMR0_PSR=5;			//LPO, bypass preescaler
	//LPTMR0_CSR=(1<<6)+1;	//Hab intr y Enable timer
	
	//NVIC_ISER|=(1<<28);		//hab intr LPT desde NVIC
}

void LPTMR1_init(){
	SIM_SCGC5 |= 1;
}

void LPTimer_IRQHandler(){
	unsigned long rpm1 = 0;
	unsigned long rpm2 = 0;
	unsigned long rpm1A = 0;
	unsigned long rpm1B = 0;
	unsigned long rpm2A = 0;
	unsigned long rpm2B = 0;
	
	LPTMR0_CSR|=(1<<7);		//apaga bandera y mantiene configuracion
	
	rpm1A = (60*fs1A_count*1000*10)/(11*468*20*2);       //formulas rpm
	rpm1B = (60*fs1B_count*1000*10)/(11*468*20*2);
	rpm2A = (60*fs2A_count*1000*10)/(11*468*20*2);       //formulas rpm
	rpm2B = (60*fs2B_count*1000*10)/(11*468*20*2);
	
	rpm1 = (rpm1A+rpm1B)/2;     //promedio ambos canales
	rpm2 = (rpm2A+rpm2B)/2;     //promedio ambos canales
	
	rpm1_promedio = (rpm1_promedio+rpm1)/2;      //promedio de resultados
	rpm2_promedio = (rpm2_promedio+rpm2)/2;      //promedio de resultados
	
	fs1A_count = 0;
	fs1B_count = 0;
	fs2A_count = 0;
	fs2B_count = 0;
}
