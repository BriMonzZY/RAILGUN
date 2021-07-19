#include "actuator.h"



/* �������弰���� */

extern u16 get_distance_flag;


/* 1ms��һ���ж� */
uint16_t tim_cnt_50 = 0;
uint16_t tim_cnt_10 = 0;
uint16_t tim_cnt_40 = 0;
uint16_t tim_cnt_5 = 0;

uint8_t xiangzuo = 1;   /* AUTO2ģʽ������ת��־λ */
uint8_t angle_flag = 0;  /* Ŀ�����ڵĽǶ� */


int judge_difference[10] = {0};  /* �����ж��Ƿ�ﵽ��׼ */
uint8_t judge_cnt = 0;

int judge_difference_pitch[15] = {0};  /* �����ж��Ƿ�ﵽ��׼ */
uint8_t judge_pitch_cnt = 0;


/* λ��ʽPID */
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
		tim_cnt_10++;
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
		
		/* ˢ��uart6״̬  pitch�ջ� */
		if(tim_cnt_10 >= 10) {
			tim_cnt_10 = 0;
			/* printf("%d \n", distance); */
			HAL_UART_Receive_IT(&huart6, temp, 64); 
			
			
			
			
			get_icm20602_accdata_spi();
			get_icm20602_gyro_spi();
			IMU_quaterToEulerianAngles();  /* ��ȡŷ���� */
			//printf("%.2f,%.2f,%.2f\n", eulerAngle.pitch, eulerAngle.roll, eulerAngle.yaw);
			if(eulerAngle.roll <=180 && eulerAngle.roll >= 0) {
				pitch_anle_icm20602 = 180-(int)eulerAngle.roll;
			}
			else if(eulerAngle.roll < 0 && eulerAngle.roll >= -180) {
				pitch_anle_icm20602 = -((int)eulerAngle.roll+180);
			}
			//printf("%d\n", pitch_anle_icm20602);
				
			
			
			if(pitch_Reach_flag == 0) {	
				pitch_angle_now += PIDCalc(&sPID_pitch, pitch_anle_icm20602);
				Pitch_Angle(pitch_angle_now);
			}	
			
				
				
				
			/* ���15�β�ֵ����4���ھ�ֹͣ */
			judge_difference_pitch[judge_pitch_cnt++] = pitch_anle_icm20602;
			if(judge_pitch_cnt == 15) judge_pitch_cnt = 0;
			for(i = 0; i < 15; i++) {
				if(!(judge_difference_pitch[i] >= sPID_pitch.SetPoint-4 && judge_difference_pitch[i] <= sPID_pitch.SetPoint+4)) break;
				if(i == 14) pitch_Reach_flag = 1;
			}
				
			
			
			
			
			
			
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
		
		/* ����ģʽ MPU6050 */
		if(tim_cnt_5 >= 5) {
			tim_cnt_5 = 0;
			if(follow_flag == 1) {
				
				#if 1
					MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
					MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
					IMU_quaterToEulerianAngles_mpu6050();  /* ��ȡŷ���� */
					printf("%.2f,%.2f,%.2f\n", eulerAngle_mpu6050.pitch, eulerAngle_mpu6050.roll, eulerAngle_mpu6050.yaw);
				#endif
				
				
				if(eulerAngle_mpu6050.yaw >= -30 && eulerAngle_mpu6050.yaw <= 30) {
					yaw_angle_now = eulerAngle_mpu6050.yaw;
					Yaw_Angle(eulerAngle_mpu6050.yaw + 30);
				}
				if(-eulerAngle_mpu6050.pitch >= 0 && -eulerAngle_mpu6050.pitch <= 50) {
					Pitch_Angle(-eulerAngle_mpu6050.pitch);
				}
				
				
			}
			
		}
		
		
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{ 
	if(GPIO_Pin == KEY0_Pin) {  /* ��� */
		delay_ms(50);
		if(HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == 0) {
			HAL_GPIO_TogglePin(Charging_relay_GPIO_Port, Charging_relay_Pin);
		}
		
	}
	
	if(GPIO_Pin == KEY1_Pin) {  /* �ŵ� */
		delay_ms(50);
		if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == 0) {
			HAL_GPIO_TogglePin(Discharge_relay_GPIO_Port, Discharge_relay_Pin);
		}
	}
	
	#if 0
	if(GPIO_Pin == MPU6050_INT_Pin) { 
		printf("6050 exti\n");
		mpu_dmp_get_data(&pitch,&roll,&yaw);										 //===�õ�ŷ���ǣ���̬�ǣ�������
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);								 //===�õ�����������
		
		printf("%f %f %f \n", pitch, roll, yaw);
	}
	#endif
	
}



/* �ڵ����亯�� */
void fire(void)
{
	/* ��λ���� */
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
	
	HAL_Delay(3500); /* ���ʱ�� */
	
	/* �ŵ� */
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
	HAL_Delay(500);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_SET);
	
	HAL_Delay(200);
	
	/* ��λ */
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
	
	fire_flag = 1;
	
}




