#include "gun_action.h"

extern u16 get_distance_flag;


/* 
	偏航角 
	60(9.1%) - 0(6.1%)   0.05% --- 1°
	

*/
void Yaw_Angle(int angle)
{
	if(angle >= 60) angle = 60;
	if(angle <= 0) angle = 0;
	
	__HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, 61 + 0.5*angle);
}

/* 俯仰角 */
void Pitch_Angle(uint8_t angle)
{
	if(angle >= 60) angle = 60;
	if(angle <= 0) angle = 0;
	
	__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 78 - 0.4*angle);
}


void MANUAL_Action(void)
{
	Yaw_Angle(manual_angle);
}



void AUTO1_Action(void)
{
	get_icm20602_accdata_spi();
	get_icm20602_gyro_spi();
	IMU_quaterToEulerianAngles();  /* 获取欧拉角 */
	//printf("%.2f,%.2f,%.2f\n", eulerAngle.pitch, eulerAngle.roll, eulerAngle.yaw);
	//printf("%.2f,%.2f,%.2f\n\n", values[0], values[1], values[2]);
	HAL_Delay(20);
	
	
	//get_distance_flag = 1;  /* 获取距离 */
	
	
	
	/* track_flag等于0代表找到目标    测距    发射 */
	if(track_flag == 0 && staSystem == AUTO1) {
		Get_Distance();
		HAL_Delay(20);  /* 等待测距 */
		printf("distance: %d\n", distance);
		
		fire();
		HAL_Delay(5);
		
		/* 发射完成后转到init */
		while(fire_flag == 0);
		staSystem = INIT;
		HAL_Delay(100);
		Auto1_To_Init();
		fire_flag = 0;
	}
	
	
}


void AUTO2_Action(void)
{
	get_icm20602_accdata_spi();
	get_icm20602_gyro_spi();

	IMU_quaterToEulerianAngles();  /* 获取欧拉角 */
	//printf("%.2f,%.2f,%.2f\n", eulerAngle.pitch, eulerAngle.roll, eulerAngle.yaw);
	//printf("%.2f,%.2f,%.2f\n\n", values[0], values[1], values[2]);
	HAL_Delay(20);
	
}


