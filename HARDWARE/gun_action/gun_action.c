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
	
	__HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, 78 - 0.4*angle);
}


/* MANUAL模式要执行的动作 */
void MANUAL_Action(void)
{
	Yaw_Angle(manual_angle);
	
	Pitch_Angle(manual_distance);
	
	
	HAL_Delay(500);
	fire();
	HAL_Delay(500);
	
	while(fire_flag == 0);
	fire_flag = 0;
	
	Manual_To_Init();
}



/* AUTO1模式要执行的动作 */
void AUTO1_Action(void)
{
	
	HAL_Delay(20);
	/* track_flag等于0代表找到目标  测距    发射 */
	if(track_flag == 0 && staSystem == AUTO1) {
		
		fire_distance = 0;
		u8 i;
		u8 effect_cnt = 0; /* 有效的个数 */
		for(i = 0; i < 4; i++) {
			if(distance_aver[i] != 0 && distance_aver[i] <= 3500) {
				effect_cnt++;
				fire_distance += distance_aver[i];
			}				
		}
		fire_distance /= effect_cnt;
		
		
		HAL_Delay(20);  /* 等待 */
		
		printf("distance: %d\n", fire_distance);
		BEEP_ONCE();
		
		fire();/* 充电 发射 */
		/* 发射完成后转到init */
		while(1);
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



void FOLLOW_Action(void)
{
	HAL_Delay(20);
	
	follow_flag = 1;
}

