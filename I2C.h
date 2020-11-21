/*
 * I2C.h
 *
 *  Created on: Nov 18, 2020
 *      Author: SCADA_Admin
 */

#ifndef I2C_H_
#define I2C_H_

void I2C_init (void);
void i2c_DisableAck(void);
void i2c_EnableAck(void);
void i2c_RepeatedStart(void);
void i2c_EnterRxMode(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_Wait(void);
void i2c_WriteByte(uint8_t data);
uint8_t i2c_ReadByte(void);
void delay(uint32_t t);
void i2c_WriteRegister(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t data);
int8_t i2c_ReadRegister(uint8_t SlaveAddress,uint8_t RegisterAddress);
void i2c_ReadMultRegister(uint8_t SlaveAddress,uint8_t RegisterAddress,uint8_t n_data, uint8_t *res);
void i2c_WriteMultRegister(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t n_data, uint8_t *data);
void IMU_setup (void);
void IMU_data(void);
void Process_Data(void);


#endif /* I2C_H_ */
