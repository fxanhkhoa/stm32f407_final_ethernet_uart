#include "DHT11_library.h"

int ERROR_NONE = 0,
    BUS_BUSY =1,
    ERROR_NOT_PRESENT =2 ,
    ERROR_ACK_TOO_LONG =3 , 
    ERROR_SYNC_TIMEOUT = 4, 
    ERROR_DATA_TIMEOUT =5 ,	
    ERROR_CHECKSUM = 6,	
    ERROR_NO_PATIENCE =7;

/****************************
*SET INPUT FOR DHT PORT_PIN*
****************************/
void DHT11_SetInput(DHT11 dht11)
{
	GPIO_InitTypeDef GPIO;
	GPIO.GPIO_Mode = GPIO_Mode_IN;
	GPIO.GPIO_Pin = dht11.DHT_PIN;
	GPIO.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(dht11.DHT_PORT, &GPIO);
}
/****************************
*SET OUTPUT FOR DHT PORT_PIN*
****************************/
void DHT11_SetOutput(DHT11 dht11)
{
	GPIO_InitTypeDef GPIO;
	GPIO.GPIO_Mode = GPIO_Mode_OUT;
	GPIO.GPIO_Pin = dht11.DHT_PIN;
	GPIO.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(dht11.DHT_PORT, &GPIO);
}


int Read_RawData(DHT11 dht11, uint8_t data[])
{
		/*---- W8 for 50us for the last pull down to back to free status ----*/
	//k = 0;
	//while (k < 50) ;
	/*----Send start signal----*/
		/*---- Set out put ----*/
			DHT11_SetOutput(dht11);
		/*---- Pull up for 60us ----*/
			/*GPIO_SetBits(GPIOA, GPIO_Pin_5);*/
			k = 0;
			//while (k < 50) ;
		/*---- Pull low for 18ms ----*/
			GPIO_ResetBits(dht11.DHT_PORT, dht11.DHT_PIN);
			k = 0;
			while (k < 180000);
	/*---- Pull up voltage and w8 response ----*/
		/*---- Set data as input ----*/
			DHT11_SetInput(dht11);
		/*---- Set pull-up ----*/
			GPIO_SetBits(dht11.DHT_PORT, dht11.DHT_PIN);
		/* Wait for response from sensor, 20-40탎 according to datasheet */
			k = 0;
			while (GPIO_ReadInputDataBit(dht11.DHT_PORT, dht11.DHT_PIN)) 
			{
				if (k > 40) 
					{
						//U_Print_Char(USART2, "no response 1\r\n"); 
						return BUS_BUSY;
					}
			}
		/* wait 1st response */
			k = 0;
			while (!GPIO_ReadInputDataBit(dht11.DHT_PORT, dht11.DHT_PIN)) 
				{
					if (k > 110) 
						{
							//U_Print_Char(USART2, "no response 2\r\n");
							return ERROR_NOT_PRESENT;
						}
				}
		/* wait 2nd response (end ready signal)*/
			k = 0;
			while (GPIO_ReadInputDataBit(dht11.DHT_PORT, dht11.DHT_PIN)) 
				{
					if (k > 110) 
						{
							//U_Print_Char(USART2, "no res 3\r\n");
							return ERROR_ACK_TOO_LONG;
						}
				}
	/*---- Get data ----*/
	for (int i = 0; i < 5; i++)
	{
		for (int j = 7; j >= 0; j--)
		{
			/* First there is always a 50탎 low period */
			k = 0;
			while (!GPIO_ReadInputDataBit(dht11.DHT_PORT, dht11.DHT_PIN)) 
			{
				if (k > 60) 
				{
					//U_Print_Char(USART2,"No respone 4\r\n");
					return ERROR_SYNC_TIMEOUT;
				}
			}
				
				
			
			/* Then the data signal is sent. 26 to 28탎 (ideally)
             indicate a low bit, and around 70탎 a high bit */
			k = 0;
			while (GPIO_ReadInputDataBit(dht11.DHT_PORT, dht11.DHT_PIN) && (k <= 100)) 
				{
					if (k == 100) 
					{
						//U_Print_Char(USART2, "Error data\r\n");
						return ERROR_DATA_TIMEOUT;
					}
				}
				//U_Print_Char(USART2, "out\r\n");
				/*---- datasheet if timer in 26-28us -> 0 and more than 70us -> 1----*/
				
				 if (k >= 20 &&  k <= 30)
            { 
							data[i] &= ~(1 << j);
							/*U_Print(USART2, i);
							U_Print(USART2, j);
							U_Print(USART2, 0);
							U_Print_Char(USART2, "\r\n");*/
						}
            
         else if (k >= 70)
						{ 
							data[i] |= (1 << j);
							/*U_Print(USART2, i);
							U_Print(USART2, j);
							U_Print(USART2, 1);
							U_Print_Char(USART2, "\r\n");*/
						}
		}
	}
	/********************* Sensor communication end *********************/
	//uint8_t check = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
    
		if (((data[0] + data[1] + data[2] + data[3]) & 0xFF) != data[4]) 
		{
			//U_Print_Char(USART2, "Error check sum\r\n");
			return ERROR_CHECKSUM;
		}
	return ERROR_NONE;
}
