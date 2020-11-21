/*
 * ADC0.c
 *
 *  Created on: Nov 19, 2020
 *      Author: jprr2
 */


#include "derivative.h"  //include peripheral declarations
#include "UART0.h"
#include "Math.h"

unsigned short samples;
unsigned int temperature = 0;
unsigned char temp_obtained = 1;

void ADC0_init(){
	SIM_SCGC6 |= (1<<27);                   //clk ADC0
	
	ADC0_SC1A |= 31;                        //Module disabled
	ADC0_SC1A &= ~(1<<6);                    //Intr disabled
	ADC0_CFG1 |= (1<<2);                    //single-ended 12-bit conversion & bus clock selected
	
	NVIC_ISER |= (1<<15);                   //Intr enable NVIC ADC0
}

void ADC0_IRQHandler(){
	if (samples > 0){
		unsigned short adc_result_raw;
		unsigned short adc_result_mV;
		
		adc_result_raw = ADC0_RA;                           //reading converted value
		adc_result_mV = (adc_result_raw * 3300) / 0xFFF;   //mapping value from 4096 to 3300
		temperature += (sqrt(((adc_result_mV+3000)/0.9375)+pow(93.75/1.875,2))-(93.75/1.875))*100;   //calculating temperature based on voltage
		samples--;
		ADC0_SC1A &= ~(31<<0);                        //Cleaning ADC0 channel
		ADC0_SC1A |= 12;                              //Start conversion. Reading from channel 12;
	}else{
		temperature /= 100;                          //Obtaining the temperature mean
		ADC0_SC1A &= ~(1<<6);                         //Intr disabled
		temp_obtained = 1;
	}
}

void read_temperature(){
	samples = 100;
	temperature = 0;
	temp_obtained = 0;
	ADC0_SC1A &= ~(31<<0);                        //Cleaning ADC0 channel
	ADC0_SC1A |= 12;                              //Start conversion. Reading from channel 12;
	
	ADC0_SC1A |= (1<<6);                          //Intr enabled
}
