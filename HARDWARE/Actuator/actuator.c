#include "actuator.h"



/* �������弰���� */

extern u16 get_distance_flag;


uint16_t tim_cnt_50 = 0;
uint16_t tim_cnt_45 = 0;
uint16_t tim_cnt_40 = 0;

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
		tim_cnt_45++;
		tim_cnt_40++;
		
		/* auto2ɨ�� */
		if(tim_cnt_50 >= 50){
			tim_cnt_50 = 0;
			
			if(staSystem == AUTO2 && xiangzuo == 1) {
				tim_angle+=2;
				yaw_angle_now+=2;
				Yaw_Angle(tim_angle);
				if(tim_angle == 60) xiangzuo = 0;
				
				if(difference >= -6 && difference <= 6 && xiangzuo == 1){
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
					printf("** %d %d \n", angle_flag, yaw_angle_now);
					fire();
					tim_angle-=2;
					yaw_angle_now-=2;
					Yaw_Angle(tim_angle);
				}
				
			}
			
		}
		
		/* uart6��λ */
		if(tim_cnt_45 >= 100) {
			
			/* DEBUG */
			get_icm20602_accdata_spi();
			get_icm20602_gyro_spi();
			IMU_quaterToEulerianAngles();  /* ��ȡŷ���� */
			printf("%.2f,%.2f,%.2f\n", eulerAngle.pitch, eulerAngle.roll, eulerAngle.yaw);
			
			
			HAL_UART_Receive_IT(&huart6, temp, 64);  /* ��ʱ��λuart6 */
		}
		
		/* auto1׷�� */
		if(tim_cnt_40 >= 40 && track_flag == 1) {
			printf("difference: %d\n", difference);
			tim_cnt_40 = 0;
			if(staSystem == AUTO1 && difference != 666) {
				
				/* PID */
				yaw_angle_now += PIDCalc(&sPID, difference);
				Yaw_Angle(yaw_angle_now);
				
				/* ���10�β�ֵ����6���ھ�ֹͣѰ�ң�Ȼ���෢�� */
				judge_difference[judge_cnt++] = difference;
				if(judge_cnt == 10) judge_cnt = 0;
				for(i = 0; i < 10; i++) {
					if(!(judge_difference[i] >= -6 && judge_difference[i] <= 6)) break;
					if(i == 9) track_flag = 0;
				}
			}
		}
		
		
	}
}



/* �ڵ����亯�� */
void fire(void)
{
	fire_flag = 0;
}




