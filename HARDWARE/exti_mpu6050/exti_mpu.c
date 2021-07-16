#include "exti_mpu.h"




void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{ 
	if(GPIO_Pin == MPU6050_INT_Pin) { 
		mpu_dmp_get_data(&pitch,&roll,&yaw);										 //===�õ�ŷ���ǣ���̬�ǣ�������
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);								 //===�õ�����������
		
		printf("%f %f %f \n", pitch, roll, yaw);
	}
	
}
