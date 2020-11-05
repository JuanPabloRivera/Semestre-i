/*
 * I2C.c
 *
 *  Created on: Nov 2, 2020
 *      Author: jprr2
 */

#include "derivative.h"
#include "math.h"

#define MPU6050_ADDR         0x68
#define MPU6050_SMPLRT_DIV   0x19
#define MPU6050_CONFIG       0x1a
#define MPU6050_GYRO_CONFIG  0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_PWR_MGMT_1   0x6b

#define MPU6050_DATA         0x3b

#define GYRO_LSB_2_DEGSEC  65.5     // [bit/(°/s)]
#define ACC_LSB_2_G        16384.0  // [bit/gravity]
#define RAD_2_DEG          57.29578 // [°/rad]
#define GYRO_OFFSET_NB_MES 3000     //
#define TEMP_LSB_2_DEGREE  340.0    // [bit/celsius]
#define TEMP_LSB_OFFSET    12412.0  //
#define DEFAULT_GYRO_COEFF 0.98

double       AngX,AngY,GyrX,GyrY,Temp;
const double R2G = 180.0/3.141592654;
const double ASF = 16384.0;
const double GSF = 131.0;

float gyroXoffset, gyroYoffset, gyroZoffset;
float temp, accX, accY, accZ, gyroX, gyroY, gyroZ;
float angleAccX, angleAccY;
float angleX, angleY, angleZ;
long preInterval;
float accCoef, gyroCoef;

int16_t Acc_rawX, Acc_rawY, Acc_rawZ, Mpu_rawT, Gyr_rawX, Gyr_rawY, Gyr_rawZ;

void CLK_init (void)
{
// Choose internal clock of 4MHz: (MCG-24)
MCG_C1|=(1<<6) + (1<<1);								//Enable both MCGOUTCLK and MCGIRCLK
MCG_C2|=1;												//Choose fast clock of 4MHz
MCG_SC=0;												//Preescaler of 1:1

// Clock configurations for the SIM: (SIM-12)
SIM_CLKDIV1=0;											//Set both system dividers OUTDIV4 and OUTDIV1 to 1:1
SIM_SOPT2|=15<<24;										//Select the clock MCGIRCLK for the UART0 and TPM

}

void i2c_DisableAck(void)
{
	I2C0_C1 |= (1<<3);								//NACK
}


void i2c_EnableAck(void)
{
	I2C0_C1 &= ~(1<<3);
}


void i2c_RepeatedStart(void)
{
	I2C0_C1 |= (1<<2);
}


void i2c_EnterRxMode(void)
{
	I2C0_C1 &= ~(1<<4);
}


void i2c_Start(void)
{
	I2C0_C1 |= (1<<4);								//Send
	I2C0_C1 |= (1<<5);								//Master
}

void i2c_Stop(void)
{
	I2C0_C1 &= ~(1<<5);								//Slave
	I2C0_C1 &= ~(1<<4);								//Receiver
}

void i2c_Wait(void)
{
	uint32_t i = 0;
	i = 1000000;

	while(((I2C0_S & (1<<1)) == 0) && i)			//Interrupt pending
	{
		i--;
	}
	I2C0_S |= (1<<1);
}

void i2c_WriteByte(uint8_t data)
{
	I2C0_D = (uint8_t)data;
}

uint8_t i2c_ReadByte(void)
{
	return (uint8_t)( I2C0_D);
}

void delay(uint32_t t)
{
	uint32_t cnt = 0;
	for(cnt=0; cnt<t; cnt++)
	{
		__asm("nop");
	};
}

void i2c_WriteRegister(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t data)
{
	
	i2c_Start();
	i2c_WriteByte((SlaveAddress << 1));
	i2c_Wait();

	i2c_WriteByte(RegisterAddress);
	i2c_Wait();

	i2c_WriteByte(data);
	i2c_Wait();

	i2c_Stop();

	delay(50);
}

int8_t i2c_ReadRegister(uint8_t SlaveAddress,uint8_t RegisterAddress)
{
	uint8_t res = 0;

	i2c_Start();
	i2c_WriteByte((SlaveAddress << 1));
	i2c_Wait();

	i2c_WriteByte(RegisterAddress);
	i2c_Wait();

	i2c_RepeatedStart();

	i2c_WriteByte(((SlaveAddress << 1) | 1));
	i2c_Wait();

	i2c_EnterRxMode();
	i2c_DisableAck();

	res = i2c_ReadByte();
	i2c_Wait();
	i2c_Stop();
	res = i2c_ReadByte();
	delay(50);

	return res;
}

int8_t i2c_ReadMultRegister(uint8_t SlaveAddress,uint8_t RegisterAddress,uint8_t n_data, uint8_t *res)
{
	uint8_t i = 0;

	i2c_Start();
	i2c_WriteByte((SlaveAddress << 1));
	i2c_Wait();

	i2c_WriteByte(RegisterAddress);
	i2c_Wait();

	i2c_RepeatedStart();

	i2c_WriteByte(((SlaveAddress << 1) | 1));
	i2c_Wait();

	i2c_EnterRxMode();
	i2c_EnableAck();

	i = i2c_ReadByte();
	i2c_Wait();

	for(i=0;i<(n_data-2);i++)
	{
		*res = i2c_ReadByte();
		res++;
		i2c_Wait();
	}

	i2c_DisableAck();

	*res = i2c_ReadByte();
	res++;
	i2c_Wait();

	i2c_Stop();

	*res = i2c_ReadByte();

	delay(50);
	return *res;
}


void i2c_WriteMultRegister(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t n_data, uint8_t *data)
{
	uint8_t i = 0;

	i2c_Start();
	i2c_WriteByte((SlaveAddress << 1));
	i2c_Wait();

	i2c_WriteByte(RegisterAddress);
	i2c_Wait();

	for(i=0;i<n_data;i++)
	{
		i2c_WriteByte(*data);
		i2c_Wait();
		data++;
	}
	i2c_Stop();

	delay(50);
}

void IMU_setup (void)
{
	i2c_WriteRegister(MPU6050_ADDR, MPU6050_SMPLRT_DIV,  0x00);
	i2c_WriteRegister(MPU6050_ADDR, MPU6050_CONFIG,      0x00);
	i2c_WriteRegister(MPU6050_ADDR, MPU6050_GYRO_CONFIG, 0x08);
	i2c_WriteRegister(MPU6050_ADDR, MPU6050_ACCEL_CONFIG,0x00);
	i2c_WriteRegister(MPU6050_ADDR, MPU6050_PWR_MGMT_1,  0x01);
}


void IMU_data(void)
{
	int8_t data[15];
	int RA;
	int i = 0;
		
	IMU_setup();
	while(i < 14)
	{
		RA = 59 + i;
		data[i] = i2c_ReadRegister(MPU6050_ADDR,RA);
		i++;
	}
	
	
	Acc_rawX = (data[0]<<8) |data[1];
	Acc_rawY = (data[2]<<8) |data[3];
	Acc_rawZ = (data[4]<<8) |data[5]; 
	Mpu_rawT = (data[6]<<8) |data[7];
	Gyr_rawX = (data[8]<<8) |data[9];
	Gyr_rawY = (data[10]<<8)|data[11];
	Gyr_rawZ = (data[12]<<8)|data[13];
	
	
	accX = ((float)Acc_rawX) / ACC_LSB_2_G;
	accY = ((float)Acc_rawY) / ACC_LSB_2_G;
	accZ = ((float)Acc_rawZ) / ACC_LSB_2_G;
	temp = (Mpu_rawT + TEMP_LSB_OFFSET) / TEMP_LSB_2_DEGREE;
	gyroX = ((float)Gyr_rawX) / GYRO_LSB_2_DEGSEC;
	gyroY = ((float)Gyr_rawY) / GYRO_LSB_2_DEGSEC;
	gyroZ = ((float)Gyr_rawZ) / GYRO_LSB_2_DEGSEC;
}
