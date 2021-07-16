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
#include "stack.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include <stdio.h>
#include <string.h>
#include <math.h>






enum estaSystem {  //枚举系统状态
  INIT,MANUAL,AUTO1,AUTO2,FOLLOW, DEVELOPER
};

typedef struct PID{
	double SetPoint;            //设定值
	double Kp;                  //比例系数
	double Ki;                  //积分系数
	double Kd;                  //微分系数
	double LastError;           //最后一次误差数Er[-1]
	double PrevError;           //最后第二次误差数er[-2]
	double SumError;            //误差积分  
}PID;


typedef struct STACK{
	unsigned char stack[100];
	unsigned int stack_top;
}STACK;



#ifndef _MAIN_C
extern enum estaSystem staSystem;//staSystem当前系统状态
extern long long manual_distance;
extern long long manual_angle;
extern int difference;
extern int tim_angle;
extern u16 track_flag;
extern PID sPID;
extern STACK 	distance_stack;
extern u16 yaw_angle_now;
extern u16 pitch_angle_now;
extern int distance;
extern int fire_distance;
extern u8 fire_flag;
extern u16 follow_flag;

extern int pitch_expect;  /* pitch轴角度期望值 */
extern int pitch_anle_icm20602;

extern unsigned char temp[64];  /* 超声波测距 */
/* extern unsigned char CMD_5[8]; */

extern float pitch,roll,yaw; 		//欧拉角
extern short aacx,aacy,aacz;		//加速度传感器原始数据
extern short gyrox,gyroy,gyroz;	//陀螺仪原始数据
extern short temperature;					//温度


#endif

/* Exported types ------------------------------------------------------------*/

#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOC

#define Discharge_relay_Pin GPIO_PIN_2
#define Discharge_relay_GPIO_Port GPIOE
#define Charging_relay_Pin GPIO_PIN_3
#define Charging_relay_GPIO_Port GPIOE
#define pitch_relay_Pin GPIO_PIN_4
#define pitch_relay_GPIO_Port GPIOE

#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOH
#define KEY1_EXTI_IRQn EXTI2_IRQn
#define KEY0_Pin GPIO_PIN_3
#define KEY0_GPIO_Port GPIOH
#define KEY0_EXTI_IRQn EXTI3_IRQn

#define MPU_SCL_Pin GPIO_PIN_5
#define MPU_SCL_GPIO_Port GPIOE
#define MPU_SDA_Pin GPIO_PIN_9
#define MPU_SDA_GPIO_Port GPIOF
#define MPU6050_INT_EXTI_IRQn EXTI9_5_IRQn
#define MPU6050_INT_Pin GPIO_PIN_6
#define MPU6050_INT_GPIO_Port GPIOE

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
