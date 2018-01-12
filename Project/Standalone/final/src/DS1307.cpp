#include "DS1307.h"

DS1307::DS1307()
{
}

DS1307::~DS1307()
{
}

void DS1307::DS1307Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(DS_1307_PORT_RCC, ENABLE);

	/* Enable UART clock */
	RCC_APB1PeriphClockCmd(DS_1307_RCC, ENABLE);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = DS_1307_SCL | DS_1307_SDA;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS_1307_PORT, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(DS_1307_PORT, GPIO_PinSource10, GPIO_AF_I2C2);
	GPIO_PinAFConfig(DS_1307_PORT, GPIO_PinSource11, GPIO_AF_I2C2);

	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = DS1307_I2C_MASTER_ADDRESS;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	/* Enable I2C */
	I2C_Init(DS1307_I2C, &I2C_InitStructure);
	I2C_Cmd(DS1307_I2C, ENABLE);
}

void DS1307::DS1307ReadTime()
{
	uint8_t index;
	uint8_t values[8] = { 0 };
	
	I2C_AcknowledgeConfig(DS1307_I2C, ENABLE);

	// Start setting register
	I2C_GenerateSTART(DS1307_I2C, ENABLE);
	while (!I2C_CheckEvent(DS1307_I2C, I2C_EVENT_MASTER_MODE_SELECT))
		;

	I2C_Send7bitAddress(DS1307_I2C, DS1307_I2C_SLAVE_ADDRESS, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(DS1307_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(DS1307_I2C, 0x00);
	while (!I2C_CheckEvent(DS1307_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(DS1307_I2C, ENABLE);
	while (!I2C_CheckEvent(DS1307_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(DS1307_I2C, DS1307_I2C_SLAVE_ADDRESS, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(DS1307_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	for (index = 0; index < 8; index++) {
		while (!I2C_CheckEvent(DS1307_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
			;
		values[index] = I2C_ReceiveData(DS1307_I2C);
		if (index == 6) {
			I2C_NACKPositionConfig(DS1307_I2C, I2C_NACKPosition_Current);
			I2C_AcknowledgeConfig(DS1307_I2C, DISABLE);
		}
	}
	
	I2C_GenerateSTOP(DS1307_I2C, ENABLE);
	while (I2C_GetFlagStatus(DS1307_I2C, I2C_FLAG_STOPF));
	
	this->sec1 = (values[0] & 0xF0) >> 4;
	this->sec0 = values[0] & 0x0F;
	this->min1 = (values[1] & 0xF0) >> 4;
	this->min0 = values[1] & 0x0F;
	this->hour1 = (values[2] & 0x30) >> 4;
	this->hour0 = values[2] & 0x0F;
	this->day1 = (values[4] & 0x30) >> 4;
	this->day0 = values[4] & 0x0F;
	this->month1 = (values[5] & 0x10) >> 4;
	this->month0 = values[5] & 0x0F;
	this->year1 = (values[6] & 0xF0) >> 4;
	this->year0 = values[6] & 0x0F;
}
void DS1307::DS1307SetTime()
{
	uint8_t index;
	uint8_t values[8] = { 0x00, 0x15, 0x17, 0x01, 0x15, 0x05, 0x13, 0x10 };

	I2C_AcknowledgeConfig(DS1307_I2C, ENABLE);

	// Start setting register
	I2C_GenerateSTART(DS1307_I2C, ENABLE);
	while (!I2C_CheckEvent(DS1307_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(DS1307_I2C, DS1307_I2C_SLAVE_ADDRESS, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(DS1307_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(DS1307_I2C, 0x00);
	while (!I2C_CheckEvent(DS1307_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	for (index = 0; index < 8; index++) {
		I2C_SendData(DS1307_I2C, values[index]);
		while (!I2C_CheckEvent(DS1307_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}

	I2C_NACKPositionConfig(DS1307_I2C, I2C_NACKPosition_Current);
	I2C_AcknowledgeConfig(DS1307_I2C, DISABLE);

	I2C_GenerateSTOP(DS1307_I2C, ENABLE);

	while (I2C_GetFlagStatus(DS1307_I2C, I2C_FLAG_STOPF));
}

void DS1307::DS1307GetTimeString() 
{
	time[0] = this->hour1 + 0x30;
	time[1] = this->hour0 + 0x30;
	time[2] = 0x3A;
	time[3] = this->min1 + 0x30;
	time[4] = this->min0 + 0x30;
	time[5] = 0x3A;
	time[6] = this->sec1 + 0x30;
	time[7] = this->sec0 + 0x30;
	time[8] = 0x00;
}
