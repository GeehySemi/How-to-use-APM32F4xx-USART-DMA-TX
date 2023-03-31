#include <stdio.h>
#include <string.h>

#include "main.h"


#define UART_MSG_CNT        20
#define UART_BUFFER_LEN     30
static char gn_uart_msg_buffer[UART_MSG_CNT][UART_BUFFER_LEN] =
{
    "apm32 gpio\r\n",
    "apm32 crc\r\n",
    "apm32 spi\r\n",
    "apm32 timer\r\n",
    "apm32 i2c\r\n",
    "apm32 uart\r\n",
    "apm32 usart\r\n",
    "apm32 dma\r\n",
    "apm32 dac\r\n",
    "apm32 adc\r\n",
    "apm32 rng\r\n",
    "apm32 wwdt\r\n",
    "apm32 can\r\n",
    "apm32 pmu\r\n",
    "apm32 sdio\r\n",
    "apm32 sai\r\n",
    "apm32 exit\r\n",
    "apm32 ltdc\r\n",
    "apm32 syscfg\r\n",
    "apm32 dummy\r\n",
};



static uint16_t gn_uart_dma_buffer_idx;


uint8_t gb_uart_tx_complete_flag;
uint8_t gb_uart_rx_complete_flag;




uint32_t gn_sw_tickcount;



#define LL_MAX_DELAY    0xFFFFFFFF

void SystemClock_Config(void);
static void APM32_DMA_Init(void);
static void APM32_USART1_UART_Init(void);


__STATIC_INLINE void LL_mDelay(uint32_t delay)
{
    __IO uint32_t tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
    ((void)tmp);

    /* Add a period to guaranty minimum wait */
    if(delay < LL_MAX_DELAY)
    {
        ++delay;
    }

    while(delay)
    {
        if((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0)
        {
            --delay;
        }
    }
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    __disable_irq();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */

    APM32_USART1_UART_Init();
    APM32_DMA_Init();

    LL_mDelay(50);

    gn_sw_tickcount = 1000;
    __enable_irq();
    while (1)
    {
        
        if(gn_sw_tickcount == 0)
        {
            gn_sw_tickcount = 1000;
        
            gb_uart_tx_complete_flag = 1;  
            DMA_ConfigMemoryTarget(DMA2_Stream7, (uint32_t)gn_uart_msg_buffer[gn_uart_dma_buffer_idx], DMA_MEMORY_0);
            DMA_ConfigDataNumber(DMA2_Stream7, strlen(gn_uart_msg_buffer[gn_uart_dma_buffer_idx]) + 1);
            DMA_Enable(DMA2_Stream7);  
            ++gn_uart_dma_buffer_idx;
            if(gn_uart_dma_buffer_idx == UART_MSG_CNT)
            {
                gn_uart_dma_buffer_idx = 0;
           }
        }
             
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    /** SystemFrequency / 1000 = 1ms */
    if(SysTick_Config(SystemCoreClock / 1000))
    {
        /** Capture error */
        while (1);
    }

    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_2);

    //RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
}




/**
  * Enable DMA controller clock
  */
static void APM32_DMA_Init(void)
{
  /* SPI2 DMA Init */
  /* DMA Configure */
    DMA_Config_T dmaConfig;

    /* Enable DMA clock */
    RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_DMA2);

   

    DMA_Reset(DMA2_Stream7);

    while (DMA_ReadCmdStatus(DMA2_Stream7) != DISABLE)
    {
    }

    /* size of buffer*/
    dmaConfig.bufferSize = 1;
    /* set memory Data Size*/
    dmaConfig.memoryDataSize = DMA_MEMORY_DATA_SIZE_BYTE;
    /* Set peripheral Data Size*/
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_BYTE;
    /* Enable Memory Address increase*/
    dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
    /* Disable Peripheral Address increase*/
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    /* Reset Circular Mode*/
    dmaConfig.loopMode = DMA_MODE_NORMAL;
    /* set priority*/
    dmaConfig.priority = DMA_PRIORITY_LOW;
    /* read from peripheral*/
    dmaConfig.dir = DMA_DIR_MEMORYTOPERIPHERAL;
    /* Set memory Address*/
    dmaConfig.memoryBaseAddr = (uint32_t)0;
    /* Set Peripheral Address*/
    dmaConfig.peripheralBaseAddr = (uint32_t)&USART1->DATA;

    dmaConfig.channel = DMA_CHANNEL_4;
    dmaConfig.fifoMode = DMA_FIFOMODE_DISABLE;
    dmaConfig.fifoThreshold = DMA_FIFOTHRESHOLD_FULL;
    dmaConfig.peripheralBurst = DMA_PERIPHERALBURST_SINGLE;
    dmaConfig.memoryBurst = DMA_MEMORYBURST_SINGLE;
    DMA_Config(DMA2_Stream7, &dmaConfig);

    /* Clear DMA TF flag*/
    DMA_ClearIntFlag(DMA2_Stream7, DMA_INT_TCIFLG7);
    /* Enable DMA Interrupt*/
    DMA_EnableInterrupt(DMA2_Stream7, DMA_INT_TCIFLG);
    DMA_EnableInterrupt(DMA2_Stream7, DMA_INT_TEIFLG);

    NVIC_EnableIRQRequest(DMA2_STR7_IRQn, 3, 1);

    //DMA_Enable(DMA2_Stream7);
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void APM32_USART1_UART_Init(void)
{
    USART_Config_T configStruct;
    GPIO_Config_T GPIO_configStruct;

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART1);

    configStruct.baudRate = 115200;
    configStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    configStruct.mode = USART_MODE_TX_RX;
    configStruct.parity = USART_PARITY_NONE;
    configStruct.stopBits = USART_STOP_BIT_1;
    configStruct.wordLength = USART_WORD_LEN_8B;

    /** Connect PXx to USARTx_Tx */
    GPIO_ConfigPinAF(UART_TXD_GPIO_Port, GPIO_PIN_SOURCE_9, GPIO_AF_USART1);
    /** Connect PXx to USARTx_Rx */
    GPIO_ConfigPinAF(UART_RXD_GPIO_Port, GPIO_PIN_SOURCE_10, GPIO_AF_USART1);

    /** Configure USART Tx as alternate function push-pull */
    GPIO_configStruct.pin = UART_TXD_Pin;
    GPIO_configStruct.mode = GPIO_MODE_AF;
    GPIO_configStruct.speed = GPIO_SPEED_50MHz;
    GPIO_configStruct.otype = GPIO_OTYPE_PP;
    GPIO_configStruct.pupd = GPIO_PUPD_NOPULL;
    GPIO_Config(UART_TXD_GPIO_Port, &GPIO_configStruct);

    /** Configure USART Rx as input pullup */
    GPIO_configStruct.pin = UART_RXD_Pin;
    GPIO_configStruct.mode = GPIO_MODE_AF;
    GPIO_configStruct.pupd = GPIO_PUPD_UP;
    GPIO_Config(UART_RXD_GPIO_Port, &GPIO_configStruct);

    /** USART configuration */
    USART_Config(USART1, &configStruct);

    USART_EnableDMA(USART1, USART_DMA_TX);

    /** Enable USART */
    USART_Enable(USART1);

    /** Enable USART1 RXBNE interrput */
    USART_EnableInterrupt(USART1, USART_INT_RXBNE);
    USART_ClearStatusFlag(USART1, USART_FLAG_RXBNE);
    NVIC_EnableIRQRequest(USART1_IRQn, 3, 0);
}



