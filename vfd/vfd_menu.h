
#ifndef __VFD_MENU_H__
#define __VFD_MENU_H__

#include <main.h>

#define VFD_FLAG_EDIT           (1 << 0)
#define VFD_FLAG_IMMEDIATE      (1 << 1)
#define VFD_FLAG_EDIT_DOUBLE    (1 << 2)
#define VFD_FLAG_EDIT_INT       (1 << 3)
#define VFD_FLAG_EDIT_LIST      (1 << 4)
#define VFD_FLAG_TIM_UPD        (1 << 5)

void vfd_menu_init(void);
void vfd_menu_draw(void);
void vfd_menu_down(void);
void vfd_menu_up(void);
void vfd_menu_left(void);
void vfd_menu_right(void);
void vfd_menu_ok(void);
void vfd_menu_tim_upd(void);
void vfd_menu_line(char *buf, uint16_t num);

#endif

