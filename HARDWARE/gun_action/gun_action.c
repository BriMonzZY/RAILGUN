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


/* MANUAL模式要执行的动作 */
void MANUAL_Action(void)
{
	Yaw_Angle(manual_angle);
}



/* AUTO1模式要执行的动作 */
void AUTO1_Action(void)
{
	
	HAL_Delay(20);
	/* track_flag等于0代表找到目标  测距    发射 */
	if(track_flag == 0 && staSystem == AUTO1) {
		fire_distance = distance;
		HAL_Delay(20);  /* 等待 */
		printf("distance: %d\n", fire_distance);
		BEEP_ONCE();
		
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


/* AUTO2要执行的动作 */
void AUTO2_Action(void)
{
	HAL_Delay(20);
}


