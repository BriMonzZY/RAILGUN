#include "gun_action.h"

extern u16 get_distance_flag;


/* 
	ƫ���� 
	60(9.1%) - 0(6.1%)   0.05% --- 1��
	

*/
void Yaw_Angle(int angle)
{
	angle = angle-6;
	
	if(angle >= 60) angle = 60;
	if(angle <= 0) angle = 0;
	
	__HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, 61 + 0.5*angle);
}

/* ������ */
void Pitch_Angle(int angle)
{
	angle = angle-5;   /* (�Ƕ�-�����ǲ�ֵ+�ڹܽǶȲ�ֵ(����ˮƽʱ�Ƕ�ֵ)) */
	
	if(angle >= 60) angle = 60;
	if(angle <= 0) angle = 0;
	
	__HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, 78 - 0.4*angle);
}


/* MANUALģʽҪִ�еĶ��� */
void MANUAL_Action(void)
{
	u8 i, min_dif_index;
	float min_dif;  /* ����ͱ��еĲ�ֵ����Сֵ */
	
	
	Yaw_Angle(manual_angle);
	
	
	/* Ѱ�����ʺϵķ���ǶȺ͵�ѹ */
	min_dif = abs(angle_distance_48v_45v[0][0]*1000 - manual_distance);
	min_dif_index = 0;
	for(i = 0; i <= 34; i++) {
		if(abs(angle_distance_48v_45v[i][0]*1000 - manual_distance) <= min_dif) {
			min_dif_index = i;
			min_dif = abs(angle_distance_48v_45v[i][0]*1000 - manual_distance);
		}
	}
	printf("distance: %f  angle: %f  voltage: %f\n", angle_distance_48v_45v[min_dif_index][0], angle_distance_48v_45v[min_dif_index][1], angle_distance_48v_45v[min_dif_index][2]);
	
	
	/* Pitchת����Ӧ�Ƕ� */
	Pitch_Angle(angle_distance_48v_45v[min_dif_index][1]);
	
	/* �����һ�νǶȲ�����Ҫ�������һ�� */
	HAL_Delay(1000);  
	if(!(pitch_anle_icm20602 >= angle_distance_48v_45v[min_dif_index][1]-5 && pitch_anle_icm20602 <= angle_distance_48v_45v[min_dif_index][1]+5)) {
		Pitch_Angle(0);
		HAL_Delay(1000);
		Pitch_Angle(angle_distance_48v_45v[min_dif_index][1]);
	}
	
	
	BEEP_ONCE();
	HAL_Delay(800);
	fire(angle_distance_48v_45v[min_dif_index][2]);  /* ���� */
	HAL_Delay(1000);
	while(fire_flag == 0);	
	fire_flag = 0;
	pitch_Reach_flag = 0;
	Manual_To_Init();
}



/* AUTO1ģʽҪִ�еĶ��� */
void AUTO1_Action(void)
{
	u8 min_dif_index;
	float min_dif;  /* ����ͱ��еĲ�ֵ����Сֵ */
	
	HAL_Delay(20);
	/* track_flag����0�����ҵ�Ŀ��  ���    ���� */
	if(track_flag == 0 && staSystem == AUTO1) {
		
		fire_distance = 0;
		u8 i;
		Distance_Process();
		fire_distance= distance - 250;
		
		/* �ȴ� */
		HAL_Delay(20);  
		printf("distance: %d\n", fire_distance);
		
		/* Ѱ�����ʺϵķ���ǶȺ͵�ѹ */
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
		fire(angle_distance_48v_45v[min_dif_index][1]);/* ��� ���� */
		/* ������ɺ�ת��init */
		while(fire_flag == 0);
		staSystem = INIT;
		HAL_Delay(100);
		Auto1_To_Init();
		fire_flag = 0;
	}
}


/* AUTO2Ҫִ�еĶ��� */
void AUTO2_Action(void)
{
	HAL_Delay(20);
}



void FOLLOW_Action(void)
{
	HAL_Delay(20);
	
	follow_flag = 1;
}

