
#ifndef __VFD_MENU_H__
#define __VFD_MENU_H__

#include <main.h>

#define VFD_FLAG_EDIT           (1 << 0)
#define VFD_FLAG_IMMEDIATE      (1 << 1)
#define VFD_FLAG_EDIT_DOUBLE    (1 << 2)
#define VFD_FLAG_EDIT_INT       (1 << 3)
#define VFD_FLAG_EDIT_LIST      (1 << 4)
#define VFD_FLAG_TIM_UPD        (1 << 5)

typedef struct vfd_menu_state_t {
    uint8_t lvl;
    struct vfd_menu_item_t *sel;
    struct vfd_menu_item_t *scroll;
} vfd_menu_state_t;

typedef struct vfd_menu_edit_int_t {
    int v1, v2, cur, step;
} vfd_menu_edit_int_t;

typedef struct vfd_menu_edit_double_t {
    double v1, v2, cur, step;
} vfd_menu_edit_double_t;

typedef struct vfd_menu_edit_list_t {
    uint8_t count, cur;
    char **data;
} vfd_menu_edit_list_t;

typedef struct vfd_menu_edit_t {
    char *cmd;
    uint16_t flags;
    union {
        vfd_menu_edit_list_t d_l;
        vfd_menu_edit_double_t d_f;
        vfd_menu_edit_int_t d_i;
    } data;
} vfd_menu_edit_t;

typedef struct vfd_menu_item_t {
    char *name;
    struct vfd_menu_edit_t *edit;
    struct vfd_menu_item_t *next;
    struct vfd_menu_item_t *prev;
    struct vfd_menu_item_t *child;
    struct vfd_menu_item_t *parent;
} vfd_menu_item_t;

void vfd_menu_init(void);
void vfd_menu_draw(void);
void vfd_menu_down(void);
void vfd_menu_up(void);
void vfd_menu_left(void);
void vfd_menu_right(void);
void vfd_menu_ok(void);
void vfd_menu_tim_upd(void);
void vfd_menu_line(char *buf, uint16_t num);

vfd_menu_item_t*    vfd_menu_append_child(vfd_menu_item_t *i1, char *name, char *cmd);
void                vfd_menu_make_edit_int(vfd_menu_item_t* item, int v1, int v2, int step, uint16_t flags);

#endif

