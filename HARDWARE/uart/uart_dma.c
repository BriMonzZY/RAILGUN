#include "uart_dma.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "main.h"
#include "numandstr.h"

uint8_t RxBuffer[UART_RX_BUF_SIZE] = {0};
uint8_t TxBuffer[UART_RX_BUF_SIZE] = {0};
uint8_t sendCompleteSign = 1;
uint8_t TxLen = 0;


/* 数据处理部分 */
/* 以空格作为分隔符，RxBuffer是串口接收缓冲数组，读到的都在这里 */
int32_t a,b,c;
double  d,e,f;
void DataProcess(void)
{
	//在这里加入数据处理的函数
	a = str2int(RxBuffer, ' ', 1);
	//b = str2int(RxBuffer, ' ', 2);
	//c = str2int(RxBuffer, ' ', 3);
//	str2double(RxBuffer, ' ', 4, &d);
//	str2double(RxBuffer, ' ', 5, &e);
//	str2double(RxBuffer, ' ', 6, &f);
	//printf("difference: %d\n", a);
	//printf("%d %d %d\n", a, b, c);
	difference = a;
}



//到USARTx_IRQHandler中添加，如:
//void USART1_IRQHandler(void)
//{
//  /* USER CODE BEGIN USART1_IRQn 0 */
//  if(__HAL_UART_GET_FLAG(&USB_Huart,UART_FLAG_IDLE))
//  {
//	  HAL_UART_IdleCallback(&USB_Huart);
//  }
//
//  /* USER CODE END USART1_IRQn 0 */
//  HAL_UART_IRQHandler(&huartx);
//}
void HAL_UART_IdleCallback(UART_HandleTypeDef *huart)
{
	__HAL_UART_CLEAR_IDLEFLAG(huart);
	{
		HAL_UART_DMAStop(huart);

        ProcessData();

        StartUartRxDMA();
	}
}

void ProcessData()
{
    uint32_t len = 0;

    //得到已经接收了多少个字节 = 总共要接收的字节数 - ?NDTR F1为CNDTR F4为NDTR
    #ifdef __STM32F1xx_HAL_H
    	len = UART_RX_BUF_SIZE - USB_Huart.hdmarx->Instance->CNDTR;
    	#define ProcessDataOK
    #endif

    #ifdef  __STM32F4xx_HAL_H
        len = UART_RX_BUF_SIZE - USB_Huart.hdmarx->Instance->NDTR;
    	#define ProcessDataOK
    #endif

    #ifndef ProcessDataOK
    	增加所用芯片的版本
    #endif

    if(len > 0)
    {
        if(sendCompleteSign == 1)
        {
#if UART_RXTX_Switch
            memset((void *)TxBuffer, 0, sizeof(TxBuffer));
            memcpy(TxBuffer, RxBuffer, len);
            TxLen = len;
            StartUartTxDMA();	//串口回显
#endif
            {
            	//在这里面加入数据处理的函数
            	DataProcess();
            }
        }
    }
}

void USB_DMA_printf(const char *format,...)
{
	uint32_t length;
	va_list args;

	va_start(args, format);
	length = vsnprintf((char*)TxBuffer, sizeof(TxBuffer)+1, (char*)format, args);
	va_end(args);

	HAL_UART_Transmit_DMA(&USB_Huart,TxBuffer,length);
}

void USB_printf(const char *format,...)
{
	uint32_t length;
	va_list args;

	va_start(args, format);
	length = vsnprintf((char*)TxBuffer, sizeof(TxBuffer)+1, (char*)format, args);
	va_end(args);

	HAL_UART_Transmit(&USB_Huart,TxBuffer,length,0xFFFF);
}




/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
//  UNUSED(huart);
    if(huart == &USB_Huart)
    {
        sendCompleteSign = 1;
    }
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
	//  UNUSED(huart);
	
	
	if(huart == &USB_Huart)
	{
		ProcessData();

		StartUartRxDMA();
	}
	
	
	
	
	
	
	
	
	
	
	/* 激光测距串口中断 */
	if(huart -> Instance == huart2.Instance){
		//printf("a\n");
		//printf("%x %x %x %x %x %x %x \n", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6]);
		printf("%d \n", temp[3]*16*16 + temp[4]);
		distance = temp[3]*16*16 + temp[4];
		HAL_UART_Receive_IT(&huart2, temp, 7);
		
	}
	
	
	
	
	
	
	
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
  */
}



uint8_t UartTxData(UART_HandleTypeDef *huart, uint8_t *buf, const uint32_t len)
{
	HAL_StatusTypeDef status;
	uint8_t ret = 1;

	if(sendCompleteSign == 0 || len == 0)
	{
		return 0;
	}

    sendCompleteSign = 0;

	status = HAL_UART_Transmit_DMA(huart, (uint8_t*)buf, len);

	if(HAL_OK != status)
	{
		ret = 0;
	}

	return ret;
}

//启动DMA发送
uint8_t StartUartTxDMA()
{
    return UartTxData(&USB_Huart, TxBuffer, TxLen);
}

uint8_t UartRxData(UART_HandleTypeDef *huart, uint8_t *buf, const uint32_t len)
{
	HAL_StatusTypeDef status;
	uint8_t ret = 1;

	status = HAL_UART_Receive_DMA(huart, (uint8_t*)buf, len);
	if(HAL_OK != status)
	{
		ret = 0;
	}
	else
	{
		/* 开启空闲接收中断 */
	    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
	}

	return ret;
}

//启动DMA接收
uint8_t StartUartRxDMA()
{
    return UartRxData(&USB_Huart, RxBuffer, UART_RX_BUF_SIZE);
}
