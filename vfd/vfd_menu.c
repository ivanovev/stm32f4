
#include <main.h>
#include "vfd.h"
#include "vfd_menu.h"
#include "pcl/pcl.h"

typedef struct vfd_menu_state {
    uint8_t lvl;
    struct vfd_menu_item *sel;
    struct vfd_menu_item *scroll;
} vfd_menu_state;

typedef struct vfd_menu_edit_int {
    int v1, v2, cur, step;
} vfd_menu_edit_int;

typedef struct vfd_menu_edit_double {
    double v1, v2, cur, step;
} vfd_menu_edit_double;

typedef struct vfd_menu_edit_list {
    uint8_t count, cur;
    char **data;
} vfd_menu_edit_list;

#define VFD_FLAG_EDIT           (1 << 0)
#define VFD_FLAG_IMMEDIATE      (1 << 1)
#define VFD_FLAG_EDIT_DOUBLE    (1 << 2)
#define VFD_FLAG_EDIT_INT       (1 << 3)
#define VFD_FLAG_EDIT_LIST      (1 << 4)

typedef struct vfd_menu_edit {
    char *cmd;
    uint16_t flags;
    union {
        vfd_menu_edit_list d_l;
        vfd_menu_edit_double d_f;
        vfd_menu_edit_int d_i;
    } data;
} vfd_menu_edit;

typedef struct vfd_menu_item {
    char *name;
    vfd_menu_edit *edit;
    struct vfd_menu_item *next;
    struct vfd_menu_item *prev;
    struct vfd_menu_item *child;
    struct vfd_menu_item *parent;
} vfd_menu_item;

typedef struct vfd_words {
    char **en_ru;
    int len;
} vfd_words;

static vfd_menu_state   *pstate = NULL;
static vfd_menu_item    *pmain = NULL;
static vfd_words        *pwords = NULL;

uint32_t vfd_dt = 1000;

#define MENU_IPADDR "IP-Address"
#define MENU_VERSION "Version"
#define MENU_BRIGHTNESS "Brightness"

static uint16_t vfd_menu_flag(vfd_menu_item *item, uint16_t flag)
{
    if(item->edit)
        return (item->edit->flags & flag) ? 1 : 0;
    return 0;
}

static vfd_menu_item* vfd_menu_new_item(char *name, char *cmd)
{
    vfd_menu_item *item = mycalloc(1, sizeof(vfd_menu_item));
    item->name = mystrdup(name);
    if(cmd)
    {
        item->edit = mycalloc(1, sizeof(vfd_menu_edit));
        item->edit->cmd = mystrdup(cmd);
    }
    return item;
}

static vfd_menu_item* vfd_menu_last(vfd_menu_item *item)
{
    while(item->next)
        item = item->next;
    return item;
}

static vfd_menu_item* vfd_menu_first(vfd_menu_item *item)
{
    while(item->prev)
        item = item->prev;
    return item;
}

static uint8_t vfd_menu_sz(vfd_menu_item *item)
{
    uint8_t j;
    item = vfd_menu_first(item);
    for(j = 0; item->next; j++)
        item = item->next;
    return j;
}

static uint8_t vfd_menu_index(vfd_menu_item *item)
{
    uint8_t j;
    vfd_menu_item *i1 = vfd_menu_first(item);
    for(j = 0; (i1 != item) && i1; j++)
        i1 = i1->next;
    return j;
}

static vfd_menu_item* vfd_menu_append(vfd_menu_item *i1, char *name, char *cmd)
{
    vfd_menu_item *item = vfd_menu_new_item(name, cmd);
    if(!i1)
        return item;
    vfd_menu_item *last = vfd_menu_last(i1);
    last->next = item;
    item->prev = last;
    item->parent = last->parent;
    return item;
}

static vfd_menu_item* vfd_menu_append_child(vfd_menu_item *i1, char *name, char *cmd)
{
    if(i1->child)
        return vfd_menu_append(i1->child, name, cmd);
    vfd_menu_item *item = vfd_menu_new_item(name, cmd);
    if(!i1)
        return item;
    i1->child = item;
    item->parent = i1;
    return item;
}

static void vfd_menu_item_data_get(char *buf, uint16_t sz, vfd_menu_item *item)
{
#ifdef MY_PCL
    char buf1[IO_BUF_SZ];
    mysnprintf(buf1, sz, "%s", item->edit->cmd);
    pcl_exec(buf1);
    if(buf1[0] == '[')
        mystrncpy(buf, &(buf1[4]), sz);
    else
        mystrncpy(buf, buf1, sz);
#else
    mysnprintf(buf, sz, "%s", "N/A");
#endif
}

static void vfd_menu_item_data_set(vfd_menu_item* item)
{
#ifdef MY_PCL
    char buf[IO_BUF_SZ];
    buf[0] = 0;
    if(vfd_menu_flag(item, VFD_FLAG_EDIT_INT) == 1)
    {
        mysnprintf(buf, sizeof(buf), "%s %d", item->edit->cmd, item->edit->data.d_i.cur);
    }
    if(buf[0])
        pcl_exec(buf);
#endif
}

static void vfd_menu_make_edit_int(vfd_menu_item* item, int v1, int v2, int step, uint16_t flags)
{
    vfd_menu_edit_int *d_i = &(item->edit->data.d_i);
    item->edit->flags = flags | VFD_FLAG_EDIT_INT;
    d_i->v1 = v1;
    d_i->v2 = v2;
    d_i->step = step;
    char data[16];
    vfd_menu_item_data_get(data, sizeof(data), item);
    d_i->cur = str2int(data);
    if(d_i->cur < v1)
        d_i->cur = v1;
    if(d_i->cur > v2)
        d_i->cur = v2;
}

void vfd_menu_init(void)
{
    if(pmain)
        return;

    pstate = mycalloc(1, sizeof(vfd_menu_state));
    pmain = vfd_menu_append(pmain, "Control", 0);
    pstate->sel = pmain;
    pstate->scroll = pmain;
    vfd_menu_append(pmain, "Monitor", 0);
    vfd_menu_item *psys = vfd_menu_append(pmain, "System", 0);
    vfd_menu_append_child(psys, "IP address", "sys ipaddr");
    vfd_menu_append_child(psys, "SW version", "sys date");
    vfd_menu_append_child(psys, "HW version", "sys hw");
    vfd_menu_item *item = vfd_menu_append_child(psys, "Brightness", "vfd brightness");
    vfd_menu_make_edit_int(item, 1, 8, 1, VFD_FLAG_IMMEDIATE);
#if 0
    pmain->items[MENU_SYS]->sub = calloc(1, sizeof(vfd_menu_item_list*));
    vfd_menu_append_((vfd_menu_item_list*)pmain->items[MENU_SYS]->sub, MENU_IPADDR, "ipaddr");
    vfd_menu_append_((vfd_menu_item_list*)pmain->items[MENU_SYS]->sub, MENU_VERSION, "build date");
    item = vfd_menu_append_((vfd_menu_item_list*)pmain->items[MENU_SYS]->sub, MENU_BRIGHTNESS, "vfd br");
    vfd_menu_make_edit_double(item, "vfd br", 1, 8, 1, 1);
#endif
    vfd_cls();
    vfd_cp866();
    vfd_brightness(3);
    vfd_menu_draw();
    //vfd_irq_init();
}

static void vfd_reverse(uint8_t r)
{
    char buf[16];
    mysnprintf(buf, sizeof(buf), "0x1F72%02X", r);
    vfd_str(buf);
}

void vfd_menu_draw(void)
{
    vfd_cls();
    if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 0)
    {
        vfd_menu_item *item = pstate->scroll;
        if(pstate->sel == item)
            vfd_reverse(1);
        vfd_str(item->name);
        vfd_reverse(0);
        vfd_str("\r\n");
        item = item->next;
        if(!item)
            return;
        if(pstate->sel == item)
            vfd_reverse(1);
        vfd_str(item->name);
        vfd_reverse(0);
        return;
    }
    if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 1)
    {
        if(vfd_menu_flag(pstate->sel, VFD_FLAG_IMMEDIATE) == 1)
        {
            vfd_menu_item_data_set(pstate->sel);
        }
        vfd_str(pstate->sel->name);
        vfd_str("\r\n");
        char data[32];
        if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT_INT) == 1)
        {
            mysnprintf(data, sizeof(data), "%d", pstate->sel->edit->data.d_i.cur);
        }
        else
            vfd_menu_item_data_get(data, sizeof(data), pstate->sel);
        vfd_str(data);
    }
}

void vfd_menu_down(void)
{
    if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 0)
    {
        if(pstate->sel->next)
        {
            pstate->sel = pstate->sel->next;
            if(pstate->scroll != pstate->sel->prev)
                pstate->scroll = pstate->sel->prev;
            vfd_menu_draw();
        }
    }
    if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 1)
    {
        if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT_INT) == 1)
        {
            vfd_menu_edit_int *d_i = &(pstate->sel->edit->data.d_i);
            d_i->cur -= d_i->step;
            if(d_i->cur < d_i->v1)
                d_i->cur = d_i->v1;
            vfd_menu_draw();
        }
    }
}

void vfd_menu_up(void)
{
    if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 0)
    {
        if(pstate->sel->prev)
        {
            pstate->sel = pstate->sel->prev;
            if(pstate->scroll != pstate->sel)
                pstate->scroll = pstate->sel;
            vfd_menu_draw();
        }
    }
    if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 1)
    {
        if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT_INT) == 1)
        {
            vfd_menu_edit_int *d_i = &(pstate->sel->edit->data.d_i);
            d_i->cur += d_i->step;
            if(d_i->v2 < d_i->cur)
                d_i->cur = d_i->v2;
            vfd_menu_draw();
        }
    }
}

void vfd_menu_right(void)
{
    if(pstate->sel->child)
    {
        pstate->sel = pstate->sel->child;
        pstate->scroll = pstate->sel;
        vfd_menu_draw();
        return;
    }
    if(pstate->sel->edit)
    {
        if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 0)
        {
            pstate->sel->edit->flags |= VFD_FLAG_EDIT;
            vfd_menu_draw();
            return;
        }
    }
}

void vfd_menu_left(void)
{
    if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 1)
    {
        pstate->sel->edit->flags &= ~VFD_FLAG_EDIT;
        vfd_menu_draw();
        return;
    }
    if(pstate->sel->parent)
    {
        pstate->sel = pstate->sel->parent;
        pstate->scroll = pstate->sel;
        if((pstate->scroll->next == 0) && pstate->scroll->prev)
            pstate->scroll = pstate->scroll->prev;
        vfd_menu_draw();
        return;
    }
}

