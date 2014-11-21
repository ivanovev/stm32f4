
#include <main.h>
#include "vfd.h"
#include "vfd_menu.h"
#include "pcl/pcl.h"

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

typedef struct vfd_words_t {
    char **en_ru;
    int len;
} vfd_words_t;

static vfd_menu_state_t   *pstate = NULL;
static vfd_menu_item_t    *pmain = NULL;
static vfd_words_t        *pwords = NULL;

static uint16_t vfd_menu_flag(vfd_menu_item_t *item, uint16_t flag)
{
    if(item->edit)
        return (item->edit->flags & flag) ? 1 : 0;
    return 0;
}

static vfd_menu_item_t* vfd_menu_new_item(char *name, char *cmd)
{
    vfd_menu_item_t *item = mycalloc(1, sizeof(vfd_menu_item_t));
    item->name = mystrdup(name);
    if(cmd)
    {
        item->edit = mycalloc(1, sizeof(vfd_menu_edit_t));
        item->edit->cmd = mystrdup(cmd);
    }
    return item;
}

static vfd_menu_item_t* vfd_menu_last(vfd_menu_item_t *item)
{
    while(item->next)
        item = item->next;
    return item;
}

static vfd_menu_item_t* vfd_menu_first(vfd_menu_item_t *item)
{
    while(item->prev)
        item = item->prev;
    return item;
}

static uint8_t vfd_menu_sz(vfd_menu_item_t *item)
{
    uint8_t j;
    item = vfd_menu_first(item);
    for(j = 0; item->next; j++)
        item = item->next;
    return j;
}

static uint8_t vfd_menu_index(vfd_menu_item_t *item)
{
    uint8_t j;
    vfd_menu_item_t *i1 = vfd_menu_first(item);
    for(j = 0; (i1 != item) && i1; j++)
        i1 = i1->next;
    return j;
}

static vfd_menu_item_t* vfd_menu_append(vfd_menu_item_t *i1, char *name, char *cmd)
{
    vfd_menu_item_t *item = vfd_menu_new_item(name, cmd);
    if(!i1)
        return item;
    vfd_menu_item_t *last = vfd_menu_last(i1);
    last->next = item;
    item->prev = last;
    item->parent = last->parent;
    return item;
}

static vfd_menu_item_t* vfd_menu_append_child(vfd_menu_item_t *i1, char *name, char *cmd)
{
    if(i1->child)
        return vfd_menu_append(i1->child, name, cmd);
    vfd_menu_item_t *item = vfd_menu_new_item(name, cmd);
    if(!i1)
        return item;
    i1->child = item;
    item->parent = i1;
    return item;
}

static uint16_t vfd_menu_item_data_get(char *buf, uint16_t sz, vfd_menu_item_t *item)
{
#ifdef ENABLE_PCL
    char buf1[IO_BUF_SZ];
    mysnprintf(buf1, sz, "%s", item->edit->cmd);
    pcl_exec(buf1);
    if((buf1[0] == '[') && (buf1[1] == '0'))
        return mysnprintf(buf, sz, "%s", &buf1[4]);
    else
        return mysnprintf(buf, sz, "%s", "N/A");
#else
    return mysnprintf(buf, sz, "%s", "N/A");
#endif
}

static void vfd_menu_item_data_set(vfd_menu_item_t* item)
{
#ifdef ENABLE_PCL
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

static uint16_t vfd_menu_make_edit_int(vfd_menu_item_t* item, int v1, int v2, int step, uint16_t flags)
{
    vfd_menu_edit_int_t *d_i = &(item->edit->data.d_i);
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

    pstate = mycalloc(1, sizeof(vfd_menu_state_t));
    pmain = vfd_menu_append(pmain, "Control", 0);
    pstate->sel = pmain;
    pstate->scroll = pmain;
    vfd_menu_append(pmain, "Monitor", 0);
    vfd_menu_item_t *psys = vfd_menu_append(pmain, "System", 0);
#ifdef ENABLE_ETH
    vfd_menu_append_child(psys, "IP address", "sys ipaddr");
#endif
    vfd_menu_append_child(psys, "SW version", "sys date");
    vfd_menu_append_child(psys, "HW version", "sys hw");
    vfd_menu_item_t *item = vfd_menu_append_child(psys, "Brightness", "vfd brightness");
    vfd_menu_make_edit_int(item, 1, 8, 1, VFD_FLAG_IMMEDIATE);
    item = vfd_menu_append_child(psys, "Uptime", "sys uptime");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
#ifdef ENABLE_PTP
    vfd_menu_item_t *pptp = vfd_menu_append_child(psys, "PTP", 0);
    item = vfd_menu_append_child(pptp, "PTP time", "sys ptptime");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
#endif
    vfd_cls();
    vfd_cp866();
    vfd_brightness(1);
    vfd_menu_draw();
}

static void vfd_reverse(uint8_t r)
{
    char buf[16];
    mysnprintf(buf, sizeof(buf), "0x1F72%02X", r);
    vfd_str(buf);
}

#define VFD_LINE_SZ 16
void vfd_menu_line(char *buf, uint16_t num)
{
    uint16_t index = 0;
    if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 0)
    {
        vfd_menu_item_t *item = pstate->scroll;
        if(num == 0)
        {
            if(pstate->sel == item)
                buf[index++] = '>';
            else
                buf[index++] = ' ';
            index += mysnprintf(&buf[index], VFD_LINE_SZ-index, "%s", item->name);
        }
        if(num == 1)
        {
            item = item->next;
            if(item)
            {
                if(pstate->sel == item)
                    buf[index++] = '>';
                else
                    buf[index++] = ' ';
                index += mysnprintf(&buf[index], VFD_LINE_SZ-index, "%s", item->name);
            }
        }
    }
    else if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 1)
    {
        if(num == 0)
        {
            buf[index++] = '>';
            index += mysnprintf(&buf[index], VFD_LINE_SZ-index, "%s", pstate->sel->name);
        }
        if(num == 1)
        {
            buf[index++] = ' ';
            if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT_INT) == 1)
                index += mysnprintf(&buf[index], VFD_LINE_SZ, "%d", pstate->sel->edit->data.d_i.cur);
            else
                index += vfd_menu_item_data_get(&buf[index], VFD_LINE_SZ, pstate->sel);
        }
    }
    if(index > 0)
    {
        if(buf[index - 1] == 0)
            index--;
    }
    while(index < VFD_LINE_SZ)
        buf[index++] = ' ';
    buf[VFD_LINE_SZ] = 0;
    //dbg_send_int2(buf, num);
}

void vfd_menu_draw(void)
{
#if 0
    vfd_cls();
    if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 0)
    {
        vfd_menu_item_t *item = pstate->scroll;
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
            vfd_menu_item_t_data_set(pstate->sel);
        }
        vfd_str(pstate->sel->name);
        vfd_str("\r\n");
        char data[16];
        if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT_INT) == 1)
        {
            mysnprintf(data, sizeof(data), "%d", pstate->sel->edit->data.d_i.cur);
        }
        else
            vfd_menu_item_t_data_get(data, sizeof(data), pstate->sel);
        vfd_str(data);
    }
#else
    char buf[17];
    vfd_menu_line(buf, 0);
    vfd_home();
    vfd_str(buf);
    vfd_menu_line(buf, 1);
    vfd_home();
    vfd_crlf();
    vfd_str(buf);
#endif
}

static void vfd_menu_imm_upd(vfd_menu_item_t *item, uint8_t up)
{
    vfd_menu_edit_int_t *d_i = &(item->edit->data.d_i);
    if(up == 1)
    {
        d_i->cur += d_i->step;
        if(d_i->v2 < d_i->cur)
            d_i->cur = d_i->v2;
    }
    if(up == 0)
    {
        d_i->cur -= d_i->step;
        if(d_i->cur < d_i->v1)
            d_i->cur = d_i->v1;
    }
    vfd_menu_item_data_set(item);
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
            vfd_menu_imm_upd(pstate->sel, 0);
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
            vfd_menu_imm_upd(pstate->sel, 1);
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

void vfd_menu_ok(void)
{

}

void vfd_menu_tim_upd(void)
{
    if(pstate->sel->edit)
    {
        if(vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT) == 1)
        {
            if(vfd_menu_flag(pstate->sel, VFD_FLAG_TIM_UPD) == 1)
            {
                vfd_home();
                vfd_crlf();
                char buf[VFD_LINE_SZ + 1];
                vfd_menu_line(buf, 1);
                vfd_str(buf);
            }
        }
    }
}

