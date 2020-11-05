/*
 * WiFi.h
 *
 *  Created on: Oct 29, 2020
 *      Author: jprr2
 */

#ifndef WIFI_H_
#define WIFI_H_

void WiFi_setup();
void WiFi_find_instructions();
void WiFi_send(char id, char message[]);
void WiFi_sendInt(int data);
void WiFi_execute_instructions();

#endif /* WIFI_H_ */
