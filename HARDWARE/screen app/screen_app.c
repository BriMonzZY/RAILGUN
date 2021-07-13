
#include "main.h"

/* 变量定义 */
u8 t=0;
u8 i=0;
u16 lastpos[5][2];		//最后一次的数据
long long Freq_yuzhi = 0, A_yuzhi = 0;
u16 Freq_judge = 0, A_judge = 0;
u16 sweep_judge = 0;  // 扫频标志位



////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
    if((x0+len-1)>=lcddev.width)x0=lcddev.width-len-1;	//限制坐标范围
    if(y0>=lcddev.height)y0=lcddev.height-1;			//限制坐标范围
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}


/**
 * @brief    	画实心圆
 * @param    	x0,y0:坐标
							r:半径
							color:颜色
 * @retval   	无
 */
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  


/**
 * @brief   两个数之差的绝对值 
 * @param   x1,x2：需取差值的两个数
 * @retval  返回值：|x1-x2|
 */
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  


/**
 * @brief    画一条粗线
 * @param    (x1,y1),(x2,y2):线条的起始坐标
						  size：线条的粗细程度
						  color：线条的颜色
 * @retval   无
 */
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}
}





#if 0
	void Button_App(void)
	{
		printf("%d %d \n", tp_dev.x[t], tp_dev.y[t]);  /* 输出坐标 */
		if(cnt == 1) {
			
			if(tp_dev.x[t] >= 380 && tp_dev.x[t] <= 480 && tp_dev.y[t] >= 220 && tp_dev.y[t] <= 280){ 
				
			}
			
		}  /* the end of if(cnt == 1) */
	}
#endif

	
	
	
	
	
	
	

/* BRIMON'S BUTTON */
	
void railgun_INIT_GUI_INIT(void)
{
	LCD_Display_Dir(0);
	
	/* init基本交互界面 */
	POINT_COLOR=BLUE;
	LCD_DrawRectangle(75, 220, 195, 270);	 	/* MANUAL */
	LCD_DrawRectangle(75, 270,195, 320);		/* AUTO1 */
	LCD_DrawRectangle(75, 320, 195, 370); 	/* AUTO2 */
	LCD_DrawRectangle(75, 370, 195, 420); 	/* FOLLOW */
	POINT_COLOR=CYAN;
	LCD_ShowString(100,20,120,120,24,"WELCOM");
  LCD_ShowString(90,230,100,100,32,"MANUAL");
	LCD_ShowString(95,280,100,100,32,"AUTO1");
  LCD_ShowString(95,330,100,100,32,"AUTO2");	
	LCD_ShowString(95,380,100,100,32,"FOLLOW");	
	
	/* 按键按下 */
	if(cnt == 1){
		/* 状态切换 */
		/**************MANUAL****************/
		if(tp_dev.x[t] >= 75 && tp_dev.x[t] <= 195 && tp_dev.y[t] >= 220 && tp_dev.y[t] <= 270){
			BEEP_ONCE();
			staSystem = MANUAL;
			Init_To_Manual();
		}
		/**************AUTO1****************/
		if(tp_dev.x[t] >= 75 && tp_dev.x[t] <= 195 && tp_dev.y[t] >= 270 && tp_dev.y[t] <= 320){
			BEEP_ONCE();
			staSystem = AUTO1;
			Init_To_Auto1();
		}
		/**************AUTO2****************/
		if(tp_dev.x[t] >= 75 && tp_dev.x[t] <= 195 && tp_dev.y[t] >= 320 && tp_dev.y[t] <= 370){
			BEEP_ONCE();
			staSystem = AUTO2;
			Init_To_Auto2();
		}
		/**************FOLLOW****************/
		if(tp_dev.x[t] >= 75 && tp_dev.x[t] <= 195 && tp_dev.y[t] >= 370 && tp_dev.y[t] <= 420){
			BEEP_ONCE();
			staSystem = FOLLOW;
			Init_To_Follow();
		}
	}
}

/* MANUAL界面 */
void railgun_MANUAL_GUI_INIT(void)
{
	if(staSystem == MANUAL) {
		LCD_ShowxNum(150,60,manual_angle,6,24,0XFE);
		LCD_ShowxNum(150,90,manual_distance,6,24,0XFE);
		
		if(cnt == 1) {
			/* distance 1 */
			if(tp_dev.x[t] >= 140 && tp_dev.x[t] <= 180 && tp_dev.y[t] >= 240 && tp_dev.y[t] <= 280){
				BEEP_ONCE();
				manual_distance = manual_distance*10 + 1;
			}
			/* distance 2 */
			if(tp_dev.x[t] >= 180 && tp_dev.x[t] <= 220 && tp_dev.y[t] >= 240 && tp_dev.y[t] <= 280){
				BEEP_ONCE();
				manual_distance = manual_distance*10 + 2;
			}
			/* distance 3 */
			if(tp_dev.x[t] >= 220 && tp_dev.x[t] <= 260 && tp_dev.y[t] >= 240 && tp_dev.y[t] <= 280){
				BEEP_ONCE();
				manual_distance = manual_distance*10 + 3;
			}
			/* distance 4 */
			if(tp_dev.x[t] >= 140 && tp_dev.x[t] <= 180 && tp_dev.y[t] >= 280 && tp_dev.y[t] <= 320){
				BEEP_ONCE();
				manual_distance = manual_distance*10 + 4;
			}
			/* distance 5 */
			if(tp_dev.x[t] >= 180 && tp_dev.x[t] <= 220 && tp_dev.y[t] >= 280 && tp_dev.y[t] <= 320){
				BEEP_ONCE();
				manual_distance = manual_distance*10 + 5;
			}
			/* distance 6 */
			if(tp_dev.x[t] >= 220 && tp_dev.x[t] <= 260 && tp_dev.y[t] >= 280 && tp_dev.y[t] <= 320){
				BEEP_ONCE();
				manual_distance = manual_distance*10 + 6;
			}
			/* distance 7 */
			if(tp_dev.x[t] >= 140 && tp_dev.x[t] <= 180 && tp_dev.y[t] >= 320 && tp_dev.y[t] <= 360){
				BEEP_ONCE();
				manual_distance = manual_distance*10 + 7;
			}
			/* distance 8 */
			if(tp_dev.x[t] >= 180 && tp_dev.x[t] <= 220 && tp_dev.y[t] >= 320 && tp_dev.y[t] <= 360){
				BEEP_ONCE();
				manual_distance = manual_distance*10 + 8;
			}
			/* distance 9 */
			if(tp_dev.x[t] >= 220 && tp_dev.x[t] <= 260 && tp_dev.y[t] >= 320 && tp_dev.y[t] <= 360){
				BEEP_ONCE();
				manual_distance = manual_distance*10 + 9;
			}
			/* distance 0 */
			if(tp_dev.x[t] >= 180 && tp_dev.x[t] <= 220 && tp_dev.y[t] >= 360 && tp_dev.y[t] <= 400){
				BEEP_ONCE();
				manual_distance = manual_distance*10 + 0;
			}
			/* distance back */
			if(tp_dev.x[t] >= 220 && tp_dev.x[t] <= 260 && tp_dev.y[t] >= 360 && tp_dev.y[t] <= 400){
				BEEP_ONCE();
				manual_distance/=10;
			}
			/* distance clear */
			if(tp_dev.x[t] >= 140 && tp_dev.x[t] <= 180 && tp_dev.y[t] >= 360 && tp_dev.y[t] <= 400){
				BEEP_ONCE();
				manual_distance = 0;
			}
			
			
			
			
			/* angle 1 */
			if(tp_dev.x[t] >= 140-130 && tp_dev.x[t] <= 180-130 && tp_dev.y[t] >= 240 && tp_dev.y[t] <= 280){
				BEEP_ONCE();
				manual_angle = manual_angle*10 + 1;
			}
			/* angle 2 */
			if(tp_dev.x[t] >= 180-130 && tp_dev.x[t] <= 220-130 && tp_dev.y[t] >= 240 && tp_dev.y[t] <= 280){
				BEEP_ONCE();
				manual_angle = manual_angle*10 + 2;
			}
			/* angle 3 */
			if(tp_dev.x[t] >= 220-130 && tp_dev.x[t] <= 260-130 && tp_dev.y[t] >= 240 && tp_dev.y[t] <= 280){
				BEEP_ONCE();
				manual_angle = manual_angle*10 + 3;
			}
			/* angle 4 */
			if(tp_dev.x[t] >= 140-130 && tp_dev.x[t] <= 180-130 && tp_dev.y[t] >= 280 && tp_dev.y[t] <= 320){
				BEEP_ONCE();
				manual_angle = manual_angle*10 + 4;
			}
			/* angle 5 */
			if(tp_dev.x[t] >= 180-130 && tp_dev.x[t] <= 220-130 && tp_dev.y[t] >= 280 && tp_dev.y[t] <= 320){
				BEEP_ONCE();
				manual_angle = manual_angle*10 + 5;
			}
			/* angle 6 */
			if(tp_dev.x[t] >= 220-130 && tp_dev.x[t] <= 260-130 && tp_dev.y[t] >= 280 && tp_dev.y[t] <= 320){
				BEEP_ONCE();
				manual_angle = manual_angle*10 + 6;
			}
			/* angle 7 */
			if(tp_dev.x[t] >= 140-130 && tp_dev.x[t] <= 180-130 && tp_dev.y[t] >= 320 && tp_dev.y[t] <= 360){
				BEEP_ONCE();
				manual_angle = manual_angle*10 + 7;
			}
			/* angle 8 */
			if(tp_dev.x[t] >= 180-130 && tp_dev.x[t] <= 220-130 && tp_dev.y[t] >= 320 && tp_dev.y[t] <= 360){
				BEEP_ONCE();
				manual_angle = manual_angle*10 + 8;
			}
			/* angle 9 */
			if(tp_dev.x[t] >= 220-130 && tp_dev.x[t] <= 260-130 && tp_dev.y[t] >= 320 && tp_dev.y[t] <= 360){
				BEEP_ONCE();
				manual_angle = manual_angle*10 + 9;
			}
			/* angle 0 */
			if(tp_dev.x[t] >= 180-130 && tp_dev.x[t] <= 220-130 && tp_dev.y[t] >= 360 && tp_dev.y[t] <= 400){
				BEEP_ONCE();
				manual_angle = manual_angle*10 + 0;
			}
			/* angle back */
			if(tp_dev.x[t] >= 220-130 && tp_dev.x[t] <= 260-130 && tp_dev.y[t] >= 360 && tp_dev.y[t] <= 400){
				BEEP_ONCE();
				manual_angle/=10;
			}
			/* angle clear */
			if(tp_dev.x[t] >= 140-130 && tp_dev.x[t] <= 180-130 && tp_dev.y[t] >= 360 && tp_dev.y[t] <= 400){
				BEEP_ONCE();
				manual_angle = 0;
			}
			
			/* back to the init */
			if(tp_dev.x[t] >= 95 && tp_dev.x[t] <= 250 && tp_dev.y[t] >= 435 && tp_dev.y[t] <= 470){
				BEEP_ONCE();
				staSystem = INIT;
				Manual_To_Init();
			}
			/* fire */
			if(tp_dev.x[t] >= 70 && tp_dev.x[t] <= 200 && tp_dev.y[t] >= 130 && tp_dev.y[t] <= 170){
				BEEP_ONCE();
				MANUAL_Action();
			}
		}	/* the end of cnt == 1 */
	}
}
	
void railgun_AUTO1_GUI_INIT(void)
{
  if(staSystem == AUTO1){
		
		AUTO1_Action();
		if(cnt == 1) {
			/* back to the init */
			if(tp_dev.x[t] >= 95 && tp_dev.x[t] <= 250 && tp_dev.y[t] >= 435 && tp_dev.y[t] <= 470){
				BEEP_ONCE();
				staSystem = INIT;
				Auto1_To_Init();
			}
		}
	}
}
	
void railgun_AUTO2_GUI_INIT(void)
{
	if(staSystem == AUTO2){
		
		AUTO2_Action();
		
		if(cnt == 1) {
			/* back to the init */
			if(tp_dev.x[t] >= 95 && tp_dev.x[t] <= 250 && tp_dev.y[t] >= 435 && tp_dev.y[t] <= 470){
				BEEP_ONCE();
				staSystem = INIT;
				Auto2_To_Init();
			}
		}
	}
}


void railgun_FOLLOW_GUI_INIT(void)
{
		FOLLOW_Action();
		
		if(cnt == 1) {
			/* back to the init */
			if(tp_dev.x[t] >= 95 && tp_dev.x[t] <= 250 && tp_dev.y[t] >= 435 && tp_dev.y[t] <= 470){
				BEEP_ONCE();
				staSystem = INIT;
				Follow_To_Init();
			}
		}
}


	
void railgun_button(void)
{
	printf("%d %d \n", tp_dev.x[t], tp_dev.y[t]);  // 输出坐标
}
// USAMI'S BUTTON
