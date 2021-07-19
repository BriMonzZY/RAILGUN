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
	yaw
	tim11 ch1    PF7
	
	pitch
	tim10 ch1    PF6
	
	
	pitch
	tim13 ch1    PF8
*/
		
		

#define _MAIN_C

#include "main.h"

/* *************************全局变量定义****************************** */

uint8_t temp_usart3;
enum estaSystem staSystem = INIT;		/* 设置初始状态staSystem为INIT */

PID 		sPID;  /* 定义PID结构体变量 */
PID			sPID_pitch;  /*  pitch PID  */
STACK 	distance_stack;

u16 cnt = 0; /* 触摸屏计数 */

long long manual_distance = 0;
long long manual_angle = 0;
int tim_angle = 0;

/* unsigned char CMD_5[8]={0x50,0x03,0x00,0x34,0x00,0x01,0xc8,0x45}; */
unsigned char temp[64];  /* uart2接收数据 */
int distance = 0;  /* 实时接收的距离数据 */
int fire_distance = 0;  /* 发射时候的距离值 */
int difference = 666;  /* k210传回的目标与中心的差值 */

__IO u8 pitch_Reach_flag = 0;  /* 加上volatile修饰防止编译器的优化使得程序卡死在循环中 */
__IO u16 track_flag = 0;  /* AUTO1追踪模式标志位 */
__IO u16 follow_flag = 0;  /* 跟随模式标志位 */
__IO u8 fire_flag = 0;  /* 发射完成标志位 */

u16 yaw_angle_now = 30; /* 当前的yaw值 */
u16 pitch_angle_now = 0;  /* 当前的pitch值 */
int pitch_anle_icm20602 = 0;


/* MPU6050数据 */
float pitch,roll,yaw; 			/* 欧拉角 */
short aacx,aacy,aacz;				/* 加速度传感器原始数据 */
short gyrox,gyroy,gyroz;		/* 陀螺仪原始数据 */
short temperature;					/* 温度 */



/* 为PID变量申请内存，范围指向pp的指针 */
void PIDInit (PID *pp)   
{
	memset(pp, 0, sizeof(PID)); /* 字节的内容全部设置为ch指定的ASCII值，块的大小由第三个参数指定 */
}

/* 为栈分配地址 */
void STACKInit(STACK *stack)   
{
	memset(stack, 0, sizeof(STACK));
}



/* ******************************主函数********************************* */
int main(void)
{
	/* 变量定义区域 */
	
	/* *************************初始化区域******************************* */
	
	HAL_Init(); 									/* 初始化HAL库 */
	Stm32_Clock_Init(360,25,2,8); /* 设置时钟,180Mhz */
	printf("clock\n");
	
	HAL_Delay(1000);
	
	//delay_init(180); 							/* 初始化延时函数 */
	MX_GPIO_Init(); 							/* GPIO初始化 */
	MX_DMA_Init(); 								/* MX DMA 初始化 */
  MX_USART1_UART_Init(); 				/* 串口1初始化 与PC通信 */
  MX_USART3_UART_Init(); 				/* 串口3初始化 与K210通信 */
	MX_USART6_UART_Init();				/* 串口6初始化 激光测距 */
	StartUartRxDMA(); 						/* 使能uartDMA */
	HAL_UART_Receive_IT(&huart3, &temp_usart3, 1); 	/* 打开uart中断 */
	HAL_UART_Receive_IT(&huart6, temp, 64); 
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
	BEEP_Init();									/* 蜂鸣器初始化 */
	printf("beep init\n");
	MX_SPI1_Init();								/* 硬件spi初始化 */
	icm20602_init_spi();					/* icm20602初始化 */
	MX_TIM2_Init();								/* 定时器初始化 */
	MX_TIM10_Init();
  MX_TIM11_Init();
	MX_TIM13_Init();
	
	
	
	HAL_TIM_PWM_Start(&htim10,TIM_CHANNEL_1);						/* 使能TIMPWM */
	HAL_TIM_PWM_Start(&htim11,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 78);  /* 设定初始占空比 */
	__HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, 76);
	__HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, 78);
	
	
	/* 初始化MPU6050 */
	delay_init(180);
	printf("mpu6050\n");
	while(MPU_Init());	
	
	#if 0
	while(mpu_dmp_init()){
		printf("MPU6050 Error!!!\r\n");
		delay_ms(500);
	}
	#endif
	
	printf("MPU6050 OK\r\n");
	
	PIDInit(&sPID);   /* 初始化PID结构体申请内存 */
	sPID.Kp = 0.035;
	sPID.Ki = 0.001;                  
	sPID.Kd = 0.04;
	sPID.SetPoint = 37.0;
	
	PIDInit(&sPID_pitch);   /* 初始化PID结构体申请内存 */
	sPID_pitch.Kp = 0.025;  //0.025
	sPID_pitch.Ki = 0.0005;                  
	sPID_pitch.Kd = 0.09;
	sPID_pitch.SetPoint = 0;  /* pitch初始期望角度为0 */
	
	STACKInit(&distance_stack);
	distance_stack.stack_top = 0;  /* 栈顶索引为0 */
	
	/* 给pitch舵机供电 */
	HAL_Delay(500);
	HAL_GPIO_WritePin(pitch_relay_GPIO_Port,pitch_relay_Pin,GPIO_PIN_RESET);
	
	
	
	HAL_TIM_Base_Start_IT(&htim2);  										/* 使能定时器中断 */
	
	
	
	
	
	BEEP_ONCE();
	printf("init finished\n\n");
	
	#if 0
	LCD_Clear(BLACK);
	u32 color;
	color=LCD_ReadPoint(10, 10);
	printf("\n%d\n", color);
	
	while(1);
	#endif
	
	
	/* **********************while(1)*************************** */
	while(1) {
		
		#if 0/* 使用DMP和定时器中断冲突 */
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{
			printf("%f %f %f\n", pitch, roll, yaw);
			temperature=MPU_Get_Temperature();	//得到温度值
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
		}
		#endif
		
		
		
		
		
//		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
//		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
//		
//		printf("%d %d %d\n", aacx, aacy, aacz);
//		printf("%d %d %d\n\n", gyrox, gyroy, gyroz);
		
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
		else if(staSystem == FOLLOW) {
			railgun_FOLLOW_GUI_INIT();
		}
	}/* the end of while(1) */		    
}


