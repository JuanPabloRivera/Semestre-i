/*
 * PIT.c
 *
 *  Created on: Oct 29, 2020
 *      Author: jprr2
 */

#include "derivative.h" /* include peripheral declarations */
#include "System.h"
#include "TPM2.h"
#include "TPM1.h"

extern int distancia;
extern unsigned short duty_cycle1;
extern unsigned short duty_cycle2;

void PIT_init(){
	SIM_SCGC6 |= (1<<23);                            //clk PIT
	PIT_MCR &= ~(1<<1);                              //MDIS disabled
	PIT_MCR |= (1<<0);                               //FRE dsiabled - timer runs in debug mode
	PIT_TCTRL0 |= (1<<1);                             //PIT0 intr enable
	PIT_TCTRL1 |= (1<<1);                             //PIT1 intr enable
	NVIC_ISER |= (1<<22);                             //intr enable PIT
}

void proximity_sensor(){
	PIT_LDVAL1 = distancia*90000;
	if ((distancia <= 50) && (distancia >= 10))     //Toggles buzzer
		GPIOE_PTOR |= (1<<3);   
	else if (distancia < 10) {
		GPIOE_PSOR |= (1<<3);                       //Turns on buzzer
		duty_cycle1 = 0;
		duty_cycle2 = 0;
		TPM1_update();                              //Stops motors
	}
	else GPIOE_PCOR |= (1<<3);                      //Turns off buzzer
	PIT_TCTRL1 |= (1<<0);                           //Starts timer
}

void pulso_sensor_us(){
	PIT_LDVAL0 = 250;                                //24MHz 240 cuentas de 41ns para 10us
	PIT_TCTRL0 |= (1<<0);                            //Starts timer
	GPIOE_PSOR |= (1<<2);							 //encender pin
}

void PIT_IRQHandler(){
	if (PIT_TFLG0 & (1<<0)){
		PIT_TFLG0 |= (1<<0);                         //borrando la bandera w1c
		PIT_TCTRL0 &= ~(1<<0);                       //Stops timer
		GPIOE_PCOR |= (1<<2);						 //apagar pin
	}
	if (PIT_TFLG1 & (1<<0)){
		PIT_TFLG1 |= (1<<0);                         //borrando la bandera w1c
		PIT_TCTRL1 &= ~(1<<0);                       //Stops timer
		proximity_sensor();
	}
}
