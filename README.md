PB10			UART3_TX
PB11			UART3_RX



PA9			  UART1_TX

PA10		    UART1_RX



PA2			UART2_TX

PA3			UART2_RX



PC7			UART6_RX

PC6			UART6_TX



PA5			SCL

PA6			MISO SDO

PA7			MOSI  SDI

PC4			SPI1_cs



PF7			TIM11ch1	yaw		挂载在APB2上

PF6			TIM10ch1	pitch		挂载在APB2上  180MHz

PF8			TIM13ch1	pitch		挂载在APB1上  90MHz



TIM2挂载在APB1上



PE2			放电寄存器

PE3			充电寄存器



PH2			KEY1

PH3			KEY0



PE4			舵机隔离继电器



PF9			mpu6050	SCL

PE5			mpu6050	SDA

PE6			mpu6050	INT





actuator.c				PID算法、定时器中断服务函数、外部中断服务函数

uart_dma.c				uart中断

gun_action				在不同状态下的执行内容、舵机偏转角度函数





mpu6050dmp和定时器中断冲突，所以不适用dmp，mpu6050用来控制follow模式。

icm20602用来pitch闭环控制。

两个用相同的解算方案。





yawpid   0.35   0.01  0.04