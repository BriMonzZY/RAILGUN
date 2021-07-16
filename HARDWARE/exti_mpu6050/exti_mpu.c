#include "exti_mpu.h"




void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{ 
	if(GPIO_Pin == MPU6050_INT_Pin) { 
		mpu_dmp_get_data(&pitch,&roll,&yaw);										 //===得到欧拉角（姿态角）的数据
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);								 //===得到陀螺仪数据
		
		printf("%f %f %f \n", pitch, roll, yaw);
	}
	
}
