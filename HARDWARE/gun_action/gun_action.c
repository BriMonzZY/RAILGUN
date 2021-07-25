#include "gun_action.h"

extern u16 get_distance_flag;


/* 
	偏航角 
	60(9.1%) - 0(6.1%)   0.05% --- 1°
	

*/
void Yaw_Angle(int angle)
{
	angle = angle-6;
	
	if(angle >= 60) angle = 60;
	if(angle <= 0) angle = 0;
	
	__HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, 61 + 0.5*angle);
}

/* 俯仰角 */
void Pitch_Angle(int angle)
{
	angle = angle-5;   /* (角度-舵机打角差值+炮管角度差值(激光水平时角度值)) */
	
	if(angle >= 60) angle = 60;
	if(angle <= 0) angle = 0;
	
	__HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, 78 - 0.4*angle);
}


/* MANUAL模式要执行的动作 */
void MANUAL_Action(void)
{
	u8 i, min_dif_index;
	float min_dif;  /* 距离和表中的差值的最小值 */
	
	
	Yaw_Angle(manual_angle);
	
	
	/* 寻找最适合的发射角度和电压 */
	min_dif = abs(angle_distance_48v_45v[0][0]*1000 - manual_distance);
	min_dif_index = 0;
	for(i = 0; i <= 34; i++) {
		if(abs(angle_distance_48v_45v[i][0]*1000 - manual_distance) <= min_dif) {
			min_dif_index = i;
			min_dif = abs(angle_distance_48v_45v[i][0]*1000 - manual_distance);
		}
	}
	printf("distance: %f  angle: %f  voltage: %f\n", angle_distance_48v_45v[min_dif_index][0], angle_distance_48v_45v[min_dif_index][1], angle_distance_48v_45v[min_dif_index][2]);
	
	
	/* Pitch转到对应角度 */
	Pitch_Angle(angle_distance_48v_45v[min_dif_index][1]);
	
	/* 如果第一次角度不符合要求就再来一次 */
	HAL_Delay(1000);  
	if(!(pitch_anle_icm20602 >= angle_distance_48v_45v[min_dif_index][1]-5 && pitch_anle_icm20602 <= angle_distance_48v_45v[min_dif_index][1]+5)) {
		Pitch_Angle(0);
		HAL_Delay(1000);
		Pitch_Angle(angle_distance_48v_45v[min_dif_index][1]);
	}
	
	
	BEEP_ONCE();
	HAL_Delay(800);
	fire(angle_distance_48v_45v[min_dif_index][2]);  /* 发射 */
	HAL_Delay(1000);
	while(fire_flag == 0);	
	fire_flag = 0;
	pitch_Reach_flag = 0;
	Manual_To_Init();
}



/* AUTO1模式要执行的动作 */
void AUTO1_Action(void)
{
	u8 min_dif_index;
	float min_dif;  /* 距离和表中的差值的最小值 */
	
	HAL_Delay(20);
	/* track_flag等于0代表找到目标  测距    发射 */
	if(track_flag == 0 && staSystem == AUTO1) {
		
		fire_distance = 0;
		u8 i;
		Distance_Process();
		fire_distance= distance - 250;
		
		/* 等待 */
		HAL_Delay(20);  
		printf("distance: %d\n", fire_distance);
		
		/* 寻找最适合的发射角度和电压 */
		min_dif = abs(angle_distance_48v_45v[0][0]*1000 - fire_distance);
		min_dif_index = 0;
		for(i = 0; i <= 34; i++) {
			if(abs(angle_distance_48v_45v[i][0]*1000 - fire_distance) <= min_dif) {
				min_dif_index = i;
				min_dif = abs(angle_distance_48v_45v[i][0]*1000 - fire_distance);
			}
		}
		printf("distance: %f  angle: %f  voltage: %f\n", angle_distance_48v_45v[min_dif_index][0], angle_distance_48v_45v[min_dif_index][1], angle_distance_48v_45v[min_dif_index][2]);
		
		BEEP_ONCE();
		fire(angle_distance_48v_45v[min_dif_index][1]);/* 充电 发射 */
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



void FOLLOW_Action(void)
{
	HAL_Delay(20);
	
	follow_flag = 1;
}

