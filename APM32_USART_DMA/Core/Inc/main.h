/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Includes */
#include "apm32f4xx.h"

/* Private includes ----------------------------------------------------------*/
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_misc.h"

#include "apm32f4xx_gpio.h"
#include "apm32f4xx_spi.h"
#include "apm32f4xx_dma.h"
#include "apm32f4xx_adc.h"
#include "apm32f4xx_usart.h"


#define UART_TXD_Pin GPIO_PIN_9
#define UART_TXD_GPIO_Port GPIOA
#define UART_RXD_Pin GPIO_PIN_10
#define UART_RXD_GPIO_Port GPIOA


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
