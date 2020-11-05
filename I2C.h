/*
 * I2C.h
 *
 *  Created on: Nov 2, 2020
 *      Author: jprr2
 */

#ifndef I2C_H_
#define I2C_H_

void CLK_init();
void I2C0_init();
void i2c_DisableAck();
void i2c_EnableAck();
void i2c_RepeatedStart();
void i2c_EnterRxMode();
void i2c_Start();
void i2c_Stop();
void i2c_Wait();
void i2c_WriteByte(uint8_t data);
int8_t i2c_ReadByte();
void delay(uint32_t nice);
void i2c_WriteRegister(uint8_t slave, uint8_t registerAdd, uint8_t data);
int8_t i2c_ReadMultRegister(uint8_t SlaveAddress,uint8_t RegisterAddress,uint8_t n_data, uint8_t *res);
void i2c_WriteMultRegister(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t n_data, uint8_t *data);
void IMU_setup ();
void IMU_data();

#endif /* I2C_H_ */
