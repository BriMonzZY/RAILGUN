import machine
import sensor
import lcd
import time
import image
import sys
import utime
from Maix import GPIO
from Maix import freq
from fpioa_manager import fm
from board import board_info
from machine import UART
from machine import Timer
import math

# 超频
# freq.set(cpu=400, pll1=400, kpu_div=1)
print(freq.get())  # 打印主频


# 全局变量定义
led_cnt = 0  # LED闪烁计数位
status = 0  # LED状态位
Difference = 0  # 和中心的差值


# RGB初始化
fm.register(board_info.LED_R,fm.fpioa.GPIO0)
led_r=GPIO(GPIO.GPIO0,GPIO.OUT)
fm.register(board_info.LED_B,fm.fpioa.GPIO1)
led_b=GPIO(GPIO.GPIO1,GPIO.OUT)
fm.register(board_info.LED_G,fm.fpioa.GPIO2)
led_g=GPIO(GPIO.GPIO2,GPIO.OUT)
led_g.value(1)
led_b.value(1)
led_r.value(1)


# 按键
fm.register(board_info.BOOT_KEY, fm.fpioa.GPIOHS1)
key = GPIO(GPIO.GPIOHS1, GPIO.IN, GPIO.PULL_UP)



# 初始化uart引脚    分配uart2    IO分配  10 TX 11 RX
# uart_A用于和stm32通讯
fm.register(10, fm.fpioa.UART2_TX, force=True)
fm.register(11, fm.fpioa.UART2_RX, force=True)
uart_A = machine.UART(UART.UART2, 115200, 8, 0, 1, timeout=1000, read_buf_len=4096)


# 定时器中断回调函数
def on_timer(timer):

    # uart_A.write('%d %d %d\r\n'%(123, -123, 432))
    uart_A.write('%d\r\n'%(Difference))
    # uart_A.write('%d\r\n'%(123))

    # print("timer")
    global led_cnt
    global status

    # 红色LED闪烁
    led_cnt+=1
    if led_cnt >= 50:
        led_r.value(status)
        led_cnt = 0
        status = 0 if (status == 1) else 1


# 初始化定时器中断  TIMER1       通道0           中断模式           TIMER周期5ms    周期单位ms         回调函数   希望传给回调函数的参数  立即开启定时器   中断优先级  分频
tim = Timer(Timer.TIMER1, Timer.CHANNEL0, mode=Timer.MODE_PERIODIC, period=5, unit=Timer.UNIT_MS, callback=on_timer, arg=on_timer, start=False, priority=1, div=0)
print("period:",tim.period())  # 打印定时器参数
# tim.start()  # 开启定时器

# 摄像头初始化
clock = time.clock()
lcd.init(freq=15000000)
# lcd.rotation(0)
# lcd.mirror()
sensor.reset()  # 重置
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_jb_quality(80)  # 传送给IDE的图像质量
sensor.set_contrast(0)  # 对比度
sensor.set_brightness(-1)  # 亮度
# sensor.set_auto_whitebal(True)  # 白平衡
sensor.set_auto_whitebal(False)
# sensor.set_saturation(-2)  # 饱和度
sensor.set_vflip(0)
sensor.set_hmirror(0)

sensor.run(1)
sensor.skip_frames(30)  # 跳过初始帧


# red_threshold = (16, 42, 18, 33, -4, 16)
red_threshold = (11, 43, 22, 53, -19, 50)
red_threshold_tmp = []




"""确定当前环境下红色阈值"""

"""
while key.value() == 1: # 等待按键按下
    led_b.value(0)

utime.sleep_ms(10)  # 延时消抖


if key.value() == 0:
    print("debug")
    sensor.set_windowing((140, 100, 40, 40)) # 设置ROI为中间部分
    img = sensor.snapshot()  # 截取图像
    statistics = img.get_statistics()

    red_threshold_tmp.append(statistics.l_min())
    red_threshold_tmp.append(statistics.l_max())
    red_threshold_tmp.append(statistics.a_min())
    red_threshold_tmp.append(statistics.a_max())
    red_threshold_tmp.append(statistics.b_min())
    red_threshold_tmp.append(statistics.b_max())

    red_threshold_tmp = tuple(red_threshold_tmp)
    print(red_threshold_tmp)
sensor.set_windowing((0, 0, 320, 240))  # 设置回原来的ROI
led_b.value(1)
"""



tim.start()  # 开启定时器

# 主循环
while True:
    clock.tick()
    img = sensor.snapshot()  # 截取图像
    fps =clock.fps()  # 计算帧率
    img.draw_string(2,2, ("%2.1ffps" %(fps)), color=(255, 255, 255), scale=2)
    img = img.draw_cross(160, 120)  # 在中心绘制一个十字

    # 获取色块
    blobs = img.find_blobs([red_threshold], area_threshold=200, roi=(0, 0, 320, 240), merge=True)
    if blobs:  # 如果成功
        max_area = 0
        max_i = 0

        # 寻找最大的面积的色块
        for i, j in enumerate(blobs):
            a = j[2]*j[3]  # 面积
            if a > max_area:  # 寻找最大
                max_i = i  # 最大数的索引值
                max_area = a

        img = img.draw_rectangle(blobs[max_i].rect())  # 边缘画矩形
        img = img.draw_cross(blobs[max_i][5], blobs[max_i][6])
        print("%d" %(blobs[max_i][5]-160))  # 打印和中心位置的偏差

        Difference = blobs[max_i][5]-160


    else:
        print("not_found")
        Difference = 666
    lcd.display(img)


"""

0,0               320,0     x


        160,120


0,240             320,240

y


(0, 0, 320, 240)
(x  y   w    h )

"""




"""
        print('该形状占空比为',blobs[i].density())

        if blobs[i].density()>0.805:  # 检测为矩形
            print("rec")
        elif blobs[i].density()>0.65:
            print("cir")
            img.draw_keypoints([(blobs[i].cx(), blobs[i].cy(), int(math.degrees(blobs[i].rotation())))], size=20)
            img.draw_circle((blobs[i].cx(), blobs[i].cy(),int((blobs[i].w()+blobs[i].h())/4)))
            print('圆形半径',(blobs[i].w()+blobs[i].h())/4)
            print("%d" %(blobs[max_i][5]-160))  # 打印和中心位置的偏差
            Difference = blobs[max_i][5]-160
"""

