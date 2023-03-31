
/* Includes */
#include "main.h"
#include "apm32f4xx_int.h"

extern uint8_t gb_spi_dma_started;
extern uint8_t gb_uart_tx_complete_flag;
extern uint8_t gb_uart_rx_complete_flag;
extern uint8_t gn_spi_rx_complete_flag;

extern uint32_t gn_sw_tickcount;

/*!
 * @brief     This function handles NMI exception
 *
 * @param     None
 *
 * @retval    None
 *
 */
void NMI_Handler(void)
{
}

/*!
 * @brief     This function handles Hard Fault exception
 *
 * @param     None
 *
 * @retval    None
 *
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief     This function handles Memory Manage exception
 *
 * @param     None
 *
 * @retval    None
 *
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief     This function handles Bus Fault exception
 *
 * @param     None
 *
 * @retval    None
 *
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief     This function handles Usage Fault exception
 *
 * @param     None
 *
 * @retval    None
 *
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief     This function handles SVCall exception
 *
 * @param     None
 *
 * @retval    None
 *
 */
void SVC_Handler(void)
{
}

/*!
 * @brief     This function handles Debug Monitor exception
 *
 * @param     None
 *
 * @retval    None
 *
 */
void DebugMon_Handler(void)
{
}

/*!
 * @brief     This function handles PendSV_Handler exception
 *
 * @param     None
 *
 * @retval    None
 *
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief     This function handles SysTick Handler
 *
 * @param     None
 *
 * @retval    None
 *
 */
void SysTick_Handler(void)
{
    if(gn_sw_tickcount > 0)
    {
        --gn_sw_tickcount;
    }
}

/**@} end of group ADC_DMA_INT_Functions */
/**@} end of group ADC_DMA */



void DMA2_STR7_IRQHandler(void)
{
    if(DMA_ReadStatusFlag(DMA2_Stream7, DMA_FLAG_TCIFLG7) == SET)
    {
        DMA_ClearStatusFlag(DMA2_Stream7, DMA_FLAG_TCIFLG7);
        DMA_Disable(DMA2_Stream7);

        gb_uart_tx_complete_flag = 0;
    }
    else if(DMA_ReadStatusFlag(DMA2_Stream7, DMA_FLAG_TEIFLG7) == SET)
    {
        DMA_ClearStatusFlag(DMA2_Stream7, DMA_FLAG_TEIFLG7);
    }
}

/**
  * @brief This function handles USART1 global interrupt
  */
void USART1_IRQHandler(void)
{
    /** USART1 Recieve Data */
    if(USART_ReadStatusFlag(USART1, USART_FLAG_RXBNE) == SET)
    {
        /* Read one byte from the receive data register */
        uint8_t rx_data = (uint8_t)USART_RxData(USART1);

        // echo
        while(USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);
        USART_TxData(USART1, rx_data);

        if((rx_data == '\n') || (rx_data == '\r'))
        {
            gb_uart_rx_complete_flag = 1;
        }
    }
}

