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

u8 distance_aver_cnt = 0;
int distance_aver[4] = {0};  /* 从最近测得的4组距离值取平均作为发射距离值 */


int stack_push(STACK *stack, unsigned char data);
int stack_read_top(STACK *stack);
int stack_pop(STACK *stack);



/* 数据处理部分 */
/* 以空格作为分隔符，RxBuffer是串口接收缓冲数组，读到的都在这里 */
int32_t a,b,c;
double  d,e,f;
void DataProcess(void)
{
	/* 在这里加入数据处理的函数 */
	a = str2int(RxBuffer, ' ', 1);
	/* b = str2int(RxBuffer, ' ', 2); */
	/* str2double(RxBuffer, ' ', 4, &d); */
	difference = a;
}


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
	/* UNUSED(huart); */
	
	unsigned int zero[100] = {0};
	uint16_t distance_tmp = 0;
	unsigned int distance_tmp_cnt = 0;
	
	
	/* k210回传数据中断 */
	if(huart == &USB_Huart)
	{
		ProcessData();
		StartUartRxDMA();
	}
	
	
	
	#if 0
	/* 激光测距uart2串口中断 */
	if(huart -> Instance == huart2.Instance){
		
		
		
		
		
		printf("%d ", temp);
		
		if(temp == 0x50 && cmd_cnt == 0) {
			recive_queue[0] = temp;
			cmd_cnt++;
		}
		if(cmd_cnt == 1) {
			recive_queue[1] = temp;
			cmd_cnt++;	
		}
		if(cmd_cnt == 2) {
			recive_queue[2] = temp;
			cmd_cnt++;
		}
		if(recive_queue[0] == 0x50 && recive_queue[1] == 0x03 && recive_queue[2] == 0x02 && cmd_cnt == 3) {
			recive_queue[cmd_cnt++] = temp;
		}
		else {
			cmd_cnt = 0;
			recive_queue[0] = recive_queue[1] = recive_queue[2] = 0;
		}
		if(cmd_cnt >= 4 && cmd_cnt <= 6) {
			recive_queue[cmd_cnt++] = temp;
		}
		if(cmd_cnt == 7) {
			cmd_cnt = 0;
			recive_queue[0] = recive_queue[1] = recive_queue[2] = recive_queue[3] = recive_queue[4] = recive_queue[5] = recive_queue[6] = 0;
			distance = recive_queue[3]*16*16 + recive_queue[4];
			printf("%d\n", distance);
		}
		HAL_UART_Receive_IT(&huart2, &temp, 1);
		
			
	}
	#endif
	
	
	/* *************************************激光测距uart6串口中断********************************* */
	if(huart -> Instance == huart6.Instance){
		u8 i = 0;
		
		/*
		for(i = 0; i < 64; i++) {
			printf("%d ", temp[i]);
		}
		*/
		
		/* 数据解算 */
		for(i = 0; i < 64; i++) {
			/* 检测到State开始将数据压入栈中 */
			if(temp[i] == 83 && temp[i+1] == 116 && temp[i+2] == 97 && temp[i+3] == 116 && temp[i+4] == 101) {
				stack_push(&distance_stack, temp[i]);
				stack_push(&distance_stack, temp[i+1]);
				stack_push(&distance_stack, temp[i+2]);
				stack_push(&distance_stack, temp[i+3]);
				stack_push(&distance_stack, temp[i+4]);
			}
			
			if(distance_stack.stack[0] == 83 && distance_stack.stack[1] == 116 && temp[i] != 109 && temp[i+1] != 109) {
				stack_push(&distance_stack, temp[i]);
			}
			/* 数据完整 开始解算 */
			if(distance_stack.stack[0] == 83 && distance_stack.stack[1] == 116 && temp[i] == 109 && temp[i+1] == 109) {
				stack_push(&distance_stack, temp[i]);
				stack_push(&distance_stack, temp[i+1]);
				
				stack_pop(&distance_stack);
				stack_pop(&distance_stack);
				stack_pop(&distance_stack);
			
				while(distance_stack.stack[distance_stack.stack_top] != 32) {
					//printf("%d ", distance_stack.stack[distance_stack.stack_top]-48);
					distance_tmp = distance_tmp + (distance_stack.stack[distance_stack.stack_top]-48)*pow(10, distance_tmp_cnt++);
					stack_pop(&distance_stack);
				}
				/* 数据读取完毕 */
				if(distance_stack.stack[distance_stack.stack_top] == 32) {
					memcpy(distance_stack.stack, zero, 100);
					distance_stack.stack_top = 0;
					distance_tmp_cnt = 0;
					distance = distance_tmp;
				}
				
			}
			if(distance_stack.stack[0] == 83 && distance_stack.stack[1] == 116 && i == 63) {  /* 没有结尾数据不完整 */
				memcpy(distance_stack.stack, zero, 100);
				distance_stack.stack_top = 0;
			}
			
		}
		
		/* 记录最近四次测量值  简易滑动平均滤波 */
		distance_aver[distance_aver_cnt++] = distance;
		if(distance_aver_cnt == 4) {
			distance_aver_cnt = 0;
		}
		
		//printf("%d\n", distance);
		
		HAL_UART_Receive_IT(&huart6, temp, 64);  /* 复位uart中断 */  
		
		
	}
	
	
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
