/**
	USART3 GPIO Configuration K210
	PB10     ------> USART3_TX
	PB11     ------> USART3_RX
*/

/**
	USART1 GPIO Configuration PC 
	PA9     ------> USART1_TX
	PA10    ------> USART1_RX
*/

/**
	偏航角
	tim11 ch1    PF7
	
	俯仰角
	tim10 ch1    PF6

*/
		
		

#define _MAIN_C

#include "main.h"

/* 全局变量定义 */

uint8_t temp_usart3;
enum estaSystem staSystem = INIT;		/* 设置初始状态staSystem为INIT */



PID sPID;  /* 定义PID结构体变量 */



u16 cnt = 0; /* 触摸屏计数 */

long long manual_distance = 0;
long long manual_angle = 0;
int tim_angle = 0;

unsigned char CMD_5[8]={0x50,0x03,0x00,0x34,0x00,0x01,0xc8,0x45};  /*  */
unsigned char temp[7];  /* uart2接收数据 */
int distance = 0;
int difference = 666;  /* k210传回的目标与中心的差值 */
u16 get_distance_flag = 0;
u16 track_flag = 0;

u16 yaw_angle_now = 30;
u16 pitch_angle_now = 0;

u8 fire_flag = 0;  /* 发射完成标志位 */



/* 为PID变量申请内存，范围指向pp的指针 */
void PIDInit (PID *pp)   
{
	memset(pp, 0, sizeof(PID)); //字节的内容全部设置为ch指定的ASCII值，块的大小由第三个参数指定，
}



/* 主函数 */
int main(void)
{
	/* 变量定义区域 */
	
	/* 初始化区域 */
	
	HAL_Init(); 									/* 初始化HAL库 */
	
	Stm32_Clock_Init(360,25,2,8); /* 设置时钟,180Mhz */
	delay_init(180); 							/* 初始化延时函数 */
	MX_GPIO_Init(); 							/* GPIO初始化 */
	MX_DMA_Init(); 								/* mx dma 初始化 */
  MX_USART1_UART_Init(); 				/* 串口1初始化 与PC通信 */
	MX_USART2_UART_Init();				/* 串口2初始化 */
  MX_USART3_UART_Init(); 				/* 串口3初始化 与K210通信 */
	StartUartRxDMA(); 						/* 使能uartDMA */
	HAL_UART_Receive_IT(&huart3, &temp_usart3, 1); /* 打开uart中断 */
	HAL_UART_Receive_IT(&huart2, temp, 7);  /* 初始化UART2中断 */
	printf("main\n");
	LED_Init(); 									/* 初始化LED */
	printf("led\n");
	KEY_Init(); 									/* 初始化按键 */
	printf("key\n");
	SDRAM_Init();								 	/* 初始化SDRAM */
	printf("sdram\n");
	LCD_Init(); 									/* 初始化LCD */
	printf("lcd\n");
	tp_dev.init(); 								/* 触摸屏初始化 */
	
	BEEP_Init();
	printf("beep init\n");
	
	MX_SPI1_Init();
	icm20602_init_spi();
	
	MX_TIM2_Init();
	MX_TIM10_Init();
  MX_TIM11_Init();
	HAL_TIM_Base_Start_IT(&htim2);   /* 使能定时器中断 */
	HAL_TIM_PWM_Start(&htim10,TIM_CHANNEL_1);		/* 使能TIMPWM */
	HAL_TIM_PWM_Start(&htim11,TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 78);  /* 设定初始占空比 */
	__HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, 76);
	
	
	
	
	PIDInit(&sPID);   /* 初始化结构体申请内存 */

	sPID.Kp = 0.035;
	sPID.Ki = 0.001;                  
	sPID.Kd = 0.04;
	sPID.SetPoint = 0.0;
	
	
	
	
	
	
	BEEP_ONCE();
	
	printf("init finished\n\n");
	
	
	/* while(1) */
	while(1) {
			
		
		
		/* 触摸屏处理部分 */
		tp_dev.scan(0);											/* 扫描触摸屏 */
		if(tp_dev.sta & TP_PRES_DOWN){			/* 触摸屏被按下 */
			if(tp_dev.sta & TP_PRES_DOWN){		/* 触摸屏被按下 */
				railgun_button();								/* 触摸屏按下动作 */
				cnt++;
				delay_ms(50);
			}
		} /* the end of if(tp_dev.sta&TP_PRES_DOWN) */
		if(!(tp_dev.sta&TP_PRES_DOWN)) {   	/* 触摸屏被松开 */ 
			if(!(tp_dev.sta&TP_PRES_DOWN)){		/* 触摸屏被松开 */
				cnt = 0;
			}
		}
	
		/* 状态机逻辑判断 */
		if(staSystem == INIT){
			railgun_INIT_GUI_INIT();
		}
		else if(staSystem == MANUAL){
			railgun_MANUAL_GUI_INIT();
		}
		else if(staSystem == AUTO1){
			railgun_AUTO1_GUI_INIT();
		}
		else if(staSystem == AUTO2){
			railgun_AUTO2_GUI_INIT();
		}
	}  /* the end of while(1) */		    
}


