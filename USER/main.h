/**
  ******************************************************************************
  * @file    Templates/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.2.2
  * @date    25-May-2015
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"

#include "sys.h"
#include "usart.h"
#include "gpio.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "touch.h"
#include "screen_app.h"
#include "dma.h"
#include "uart_dma.h"
#include "beep.h"
#include "spi.h"
#include "icm20602.h"
#include "attitude_solution.h"
#include "status_transmit.h"
#include "gun_action.h"
#include "actuator.h"
#include "tim.h"
#include "senser.h"
#include <stdio.h>
#include <string.h>






enum estaSystem {  //ö��ϵͳ״̬
  INIT,MANUAL,AUTO1,AUTO2
};

typedef struct{
	double SetPoint;            //�趨ֵ
	double Kp;                  //����ϵ��
	double Ki;                  //����ϵ��
	double Kd;                  //΢��ϵ��
	double LastError;           //���һ�������Er[-1]
	double PrevError;           //���ڶ��������er[-2]
	double SumError;            //������  
}PID;




#ifndef _MAIN_C
extern enum estaSystem staSystem;//staSystem��ǰϵͳ״̬
extern long long manual_distance;
extern long long manual_angle;
extern int difference;
extern int tim_angle;
extern u16 track_flag;
extern PID sPID;
extern u16 yaw_angle_now;
extern u16 pitch_angle_now;
extern int distance;
extern u8 fire_flag;

extern eulerianAngles_t eulerAngle;  /* ŷ���ǽṹ�� */
extern unsigned char temp[7];
extern unsigned char CMD_5[8];

#endif

/* Exported types ------------------------------------------------------------*/

#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOC

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/