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

/* *************************ȫ�ֱ�������****************************** */

uint8_t temp_usart3;
enum estaSystem staSystem = INIT;		/* ���ó�ʼ״̬staSystemΪINIT */

PID 		sPID;  /* ����PID�ṹ����� */
STACK 	distance_stack;

u16 cnt = 0; /* ���������� */

long long manual_distance = 0;
long long manual_angle = 0;
int tim_angle = 0;

/* unsigned char CMD_5[8]={0x50,0x03,0x00,0x34,0x00,0x01,0xc8,0x45}; */
unsigned char temp[64];  /* uart2�������� */
int distance = 0;  /* ʵʱ���յľ������� */
int fire_distance = 0;  /* ����ʱ��ľ���ֵ */
int difference = 666;  /* k210���ص�Ŀ�������ĵĲ�ֵ */
u16 track_flag = 0;  /* AUTO1׷��ģʽ��־λ */
u16 yaw_angle_now = 30; /* ��ǰ��yawֵ */
u16 pitch_angle_now = 0;  /* ��ǰ��pitchֵ */
u16 follow_flag = 0;  /* ����ģʽ��־λ */
u8 fire_flag = 0;  /* ������ɱ�־λ */



/* ΪPID���������ڴ棬��Χָ��pp��ָ�� */
void PIDInit (PID *pp)   
{
	memset(pp, 0, sizeof(PID)); /* �ֽڵ�����ȫ������Ϊchָ����ASCIIֵ����Ĵ�С�ɵ���������ָ�� */
}

/* Ϊջ�����ַ */
void STACKInit(STACK *stack)   
{
	memset(stack, 0, sizeof(STACK));
}



/* ******************************������********************************* */
int main(void)
{
	/* ������������ */
	
	/* *************************��ʼ������******************************* */
	
	HAL_Init(); 									/* ��ʼ��HAL�� */
	Stm32_Clock_Init(360,25,2,8); /* ����ʱ��,180Mhz */
	delay_init(180); 							/* ��ʼ����ʱ���� */
	MX_GPIO_Init(); 							/* GPIO��ʼ�� */
	MX_DMA_Init(); 								/* MX DMA ��ʼ�� */
  MX_USART1_UART_Init(); 				/* ����1��ʼ�� ��PCͨ�� */
  MX_USART3_UART_Init(); 				/* ����3��ʼ�� ��K210ͨ�� */
	MX_USART6_UART_Init();				/* ����6��ʼ�� ������ */
	StartUartRxDMA(); 						/* ʹ��uartDMA */
	HAL_UART_Receive_IT(&huart3, &temp_usart3, 1); 	/* ��uart�ж� */
	HAL_UART_Receive_IT(&huart6, temp, 64); 
	printf("main\n");
	LED_Init(); 									/* ��ʼ��LED */
	printf("led\n");
	KEY_Init(); 									/* ��ʼ������ */
	printf("key\n");
	SDRAM_Init();								 	/* ��ʼ��SDRAM */
	printf("sdram\n");
	LCD_Init(); 									/* ��ʼ��LCD */
	printf("lcd\n");
	tp_dev.init(); 								/* ��������ʼ�� */
	BEEP_Init();									/* ��������ʼ�� */
	printf("beep init\n");
	MX_SPI1_Init();								/* Ӳ��spi��ʼ�� */
	icm20602_init_spi();					/* icm20602��ʼ�� */
	MX_TIM2_Init();								/* ��ʱ����ʼ�� */
	MX_TIM10_Init();
  MX_TIM11_Init();
	MX_TIM13_Init();
	HAL_TIM_Base_Start_IT(&htim2);  										/* ʹ�ܶ�ʱ���ж� */
	
	
	HAL_TIM_PWM_Start(&htim10,TIM_CHANNEL_1);						/* ʹ��TIMPWM */
	HAL_TIM_PWM_Start(&htim11,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 78);  /* �趨��ʼռ�ձ� */
	__HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, 76);
	__HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, 78);
	
	
	PIDInit(&sPID);   /* ��ʼ��PID�ṹ�������ڴ� */
	sPID.Kp = 0.035;
	sPID.Ki = 0.001;                  
	sPID.Kd = 0.04;
	sPID.SetPoint = 37.0;
	
	STACKInit(&distance_stack);
	distance_stack.stack_top = 0;  /* ջ������Ϊ0 */
	
	HAL_Delay(500);
	HAL_GPIO_WritePin(pitch_relay_GPIO_Port,pitch_relay_Pin,GPIO_PIN_RESET);
	
	BEEP_ONCE();
	printf("init finished\n\n");
	
	
	/* **********************while(1)*************************** */
	while(1) {
		
		/* ������������ */
		tp_dev.scan(0);											/* ɨ�败���� */
		if(tp_dev.sta & TP_PRES_DOWN){			/* ������������ */
			if(tp_dev.sta & TP_PRES_DOWN){		/* ������������ */
				railgun_button();								/* ���������¶��� */
				cnt++;
				delay_ms(50);
			}
		} /* the end of if(tp_dev.sta&TP_PRES_DOWN) */
		if(!(tp_dev.sta&TP_PRES_DOWN)) {   	/* ���������ɿ� */ 
			if(!(tp_dev.sta&TP_PRES_DOWN)){		/* ���������ɿ� */
				cnt = 0;
			}
		}
	
		/* ״̬���߼��ж� */
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


