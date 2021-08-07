# 模拟电磁曲射炮

## 接口定义

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



## 文件内容

actuator.c				位置式PID算法、定时器中断服务函数、外部中断服务函数

uart_dma.c			  uart中断，处理k210传回的数据。激光测距的数据解算代码

gun_action.c			在不同状态下的执行内容、舵机偏转角度函数。

status_transmit.c	状态转移函数

stack.c						用数组实现的栈

attitude_solution.c	陀螺仪数据融合(姿态解算)算法(使用互补滤波算法)



## NOTE

mpu6050dmp和定时器中断冲突，所以不适用dmp，mpu6050用来控制follow模式。

icm20602用来pitch闭环控制（放弃）。

两个陀螺仪使用相同的解算方案。





yawpid   0.35   0.01  0.04



## Picture

<img src="https://gitee.com/BriMon-zZY/blog-image/raw/master/img/202108072118248.jpg" alt="img" style="zoom: 33%;" />



<img src="https://gitee.com/BriMon-zZY/blog-image/raw/master/img/202108072119924.jpg" alt="img" style="zoom:33%;" />