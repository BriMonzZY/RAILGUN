#ifndef __SCREEN_APP_H
#define __SCREEN_APP_H

#include "sys.h"

extern const u16 POINT_COLOR_TBL[10];
extern u8 local_position;
void ctp_test(void);
void Button_App(void);
void railgun_MANUAL_GUI_INIT(void); //GUI
void railgun_INIT_GUI_INIT(void);
void railgun_AUTO1_GUI_INIT(void);
void railgun_AUTO2_GUI_INIT(void);
void railgun_button(void); //key
void railgun_FOLLOW_GUI_INIT(void);
#endif
