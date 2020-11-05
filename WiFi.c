/*
 * WiFi.c
 *
 *  Created on: Oct 29, 2020
 *      Author: jprr2
 */

#include "derivative.h" /* include peripheral declarations */
#include "System.h"
#include "UART1.h"
#include "TPM2.h"
#include "LPTMR.h"
#include "TPM1.h"
#include "i2c.h"

unsigned char id;
extern unsigned char messageComplete;
extern char buffer[];
extern int distancia;
extern unsigned long rpm1_promedio;
extern unsigned long rpm2_promedio;
extern unsigned short duty_cycle1;
extern unsigned short duty_cycle2;

void WiFi_setup(){
	UART1_write("AT+RST");                                    //Resetear el modulo
	UART1_read("ready");                                      //Esperar a que el modulo este listo
	while(!(messageComplete));
	UART1_write("AT");                                        //Confirmar que haya comunicacion
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CWMODE=1");                               //Configurar como cliente
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CWLAP");                                  //Mostar redes disponibles
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CWJAP=\"INFINITUM8234\",\"Rki4KckWRi\""); //Conectarse a red
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CIFSR");                                  //Mostrar direccion IP
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CIPMUX=1");                               //Configurar para multiples conexiones
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CIPSERVER=1,80");                         //Configurar puerto
	UART1_read("OK");
	while(!(messageComplete));
	
	//FLASH GREEN LED TO KNOW ITS READY TO RECEIVE INSTRUCTIONS
	int i;
	LPTMR0_CMR=200-1;		                                 // Waiting for 200 ms	
	LPTMR0_CSR |= 1;                                       //Starts timer
	for (i = 0; i < 6; i++){
		GPIOB_PTOR |= (1<<19);
		do{}while((LPTMR0_CSR & (1<<7)) == 0);                       //While timer not finished
		LPTMR0_CSR |=(1<<7);		                            //apaga bandera y mantiene configuracion
	}
	LPTMR0_CMR=20-1;		                                 // Waiting for 20 ms
	LPTMR0_CSR=(1<<6)+1;	//Hab intr y Enable timer
	NVIC_ISER|=(1<<28);		//hab intr LPTMR desde NVIC
}

void WiFi_send(char message[]){
	unsigned char len = 0;
	while(message[len] != '\0') len++;
	char open[] = "AT+CIPSEND=i,le";
	char close[] = "AT+CIPCLOSE=i";
	
	open[11] = id;
	open[13] = (len/10) + '0';
	open[14] = (len%10) + '0';
	close[12] = id;
	
	UART1_write(open);
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(message);
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(close);
	UART1_read("OK");
	while(!(messageComplete));
}

void WiFi_sendInt(int data){
	//ITOA
	char string[3] = {0};
	string[2] = (data%10) + '0';
	data /= 10;
	string[1] = (data%10) + '0';
	string[0] = (data/10) + '0';
	
	char open[] = "AT+CIPSEND=i,13";
	char close[] = "AT+CIPCLOSE=i";
	char message[] = "<h1> ### </h1>";
	
	open[11] = id;
	close[12] = id;
	message[5] = string[0];
	message[6] = string[1];
	message[7] = string[2];
	
	UART1_write(open);
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(message);
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(close);
	UART1_read("OK");
	while(!(messageComplete));
}

void WiFi_sendInfo(){
	unsigned char len = 0;
	char open[] = "AT+CIPSEND=i,le";
	char close[] = "AT+CIPCLOSE=i";
	
	//DISTANCIA
	char string_dist[3] = {0};
	string_dist[2] = (distancia%10) + '0';
	distancia /= 10;
	string_dist[1] = (distancia%10) + '0';
	string_dist[0] = (distancia/10) + '0';
	//RPM1
	char string_rpm1[3] = {0};
	string_rpm1[2] = (rpm1_promedio%10) + '0';
	rpm1_promedio /= 10;
	string_rpm1[1] = (rpm1_promedio%10) + '0';
	string_rpm1[0] = (rpm1_promedio/10) + '0';
	//RPM2
	char string_rpm2[3] = {0};
	string_rpm2[2] = (rpm2_promedio%10) + '0';
	rpm2_promedio /= 10;
	string_rpm2[1] = (rpm2_promedio%10) + '0';
	string_rpm2[0] = (rpm2_promedio/10) + '0';
	
	char message[] = "{\"mensaje\":\"ESTADO DEL AGV\", \"distancia\":\"###\", \"RPM_der\":\"###\", \"RPM_izq\":\"###\"}";
	len = sizeof(message)/sizeof(message[0]);
	message[42] = string_dist[0];
	message[43] = string_dist[1];
	message[44] = string_dist[2];
	message[59] = string_rpm1[0];
	message[60] = string_rpm1[1];
	message[61] = string_rpm1[2];
	message[76] = string_rpm2[0];
	message[77] = string_rpm2[1];
	message[78] = string_rpm2[2];
	
	open[11] = id;
	open[13] = (len/10) + '0';
	open[14] = (len%10) + '0';
	close[12] = id;
	
	UART1_write(open);
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(message);
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(close);
	UART1_read("OK");
	while(!(messageComplete));
}

void WiFi_execute_instructions(){
	char instruction[100] = "";
	char pre_instruction[] = "GET /";
	char pre_id[] = "+IPD,";
	unsigned char instruction_obtained = 0;
	unsigned char pre_id_found = 0;
	unsigned char pre_inst_found = 0;
	unsigned char inst_ind = 0;
	unsigned char i = 0;
	
	UART1_read(" HTTP/");
	while(!(messageComplete));
	
	//FINDING INSTRUCTION
	while (!(instruction_obtained)){
		if (pre_id_found){
			if (pre_inst_found){
				instruction[inst_ind] = buffer[i];
				inst_ind++;
				if (buffer[i] == '\0'){
					instruction_obtained = 1;
					instruction[inst_ind-7] = '\0';
				}
			}else{
				if (buffer[i] == pre_instruction[inst_ind]){           //Coincidence found
					inst_ind++;
					if (pre_instruction[inst_ind] == '\0'){            //Found GET /
						pre_inst_found = 1;
						inst_ind = 0;
					}
				}else inst_ind = 0;
			}
		}else{
			if (buffer[i] == pre_id[inst_ind]){                        //Coincidence found
				inst_ind++;
				if (pre_id[inst_ind] == '\0'){                         //Fount +IPD,
					pre_id_found = 1;
					id = buffer[++i];
					inst_ind = 0;
				}
			}else inst_ind = 0;
		}
		i++;
	}
	
	switch (instruction[0]){
		case 'g':
		case 'G':
			GPIOB_PTOR |= (1<<19);                      //Toggle green led
			break;
		case 'b':
		case 'B':
			GPIOE_PTOR |= (1<<4);                       //Toggle blue led
			break;
		case 'w':
		case 'W':                                      //Moving forward
			duty_cycle1 = 60;                         
			duty_cycle2 = 60;
			break;
		case 's':
		case 'S':									   //Stop moving
			duty_cycle1 = 0;
			duty_cycle2 = 0;
			break;
		case 'a':
		case 'A':                                      //Turn left
			duty_cycle1 = 20;
			duty_cycle2 = 60;
			break;
		case 'd':
		case 'D':                                      //Turn right
			duty_cycle1 = 60;
			duty_cycle2 = 20;
			break;
		default:
			break;
	}
	TPM1_update();
	WiFi_sendInfo();
}
