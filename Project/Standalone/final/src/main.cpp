#include "main.h"

/******* NOTE *******
*
*		EEPROM starts from 0x0010
*		Each Member needs 23 address
* 	Each address contains 8 bytes
*
*********************/

/********** Global variables ***********/
uint32_t time = 0;
char *s = new char[40], *s1 = new char[40]; uint8_t pos,pos1 = 0; // Use for USART
char mode = NONE;
char _read = 1;
uint16_t number;
uint8_t quantity;


int main()
{
	DS1307 real_time;
	SystemInit();
	Init_peripheral();
	
	GPIO_ResetBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_12);
	//GPIO_ResetBits(GPIOD, GPIO_Pin_12);
	GPIO_ResetBits(GPIOD, GPIO_Pin_15 | GPIO_Pin_14);
	//GPIO_ResetBits(GPIOD, GPIO_Pin_14);
	
	/********** Variables ***********/

	
	//quantity = 0;
	_read = 1;
	quantity = EEPROM_readByte(0x0009);
		time = 0;
		while (time < 100)

	while (1)
	{
		//U_Print(USART1, quantity);
		//mode = CheckAT(s);
		if (mode == NONE)
		{
	
		}
		else if (mode == IDCHECK)
		{
			if (_read == 0)
			{
				//U_Print_Char(USART1,"IDCHECK");
				char *temps = new char[8];
				temps[0] = s[10];temps[1] = s[11];
				temps[2] = s[12];temps[3] = s[13];
				temps[4] = s[14];temps[5] = s[15];
				temps[6] = s[16];temps[7] = s[17];
				
				// Other option
				U_Print_Char(USART1,"OK+IDREAD+");
				U_Print_Char(USART1,temps);
				U_Print_Char(USART1,"*");
				
				_read = 1;
				mode = NONE;
				strcpy(s,"");
				GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
				delete[] temps;
			}
			else if (_read == 1)
			{
				char *temps = new char[8];
				temps[0] = s[10];temps[1] = s[11];
				temps[2] = s[12];temps[3] = s[13];
				temps[4] = s[14];temps[5] = s[15];
				temps[6] = s[16];temps[7] = s[17];
				
				//U_Print_Char(USART1,temps);
				
				mode = Check_Open_Door(s);
				//mode = NONE;
			}
		}
		else if (mode == DS1037)
		{
			real_time.DS1307ReadTime();
			U_Print(USART2, real_time.hour1);
			U_Print(USART2, real_time.hour0);
			U_Print_Char(USART2,"\n");
			U_Print(USART2, real_time.min1);
			U_Print(USART2, real_time.min0);
			U_Print_Char(USART2,"\n");
			U_Print(USART2, real_time.sec1);
			U_Print(USART2, real_time.sec0);
			U_Print_Char(USART2,"\n");
			time = 0;
			while (time < 100000 / 2);
		}
		else if  (mode == OPEN1)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_15);
			OpenDoor(0);
			U_Print_Char(USART1, "1*");
			mode = NONE;
			time = 0;
			while (time < 50000 * 1);
			CloseDoor(0);
			GPIO_ResetBits(GPIOD,GPIO_Pin_15);
		}
		else if  (mode == OPEN2)
		{
			OpenDoor(1);
			U_Print_Char(USART1, "2*");
			mode = NONE;
			time = 0;
			while (time < 50000 * 2);
			CloseDoor(1);
		}
		else if  (mode == OPEN3)
		{
			OpenDoor(2);
			U_Print_Char(USART1, "3*");
			mode = NONE;
			time = 0;
			while (time < 50000 * 2);
			CloseDoor(2);
		}
		else if  (mode == OPEN4)
		{
			OpenDoor(3);
			U_Print_Char(USART1, "4*");
			mode = NONE;
			time = 0;
			while (time < 50000 * 2);
			CloseDoor(3);
		}
		else if (mode == IDREAD)
		{
			GPIO_SetBits(GPIOD, GPIO_Pin_15);
			_read = 0;
		}
		else if (mode == IDADD)
		{			
			number = 0x0010 + quantity * NUMBER_OF_BLOCK ;
			char temp[11];
			
			// Get RFID
			strcpy(temp,IDADD_READ_RFID(s1));
//			U_Print_Char(USART1, temp);
//			U_Print_Char(USART1, "\n");
			for (int i = 0; i < 8; i++)
			{
				EEPROM_writeByte(number, temp[i]);
				time = 0;
				while (time < 100);
				GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
				number += 0x0001;
				
			}
			
			// Get TIME
			strcpy(temp,IDADD_READ_TIME(s1));
		
			for (int i = 0; i < 11; i++)
			{
				EEPROM_writeByte(number, temp[i]);
				time = 0;
				while (time < 100);
				GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
				number += 0x0001;
			}
				
			// Get DOOR
			strcpy(temp,IDADD_READ_DOOR(s1));
				U_Print_Char(USART1, temp);
				EEPROM_writeByte(number, temp[0]);
				time = 0;
				while (time < 100);
				GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
				number += 0x0001;
			
			
//			RFID_ID[number].ADDR[8] = '\0';
//			RFID_ID[number].TIME_SEND[11] = '\0';
//			RFID_ID[number].DOOR[4] = '\0';			
			quantity ++;
			EEPROM_writeByte(0x0009,quantity);
			//U_Print_Char(USART1,RFID_ID[number].ADDR);
			//U_Print_Char(USART1,RFID_ID[number].TIME_SEND);
			//U_Print_Char(USART1,RFID_ID[number].DOOR);
			//U_Print(USART1,quantity);
			U_Print_Char(USART1, s1);
			U_Print_Char(USART1, " Added!!! \n");
			U_Print(USART1, quantity);
			U_Print_Char(USART1,"\n");
			strcpy(s1,"");
			mode = NONE;
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		}
		else if (mode == IDDEL)
		{
			char temps[8];
				temps[0] = s[9];temps[1] = s[10];
				temps[2] = s[11];temps[3] = s[12];
				temps[4] = s[13];temps[5] = s[14];
				temps[6] = s[15];temps[7] = s[16];
			ID_DEL(temps);
		}
		else if (mode == CLEARALL)
		{
			EEPROM_writeByte(0x0009,0);
			time = 0;
			while (time < 100);
			U_Print_Char(USART1,"Clear ALL \n");
			quantity = EEPROM_readByte(0x0009);
			mode = NONE;
		}
	}
}

extern "C" void USART2_IRQHandler(void)
{
	// check if the USART1 receive interrupt flag was set
	if( USART_GetITStatus(USART2, USART_IT_RXNE) )
		{
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
			s[pos] = USART_ReceiveData(USART2);
//			char sk[2];
//			sk[0] = s[pos];
//			U_Print_Char(USART1,sk);
			if (s[pos] == '*')
			{
				GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
				//U_Print_Char(USART1,s);
				mode = CheckAT(s);
				pos = 0;
				if (mode != IDCHECK)
				{
					strcpy(s,"");
				}
			}
			else pos ++;
		}
}

extern "C" void USART1_IRQHandler(void)
{
	// check if the USART1 receive interrupt flag was set
	if( USART_GetITStatus(USART1, USART_IT_RXNE) )
		{
			s1[pos1] = USART_ReceiveData(USART1);
			if (s1[pos1] == '*')
			{
				//U_Print_Char(USART1,s1);
				GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
				mode = CheckAT(s1);
				//U_Print(USART1,mode);
				pos1 = 0;
				if (mode != IDADD)
				{
					strcpy(s1,"");
				}
			}
			else pos1 ++;
		}
}

void TIM4_IRQHandler()
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
				//time_now ++;
				time++;
        //led_toggle();
    }
}

void SysTick_Init(void) {
	/****************************************
	 *SystemFrequency/1000      1ms         *
	 *SystemFrequency/100000    10us        *
	 *SystemFrequency/1000000   1us         *
	 *****************************************/
	while (SysTick_Config(SystemCoreClock / 1000000) != 0) {
	} // One SysTick interrupt now equals 1us

}

extern "C" void SysTick_Handler(void) {

}

void Init_peripheral()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,	ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	
	/*---- GPIO Init ----*/
	GPIO_InitTypeDef GPIO;
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO.GPIO_Mode = GPIO_Mode_OUT;
	GPIO.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO);

	GPIO.GPIO_Mode = GPIO_Mode_OUT;
	GPIO.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO);
	
	/*---- Relay GPIO Init ----*/
	
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
	
	/*---- PB6-TX, PB7-RX, PD5-TX, PD6-RX ----*/
	GPIO.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO.GPIO_Mode = GPIO_Mode_AF;
	GPIO.GPIO_OType = GPIO_OType_PP;
	GPIO.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO);
	
	GPIO.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO.GPIO_Mode = GPIO_Mode_AF;
	GPIO.GPIO_OType = GPIO_OType_PP;
	GPIO.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO);
	
	/*GPIO.GPIO_Pin = GPIO_Pin_10;
	GPIO.GPIO_Mode = GPIO_Mode_IN;
	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO);*/
	
	/*---- Timer Init ----*/
	// Timer 4 -----> Max 50Mhz
	TIM_TimeBaseInitTypeDef TIMER;
	TIMER.TIM_ClockDivision = TIM_CKD_DIV1;
	TIMER.TIM_Prescaler = 1000;
	TIMER.TIM_CounterMode = TIM_CounterMode_Up;
	TIMER.TIM_Period = 2-1;
	TIMER.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIMER);
	TIM_Cmd(TIM4, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	/*---- USART Init ----*/
	USART_InitTypeDef USART;
	USART.USART_BaudRate = 9600;
	USART.USART_StopBits = USART_StopBits_1;
	USART.USART_Parity = USART_Parity_No;
	USART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt 
	USART_Cmd(USART2, ENABLE);
	
	USART.USART_BaudRate = 115200;
	USART.USART_StopBits = USART_StopBits_1;
	USART.USART_Parity = USART_Parity_No;
	USART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt 
	USART_Cmd(USART1, ENABLE);
	
	/*---- SPI Init ----*/
	/* configure pins used by SPI1
	 * PC10 = SCK
	 * PC11 = MISO
	 * PC12 = MOSI
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	// connect SPI1 pins to SPI alternate function
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);
	
	// Chip Select Pin
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
	
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SPI frequency is APB2 frequency / 4
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
	SPI_Init(SPI3, &SPI_InitStruct); 
	
	SPI_Cmd(SPI3, ENABLE); // enable SPI1
	
	
	/*----- NVIC Timer interrupt -----*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/*----- NVIC USART2 interrupt -----*/
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/*----- NVIC USART1 interrupt -----*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
