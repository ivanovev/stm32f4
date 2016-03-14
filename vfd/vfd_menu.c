
#include <main.h>
#include "vfd.h"
#include "vfd_menu.h"
#include "pcl/pcl.h"
#include "util/heap1.h"

vfd_menu_state_t   *pstate = NULL;
vfd_menu_item_t    *pmain = NULL;

#if 0
typedef struct vfd_words_t {
    char **en_ru;
    int len;
} vfd_words_t;
static vfd_words_t        *pwords = NULL;
#endif

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

#if 0
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
#endif

static vfd_menu_item_t* vfd_menu_append(vfd_menu_item_t *i1, char *name, char *cmd)
{
    vfd_menu_item_t *item = vfd_menu_new_item(name, cmd);
    if(!i1)
        return item;
    vfd_menu_item_t *last = i1;
    while(last->next)
        last = last->next;
    last->next = item;
    item->prev = last;
    item->parent = last->parent;
    return item;
}

vfd_menu_item_t* vfd_menu_append_child(vfd_menu_item_t *i1, char *name, char *cmd)
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

void vfd_menu_make_edit_int(vfd_menu_item_t* item, int v1, int v2, int step, uint16_t flags)
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

    vfd_menu_item_t *item, *psys, *pparent;
    pstate = mycalloc(1, sizeof(vfd_menu_state_t));
    pmain = vfd_menu_append(pmain, "Control", 0);
    pstate->sel = pmain;
    pstate->scroll = pmain;
    vfd_menu_append(pmain, "Monitor", 0);
    psys = vfd_menu_append(pmain, "System", 0);
#ifdef ENABLE_ETH
    pparent = vfd_menu_append_child(psys, "Ethernet", 0);
    vfd_menu_append_child(pparent, "IP address", "sys ipaddr");
    vfd_menu_append_child(pparent, "MAC address", "sys macaddr");
#ifdef ENABLE_PTP
    pparent = vfd_menu_append_child(psys, "PTP", 0);
    item = vfd_menu_append_child(pparent, "Time", "ptp time");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
    item = vfd_menu_append_child(pparent, "Offset", "ptp offset");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
    item = vfd_menu_append_child(pparent, "Clock id", "ptp clkid");
    item = vfd_menu_append_child(pparent, "Port id", "ptp portid");
    item = vfd_menu_append_child(pparent, "PPS", "ptp pps");
    vfd_menu_make_edit_int(item, 0, 15, 1, VFD_FLAG_IMMEDIATE);
#endif
#endif
    pparent = vfd_menu_append_child(psys, "Version", 0);
    vfd_menu_append_child(pparent, "SW", "sys date");
    vfd_menu_append_child(pparent, "HW", "sys hw");

    item = vfd_menu_append_child(psys, "Brightness", "vfd brightness");
    vfd_menu_make_edit_int(item, 1, 8, 1, VFD_FLAG_IMMEDIATE);
    item = vfd_menu_append_child(psys, "Uptime", "sys uptime");
    item->edit->flags |= VFD_FLAG_TIM_UPD;

    vfd_cls();
    vfd_cp866();
    vfd_brightness(1);
    vfd_menu_draw();
}

#if 0
static void vfd_reverse(uint8_t r)
{
    char buf[16];
    mysnprintf(buf, sizeof(buf), "0x1F72%02X", r);
    vfd_str(buf);
}
#endif

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
            if((vfd_menu_flag(pstate->sel, VFD_FLAG_EDIT_INT) == 1) && (vfd_menu_flag(pstate->sel, VFD_FLAG_TIM_UPD) == 0))
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
    dbg_send_str3("down", 1);
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
    dbg_send_str3("up", 1);
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
    dbg_send_str3("right", 1);
    if(pstate->sel->child)
    {
        dbg_send_str3(pstate->sel->name, 1);
        pstate->sel = pstate->sel->child;
        pstate->scroll = pstate->sel;
        dbg_send_str3(pstate->sel->name, 0);
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
    dbg_send_str3("left", 1);
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

__weak void vfd_menu_ok(void)
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

