
#ifndef __VFD_MENU_H__
#define __VFD_MENU_H__

#include <main.h>

#define DISPLAY_FLAG_EDIT           (1 << 0)
#define DISPLAY_FLAG_IMMEDIATE      (1 << 1)
#define DISPLAY_FLAG_EDIT_DOUBLE    (1 << 2)
#define DISPLAY_FLAG_EDIT_INT       (1 << 3)
#define DISPLAY_FLAG_EDIT_LIST      (1 << 4)
#define DISPLAY_FLAG_TIM_UPD        (1 << 5)

typedef struct menu_state_t {
    uint8_t lvl;
    struct menu_item_t *sel;
    struct menu_item_t *scroll;
} menu_state_t;

typedef struct menu_edit_int_t {
    int v1, v2, cur, step;
} menu_edit_int_t;

typedef struct menu_edit_double_t {
    double v1, v2, cur, step;
} menu_edit_double_t;

typedef struct menu_edit_list_t {
    uint8_t count, cur;
    char **data;
} menu_edit_list_t;

typedef struct menu_edit_t {
    char *cmd;
    uint16_t flags;
    union {
        menu_edit_list_t d_l;
        menu_edit_double_t d_f;
        menu_edit_int_t d_i;
    } data;
} menu_edit_t;

typedef struct menu_item_t {
    char *name;
    struct menu_edit_t *edit;
    struct menu_item_t *next;
    struct menu_item_t *prev;
    struct menu_item_t *child;
    struct menu_item_t *parent;
} menu_item_t;

void menu_init(void);
void menu_draw(void);
void menu_down(void);
void menu_up(void);
void menu_left(void);
void menu_right(void);
void menu_ok(void);
void menu_home(uint8_t draw);
void menu_tim_upd(void);
void menu_line(char *buf, uint16_t num);

menu_item_t*    menu_append_child(menu_item_t *i1, char *name, char *cmd);
void            menu_make_edit_int(menu_item_t* item, int v1, int v2, int step, uint16_t flags);

#endif

