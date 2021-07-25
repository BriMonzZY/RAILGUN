#include "actuator.h"



/* 变量定义及声明 */

extern u16 get_distance_flag;


/* 1ms进一次中断 */
uint16_t tim_cnt_50 = 0;
uint16_t tim_cnt_10 = 0;
uint16_t tim_cnt_40 = 0;
uint16_t tim_cnt_5 = 0;

uint8_t xiangzuo = 1;   /* AUTO2模式向左旋转标志位 */
uint8_t angle_flag = 0;  /* 目标所在的角度 */


int judge_difference[10] = {0};  /* 用于判断是否达到标准 */
uint8_t judge_cnt = 0;

int judge_difference_pitch[15] = {0};  /* 用于判断是否达到标准 */
uint8_t judge_pitch_cnt = 0;




int stack_push(STACK *stack, unsigned char data);
int stack_read_top(STACK *stack);
int stack_pop(STACK *stack);




/* 位置式PID */
double PIDCalc(PID *pp, double NextPoint)   
{
	
	if(yaw_angle_now >= 60 || yaw_angle_now <= 0) pp->SumError = 0;
	
	double dError,                              /* 当前微分 */
				 Error;                               /* 偏差 */
	Error = pp->SetPoint - NextPoint;           /* 偏差值=设定值-输入值（当前值）*/
	pp->SumError += Error;                      /* 积分=积分+偏差  --偏差的累加 */
	dError = pp->LastError - pp->PrevError;     /* 当前微分 = 最后误差 - 之前误差 */
	pp->PrevError = pp->LastError;              /* 更新“之前误差” */
	pp->LastError = Error;                      /* 更新“最后误差” */
	return (pp->Kp * Error                      /* 比例项 = 比例常数 * 偏差 */
			+   pp->Ki *  pp->SumError              /* 积分项 = 积分常数 * 误差积分 */
			+   pp->Kd * dError                     /* 微分项 = 微分常数 * 当前微分 */
				 );
}






/* 定时器中断服务函数 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	u8 i;
	if (htim == (&htim2)) {
		
		tim_cnt_50++;
		tim_cnt_10++;
		tim_cnt_40++;
		tim_cnt_5++;
		
		/* *********************************************************auto2扫描************************************************ */
		if(tim_cnt_50 >= 50){
			
			//HAL_UART_Receive_IT(&huart6, temp_uart6, 64);
			
			
			u8 i, min_dif_index;
			float min_dif;  /* 距离和表中的差值的最小值 */
			
			tim_cnt_50 = 0;
			
			/* 向左扫描 */
			if(staSystem == AUTO2 && xiangzuo == 1) {
				/* 开始充电 */
				if(tim_angle == 0) {
					HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
					HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
					delay_ms(30);
					HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
				}
				
				tim_angle+=1;
				yaw_angle_now+=1;
				Yaw_Angle(tim_angle);
				if(tim_angle == 60) xiangzuo = 0;
				
				if(difference >= sPID.SetPoint-6 && difference <= sPID.SetPoint+6 && xiangzuo == 1){
					angle_flag = tim_angle;
					Distance_Process();  /* 获取距离 */
					fire_distance = distance;
					
					fire_distance-=250;
					
					printf("**************************%d  %d  %d********************************* \n", angle_flag, fire_distance, yaw_angle_now);
					
					
					/* 寻找最适合的发射角度和电压 */
					min_dif = abs(angle_distance_48v_45v[17][0]*1000 - fire_distance);
					min_dif_index = 0;
					for(i = 17; i <= 34; i++) {
						if(abs(angle_distance_48v_45v[i][0]*1000 - fire_distance) <= min_dif) {
							min_dif_index = i;
							min_dif = abs(angle_distance_48v_45v[i][0]*1000 - fire_distance);
						}
					}
					printf("distance: %f  angle: %f  voltage: %f\n", angle_distance_48v_45v[min_dif_index][0], angle_distance_48v_45v[min_dif_index][1], angle_distance_48v_45v[min_dif_index][2]);
				}
				
				
			}
			/* 向右扫描 */
			if(staSystem == AUTO2 && xiangzuo == 0) {
				/* 停止充电 */
				if(tim_angle == 60) {
					Pitch_Angle(angle_distance_48v_45v[min_dif_index][1]);
					HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
					HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
				}
				
				tim_angle-=1;
				yaw_angle_now-=1;
				Yaw_Angle(tim_angle);
				
				if(yaw_angle_now == angle_flag-8) {
					
					/* 打开放电继电器发射 */
					HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
					HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_SET);
					delay_ms(50);
					printf("** %d %d \n", angle_flag, yaw_angle_now);
					
					tim_angle-=1;
					yaw_angle_now-=1;
					Yaw_Angle(tim_angle);
				}
				if(tim_angle <= 0) {
					HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
					Auto2_To_Init();
					xiangzuo = 1;
				}
			}
		}
		
		
		
		
		/* *********************************************10ms***************************************** */
		if(tim_cnt_10 >= 10) {
			tim_cnt_10 = 0;
			
			//Distance_Process();
			//printf("%d \n", distance);
			
			
			/* Pitch轴闭环控制 */
			#if 0
				get_icm20602_accdata_spi();
				get_icm20602_gyro_spi();
				IMU_quaterToEulerianAngles();  /* 获取欧拉角 */
				/* printf("%.2f,%.2f,%.2f\n", eulerAngle.pitch, eulerAngle.roll, eulerAngle.yaw); */
				if(eulerAngle.roll <=180 && eulerAngle.roll >= 0) {
					pitch_anle_icm20602 = 180-(int)eulerAngle.roll;
				}
				else if(eulerAngle.roll < 0 && eulerAngle.roll >= -180) {
					pitch_anle_icm20602 = -((int)eulerAngle.roll+180);
				}
				printf("%d\n", pitch_anle_icm20602);
				
				
				if(pitch_Reach_flag == 0) {	
					pitch_angle_now += PIDCalc(&sPID_pitch, pitch_anle_icm20602);
					Pitch_Angle(pitch_angle_now);
				}	
				
				
				/* 如果15次差值都在4以内就停止 */
				judge_difference_pitch[judge_pitch_cnt++] = pitch_anle_icm20602;
				if(judge_pitch_cnt == 15) judge_pitch_cnt = 0;
				for(i = 0; i < 15; i++) {
					if(!(judge_difference_pitch[i] >= sPID_pitch.SetPoint-4 && judge_difference_pitch[i] <= sPID_pitch.SetPoint+4)) break;
					if(i == 14) pitch_Reach_flag = 1;
				}
			#endif
				
			
			/* 获取Pitch  10ms */
			get_icm20602_accdata_spi();
			get_icm20602_gyro_spi();
			IMU_quaterToEulerianAngles();  /* 获取欧拉角 */
			//printf("%.2f,%.2f,%.2f\n", eulerAngle.pitch, eulerAngle.roll, eulerAngle.yaw);
			if(eulerAngle.roll <=180 && eulerAngle.roll >= 0) {
				pitch_anle_icm20602 = 180-(int)eulerAngle.roll;
			}
			else if(eulerAngle.roll < 0 && eulerAngle.roll >= -180) {
				pitch_anle_icm20602 = -((int)eulerAngle.roll+180);
			}
			//printf("%d\n", pitch_anle_icm20602);
			
		}  /* the end of if(tim_cnt_10 >= 10) */
		
		
		
		
		/* ****************************************************************auto1追踪******************************************* */
		if(tim_cnt_40 >= 40 && track_flag == 1) {
			tim_cnt_40 = 0;
			
			printf("difference: %d\n", difference);
			
			if(staSystem == AUTO1 && difference != 666) {
				
				/* PID */
				yaw_angle_now += PIDCalc(&sPID, difference);
				Yaw_Angle(yaw_angle_now);
				
				/* 如果10次差值都在6以内就停止寻找，然后测距发射 */
				judge_difference[judge_cnt++] = difference;
				if(judge_cnt == 10) judge_cnt = 0;
				for(i = 0; i < 10; i++) {
					if(!(judge_difference[i] >= sPID.SetPoint-6 && judge_difference[i] <= sPID.SetPoint+6)) break;
					if(i == 9) track_flag = 0;
				}
			}
		}
		
		/* *************************************************************跟随模式 MPU6050***************************************** */
		if(tim_cnt_5 >= 5) {
			tim_cnt_5 = 0;
			if(follow_flag == 1) {
				
				#if 1
					MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
					MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
					IMU_quaterToEulerianAngles_mpu6050();  /* 获取欧拉角 */
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
	#if 0
	if(GPIO_Pin == KEY0_Pin) {  /* 充电 */
		delay_ms(50);
		if(HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == 0) {
			HAL_GPIO_TogglePin(Charging_relay_GPIO_Port, Charging_relay_Pin);
		}
		
	}
	#endif
	
	
	#if 1
	if(GPIO_Pin == KEY0_Pin) {  /* 充电放电一体 */
		delay_ms(50);
		if(HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == 0) {
			/* 归位保护 */
			HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
			delay_ms(50);
			delay_ms(1000);
			/* 充电 */
			HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
			
			delay_ms(3500); /* 充电时间 1000ms 45v*/
			
			/* 放电 */
			HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
			delay_ms(600);
			HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_SET);
			
			delay_ms(1000);
			
			/* 归位 */
			HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
		}
	}
	
	#endif
	
	if(GPIO_Pin == KEY1_Pin) {  /* 放电 */
		delay_ms(50);
		if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == 0) {
			Distance_Process();
			printf("distance: %d\n", distance);
			HAL_GPIO_TogglePin(Discharge_relay_GPIO_Port, Discharge_relay_Pin);
		}
	}
	
	#if 0
	if(GPIO_Pin == MPU6050_INT_Pin) { 
		printf("6050 exti\n");
		mpu_dmp_get_data(&pitch,&roll,&yaw);										 //===得到欧拉角（姿态角）的数据
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);								 //===得到陀螺仪数据
		
		printf("%f %f %f \n", pitch, roll, yaw);
	}
	#endif
	
}



/* 炮弹发射函数 */
void fire(u8 voltage)
{
	/* 归位保护 */
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
	
	/* 充电时间 */
	if(voltage == 45) delay_ms(1000);
	else if(voltage == 48) delay_ms(3500); 
	else delay_ms(3500); 
	
	/* 放电 */
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
	HAL_Delay(500);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_SET);
	
	HAL_Delay(1000);
	
	/* 归位 */
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
	
	fire_flag = 1;
	
}




