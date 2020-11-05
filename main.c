/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "System.h"
#include "WiFi.h"

int main(void) {
	Sys_init();
	
	while(1){
		WiFi_execute_instructions();
	}
	return 0;
}
