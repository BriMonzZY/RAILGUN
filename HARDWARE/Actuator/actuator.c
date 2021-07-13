#include "actuator.h"



/* 变量定义及声明 */

extern u16 get_distance_flag;


/* 1ms进一次中断 */
uint16_t tim_cnt_50 = 0;
uint16_t tim_cnt_100 = 0;
uint16_t tim_cnt_40 = 0;
uint16_t tim_cnt_5 = 0;

uint8_t xiangzuo = 1;   /* AUTO2模式向左旋转标志位 */
uint8_t angle_flag = 0;  /* 目标所在的角度 */


int judge_difference[10] = {0};  /* 用于判断是否达到标准 */

uint8_t judge_cnt = 0;


/* PID */
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
		tim_cnt_100++;
		tim_cnt_40++;
		tim_cnt_5++;
		
		/* auto2扫描 */
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
					
					/* 打开放电继电器发射 */
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
		
		/* 无用 */
		if(tim_cnt_100 >= 100) {
			tim_cnt_100 = 0;
			//printf("%d \n", distance);
			HAL_UART_Receive_IT(&huart6, temp, 64); 
		}
		
		/* auto1追踪 */
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
		
		/* 跟随模式 */
		if(tim_cnt_5 >= 5) {
			tim_cnt_5 = 0;
			if(follow_flag == 1) {
				
				#if 1
					get_icm20602_accdata_spi();
					get_icm20602_gyro_spi();
					IMU_quaterToEulerianAngles();  /* 获取欧拉角 */
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



/* 炮弹发射函数 */
void fire(void)
{
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_SET);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
	
	HAL_Delay(1000); /* 充电时间 */
	
	/* 放电 */
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
	HAL_Delay(10);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_SET);
	
	HAL_Delay(200);
	
	/* 归位 */
	HAL_GPIO_WritePin(Charging_relay_GPIO_Port,Charging_relay_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(Discharge_relay_GPIO_Port,Discharge_relay_Pin,GPIO_PIN_RESET);
	
	fire_flag = 1;
	
}




