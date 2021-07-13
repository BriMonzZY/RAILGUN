#include "actuator.h"



/* �������弰���� */

extern u16 get_distance_flag;


/* 1ms��һ���ж� */
uint16_t tim_cnt_50 = 0;
uint16_t tim_cnt_100 = 0;
uint16_t tim_cnt_40 = 0;
uint16_t tim_cnt_5 = 0;

uint8_t xiangzuo = 1;   /* AUTO2ģʽ������ת��־λ */
uint8_t angle_flag = 0;  /* Ŀ�����ڵĽǶ� */


int judge_difference[10] = {0};  /* �����ж��Ƿ�ﵽ��׼ */

uint8_t judge_cnt = 0;


/* PID */
double PIDCalc(PID *pp, double NextPoint)   
{
	
	if(yaw_angle_now >= 60 || yaw_angle_now <= 0) pp->SumError = 0;
	
	double dError,                              /* ��ǰ΢�� */
				 Error;                               /* ƫ�� */
	Error = pp->SetPoint - NextPoint;           /* ƫ��ֵ=�趨ֵ-����ֵ����ǰֵ��*/
	pp->SumError += Error;                      /* ����=����+ƫ��  --ƫ����ۼ� */
	dError = pp->LastError - pp->PrevError;     /* ��ǰ΢�� = ������ - ֮ǰ��� */
	pp->PrevError = pp->LastError;              /* ���¡�֮ǰ�� */
	pp->LastError = Error;                      /* ���¡������ */
	return (pp->Kp * Error                      /* ������ = �������� * ƫ�� */
			+   pp->Ki *  pp->SumError              /* ������ = ���ֳ��� * ������ */
			+   pp->Kd * dError                     /* ΢���� = ΢�ֳ��� * ��ǰ΢�� */
				 );
}









/* ��ʱ���жϷ����� */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	u8 i;
	if (htim == (&htim2)) {
		
		tim_cnt_50++;
		tim_cnt_100++;
		tim_cnt_40++;
		tim_cnt_5++;
		
		/* auto2ɨ�� */
		if(tim_cnt_50 >= 50){
			tim_cnt_50 = 0;
			
			if(staSystem == AUTO2 && xiangzuo == 1) {
				tim_angle+=2;
				yaw_angle_now+=2;
				Yaw_Angle(tim_angle);
				if(tim_angle == 60) xiangzuo = 0;
				
				if(difference >= sPID.SetPoint-6 && difference <= sPID.SetPoint+6 && xiangzuo == 1){
					angle_flag = tim_angle;
					fire_distance = distance;
					
					printf("**************************%d  %d  %d********************************* \n", angle_flag, fire_distance, yaw_angle_now);
				}
				
			}
			if(staSystem == AUTO2 && xiangzuo == 0) {
				tim_angle-=2;
				yaw_angle_now-=2;
				Yaw_Angle(tim_angle);
				
				if(yaw_angle_now == angle_flag) {
					
					/* �򿪷ŵ�̵������� */
					HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_SET);
					
					printf("** %d %d \n", angle_flag, yaw_angle_now);
					
					tim_angle-=2;
					yaw_angle_now-=2;
					Yaw_Angle(tim_angle);
				}
				if(tim_angle == 0) {
					HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
					Auto2_To_Init();
					xiangzuo = 1;
				}
			}
			
		}
		
		/* ���� */
		if(tim_cnt_100 >= 100) {
			tim_cnt_100 = 0;
			//printf("%d \n", distance);
			HAL_UART_Receive_IT(&huart6, temp, 64); 
		}
		
		/* auto1׷�� */
		if(tim_cnt_40 >= 40 && track_flag == 1) {
			tim_cnt_40 = 0;
			
			printf("difference: %d\n", difference);
			
			if(staSystem == AUTO1 && difference != 666) {
				
				/* PID */
				yaw_angle_now += PIDCalc(&sPID, difference);
				Yaw_Angle(yaw_angle_now);
				
				/* ���10�β�ֵ����6���ھ�ֹͣѰ�ң�Ȼ���෢�� */
				judge_difference[judge_cnt++] = difference;
				if(judge_cnt == 10) judge_cnt = 0;
				for(i = 0; i < 10; i++) {
					if(!(judge_difference[i] >= sPID.SetPoint-6 && judge_difference[i] <= sPID.SetPoint+6)) break;
					if(i == 9) track_flag = 0;
				}
			}
		}
		
		/* ����ģʽ */
		if(tim_cnt_5 >= 5) {
			tim_cnt_5 = 0;
			if(follow_flag == 1) {
				
				#if 1
					get_icm20602_accdata_spi();
					get_icm20602_gyro_spi();
					IMU_quaterToEulerianAngles();  /* ��ȡŷ���� */
					printf("%.2f,%.2f,%.2f\n", eulerAngle.pitch, eulerAngle.roll, eulerAngle.yaw);
				#endif
				
				
				if(eulerAngle.yaw >= -30 && eulerAngle.yaw <= 30) {
					yaw_angle_now = eulerAngle.yaw;
					Yaw_Angle(eulerAngle.yaw + 30);
				}
				if(-eulerAngle.pitch >= 0 && -eulerAngle.pitch <= 60) {
					Pitch_Angle(-eulerAngle.pitch);
				}
				
				
			}
			
		}
		
		
	}
}



/* �ڵ����亯�� */
void fire(void)
{
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
	
	HAL_Delay(1000); /* ���ʱ�� */
	
	/* �ŵ� */
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
	HAL_Delay(10);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_SET);
	
	HAL_Delay(200);
	
	/* ��λ */
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
	
	fire_flag = 1;
	
}




