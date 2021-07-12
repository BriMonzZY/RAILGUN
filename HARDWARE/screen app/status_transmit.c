#include "status_transmit.h"


extern u16 get_distance_flag;


void Manual_To_Init(void)
{
	LCD_Clear(WHITE);
	Yaw_Angle(30);
}


void Init_To_Manual(void)
{
	LCD_Clear(WHITE);
	
	/* DISTANCE */
	POINT_COLOR=BLUE;
	LCD_DrawRectangle(140, 240, 180, 280);	//1
	LCD_DrawRectangle(180, 240, 220, 280);	//2
	LCD_DrawRectangle(220, 240, 260, 280);	//3
	LCD_DrawRectangle(140, 280, 180, 320);	//4
	LCD_DrawRectangle(180, 280, 220, 320);	//5
	LCD_DrawRectangle(220, 280, 260, 320);	//6
	LCD_DrawRectangle(140, 320, 180, 360);	//7
	LCD_DrawRectangle(180, 320, 220, 360);	//8
	LCD_DrawRectangle(220, 320, 260, 360);	//9
	LCD_DrawRectangle(180, 360, 220, 400);	//0
	LCD_DrawRectangle(220, 360, 260, 400);	//back
	LCD_DrawRectangle(140, 360, 180, 400);	//enter

	POINT_COLOR=CYAN;
	LCD_ShowString(150,200,120,120,24,"Distance");
	LCD_ShowString(145,245,120,120,24,"1");
	LCD_ShowString(185,245,120,120,24,"2");
	LCD_ShowString(225,245,120,120,24,"3");
	LCD_ShowString(145,285,120,120,24,"4");
	LCD_ShowString(185,285,120,120,24,"5");
	LCD_ShowString(225,285,120,120,24,"6");
	LCD_ShowString(145,325,120,120,24,"7"); 
	LCD_ShowString(185,325,120,120,24,"8");
	LCD_ShowString(225,325,120,120,24,"9");
	LCD_ShowString(185,365,120,120,24,"0");
	LCD_ShowString(225,365,120,120,24,"Ba");
	LCD_ShowString(145,365,120,120,24,"Cl");
	
	
	/* ANGLE */
	POINT_COLOR=BLUE;
	LCD_DrawRectangle(10, 240, 50, 280);	//1
	LCD_DrawRectangle(50, 240, 90, 280);	//2
	LCD_DrawRectangle(90, 240, 130, 280);	//3
	LCD_DrawRectangle(10, 280, 50, 320);	//4
	LCD_DrawRectangle(50, 280, 90, 320);	//5
	LCD_DrawRectangle(90, 280, 130, 320);	//6
	LCD_DrawRectangle(10, 320, 50, 360);	//7
	LCD_DrawRectangle(50, 320, 90, 360);	//8
	LCD_DrawRectangle(90, 320, 130, 360);	//9
	LCD_DrawRectangle(50, 360, 90, 400);	//0
	LCD_DrawRectangle(90, 360, 130, 400);	//back
	LCD_DrawRectangle(10, 360, 50, 400);	//enter
	LCD_DrawRectangle(95, 435, 250, 470); // back to init
	LCD_DrawRectangle(70, 130, 200, 170);  // fire
	
	POINT_COLOR=CYAN;
	LCD_ShowString(20,200,120,120,24,"Angle");
	LCD_ShowString(145-130,245,120,120,24,"1");
	LCD_ShowString(185-130,245,120,120,24,"2");
	LCD_ShowString(225-130,245,120,120,24,"3");
	LCD_ShowString(145-130,285,120,120,24,"4");
	LCD_ShowString(185-130,285,120,120,24,"5");
	LCD_ShowString(225-130,285,120,120,24,"6");
	LCD_ShowString(145-130,325,120,120,24,"7"); 
	LCD_ShowString(185-130,325,120,120,24,"8");
	LCD_ShowString(225-130,325,120,120,24,"9");
	LCD_ShowString(55,365,120,120,24,"0");
	LCD_ShowString(95,365,120,120,24,"Ba");
	LCD_ShowString(15,365,120,120,24,"Cl");
	LCD_ShowString(100,440,200,120,24,"back to init");
	LCD_ShowString(110,135,200,120,24,"Fire");
	
	
	LCD_ShowString(20,60,120,120,24,"Angle:");
	LCD_ShowString(20,90,120,120,24,"Distance:");
	
	LCD_ShowString(230,60,120,120,24,"du");
	LCD_ShowString(230,90,120,120,24,"mm");
}



void Init_To_Auto1(void)
{
	LCD_Clear(WHITE);
	
	POINT_COLOR=BLUE;
	LCD_DrawRectangle(95, 435, 250, 470); // back to init
	POINT_COLOR=CYAN;
	LCD_ShowString(100,440,200,120,24,"back to init");
	
	track_flag = 1;  /* ¿ªÆôÄ¿±ê×·×Ù */
	
}

void Init_To_Auto2(void)
{
	LCD_Clear(WHITE);
	
	POINT_COLOR=BLUE;
	LCD_DrawRectangle(95, 435, 250, 470); // back to init
	POINT_COLOR=CYAN;
	LCD_ShowString(100,440,200,120,24,"back to init");
	
	Yaw_Angle(0);
	xiangzuo = 1;
	
	yaw_angle_now = 0;
	
}


void Auto1_To_Init(void)
{
	LCD_Clear(WHITE);
	Yaw_Angle(30);
	get_distance_flag = 0;
	yaw_angle_now = 30;
	
	track_flag = 0;
}

void Auto2_To_Init(void)
{
	LCD_Clear(WHITE);
	Yaw_Angle(30);
	tim_angle = 0;
	yaw_angle_now=30;
}
