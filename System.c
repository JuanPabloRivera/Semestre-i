/*
 * System.c
 *
 *  Created on: Oct 29, 2020
 *      Author: jprr2
 */

#include "derivative.h" /* include peripheral declarations */
#include "LPTMR.h"
#include "PIT.h"
#include "WiFi.h"
#include "UART0.h"
#include "UART1.h"
#include "TPM2.h"
#include "TPM1.h"
#include "TPM0.h"

//MCGIRCLK = 4MHz ; MCGOUTCLK = MCGFLLCLK = 48MHz ; Bus Clock = 24MHz
//UART0:MCGFLLCLK ; PIT=UART1:Bus Clock ; TPM0=TPM1=TPM2: MCGIRCLK
void clk_init (void){
	MCG_C1 |=  0<<6;						// CLKS = 0: MCGOUTCLK Source is FLL (default) 
	MCG_C1 |= (1<<2) + (1<<1);				// IREFS = 1: Slow internal reference clock (default) for FLL & MCGIRCLK active
	MCG_C2 |= 1;					        //Mux IRCLK : FIRC (4 MHz) pag 116
	MCG_SC = 0;					            //Preescaler FIRC 1:1 pag 116
	MCG_C4 |= (1<<5) + (1<<7);				//DRTS_DRS = 1: Mid range & DCO range 48MHz
}

void PORT_init(){
	SIM_SCGC5 |= (1<<9)+(1<<10)+(1<<11)+(1<<13);    //clk PORTA & PORTB & PORTC & PORTE
	
	//LED PROXIMIDAD (ROJO)
	PORTE_PCR3 = (1<<8);                            //PORTE 3 as GPIO
	GPIOE_PDDR |= (1<<3);                           //E3 as output
	GPIOE_PCOR |= (1<<3);                           //Turns off led
	
	//LED AZUL
	PORTE_PCR4 = (1<<8);                           //PORTE 4 as GPIO
	GPIOE_PDDR |= (1<<4);                          //E4 as output
	GPIOE_PCOR |= (1<<4);                          //Turns off led
	
	//LED VERDE
	PORTB_PCR19 = (1<<8);                          //PORTB 19 as GPIO
	GPIOB_PDDR |= (1<<19);                         //B19 as output
	GPIOB_PSOR |= (1<<19);                         //Turns off led
	
	//Trig pin
	PORTE_PCR2 = (1<<8);                           //PORTE 2 as GPIO
	GPIOE_PDDR |= (1<<2);                          //E2 as output
	GPIOE_PSOR |= (1<<2);                          //Turns off pin
	
	//Echo pin
	PORTE_PCR22 = (3<<8);                          //TPM2_CH0
	
	//UART0 TX & RX
	PORTA_PCR1 = (2<<8);                           //PORTA 1 as RX
	PORTA_PCR2 = (2<<8);                           //PORTA 2 as TX
	
	//UART1 TX & RX
	PORTE_PCR0 = (3<<8);                           //PORTE 0 as TX
	PORTE_PCR1 = (3<<8);                           //PORTE 1 as RX
	
	//TPM1 CH0 & CH1 (Encoders)
	PORTC_PCR1 = (4<<8);						   //TPM0_C0
	PORTC_PCR2 = (4<<8);     					   //TPM0_C1
	PORTC_PCR3 = (4<<8);						   //TPM0_C2
	PORTC_PCR4 = (4<<8);                           //TPM0_C3
	
	//TPM1_CH1 (PWM)
	PORTB_PCR0 = (3<<8);                           //TPM1_C0
	PORTB_PCR1 = (3<<8);                           //TPM1_C1
}

char *strcat(char *dest, const char *src){
    int i,j;
    for (i = 0; dest[i] != '\0'; i++);
    for (j = 0; src[j] != '\0'; j++)
        dest[i+j] = src[j];
    dest[i+j] = '\0';
    return dest;
}

void Sys_init(){
	clk_init();
	PORT_init();
	LPTMR0_init();
	PIT_init();
	UART0_init();
	UART1_init();
	WiFi_setup();
	TPM1_init();
	TPM0_init();
	TPM2_init();
	pulso_sensor_us();
	proximity_sensor();
}
