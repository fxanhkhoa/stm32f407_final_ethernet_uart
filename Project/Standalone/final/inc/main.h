#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif 
	
#define NONE 1
#define ID   2
#define DS1037 99
	
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_usart.h"
#include "misc.h"
#include "U_Print.h"
#include <stdio.h>
#include <string.h>
#include "AT_Command.h"
#include "DS1307.h"
#include "function.h"
#include "25lc256.h"
	
	
void Init_peripheral();

	
#ifdef __cplusplus
}
#endif

#endif /* _MAIN_H_ */
