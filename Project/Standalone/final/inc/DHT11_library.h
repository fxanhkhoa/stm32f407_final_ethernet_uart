#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"

extern uint32_t k;



struct DHT11{
	GPIO_TypeDef * DHT_PORT;
	uint16_t DHT_PIN;
};

void DHT11_SetOutput(DHT11 dht11);
void DHT11_SetInput(DHT11 dht11);
int Read_RawData(DHT11 dht11, uint8_t data[]);
