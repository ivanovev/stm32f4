
#include <main.h>
#include "menu.h"
#include "pcl/pcl.h"
#include "util/heap1.h"
#include "display.h"

menu_state_t   *pstate = NULL;
menu_item_t    *pmain = NULL;

#if 0
typedef struct vfd_words_t {
    char **en_ru;
    int len;
} vfd_words_t;
static vfd_words_t        *pwords = NULL;
#endif

static uint16_t menu_flag(menu_item_t *item, uint16_t flag)
{
    if(item->edit)
        return (item->edit->flags & flag) ? 1 : 0;
    return 0;
}

static menu_item_t* menu_new_item(char *name, char *cmd)
{
    menu_item_t *item = mycalloc(1, sizeof(menu_item_t));
    item->name = mystrdup(name);
    if(cmd)
    {
        item->edit = mycalloc(1, sizeof(menu_edit_t));
        item->edit->cmd = mystrdup(cmd);
    }
    return item;
}

#if 0
static menu_item_t* menu_last(menu_item_t *item)
{
    while(item->next)
        item = item->next;
    return item;
}

static menu_item_t* menu_first(menu_item_t *item)
{
    while(item->prev)
        item = item->prev;
    return item;
}

static uint8_t menu_sz(menu_item_t *item)
{
    uint8_t j;
    item = menu_first(item);
    for(j = 0; item->next; j++)
        item = item->next;
    return j;
}

static uint8_t menu_index(menu_item_t *item)
{
    uint8_t j;
    menu_item_t *i1 = menu_first(item);
    for(j = 0; (i1 != item) && i1; j++)
        i1 = i1->next;
    return j;
}
#endif

static menu_item_t* menu_append(menu_item_t *i1, char *name, char *cmd)
{
    menu_item_t *item = menu_new_item(name, cmd);
    if(!i1)
        return item;
    menu_item_t *last = i1;
    while(last->next)
        last = last->next;
    last->next = item;
    item->prev = last;
    item->parent = last->parent;
    return item;
}

menu_item_t* menu_append_child(menu_item_t *i1, char *name, char *cmd)
{
    if(i1->child)
        return menu_append(i1->child, name, cmd);
    menu_item_t *item = menu_new_item(name, cmd);
    if(!i1)
        return item;
    i1->child = item;
    item->parent = i1;
    return item;
}

static uint16_t menu_item_data_get(char *buf, uint16_t sz, menu_item_t *item)
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

static void menu_item_data_set(menu_item_t* item)
{
#ifdef ENABLE_PCL
    char buf[IO_BUF_SZ];
    buf[0] = 0;
    if(menu_flag(item, DISPLAY_FLAG_EDIT_INT) == 1)
    {
        mysnprintf(buf, sizeof(buf), "%s %d", item->edit->cmd, item->edit->data.d_i.cur);
    }
    if(buf[0])
        pcl_exec(buf);
#endif
}

void menu_make_edit_int(menu_item_t* item, int v1, int v2, int step, uint16_t flags)
{
    menu_edit_int_t *d_i = &(item->edit->data.d_i);
    item->edit->flags = flags | DISPLAY_FLAG_EDIT_INT;
    d_i->v1 = v1;
    d_i->v2 = v2;
    d_i->step = step;
    char data[16];
    menu_item_data_get(data, sizeof(data), item);
    d_i->cur = str2int(data);
    if(d_i->cur < v1)
        d_i->cur = v1;
    if(d_i->cur > v2)
        d_i->cur = v2;
}

void menu_init(void)
{
    if(pmain)
        return;

    menu_item_t *item, *psys, *pparent;
    pstate = mycalloc(1, sizeof(menu_state_t));
    pmain = menu_append(pmain, "Control", 0);
    pstate->sel = pmain;
    pstate->scroll = pmain;
    menu_append(pmain, "Monitor", 0);
    psys = menu_append(pmain, "System", 0);
#ifdef ENABLE_ETH
    pparent = menu_append_child(psys, "Ethernet", 0);
    menu_append_child(pparent, "IP address", "eth ipaddr");
    menu_append_child(pparent, "MAC address", "eth macaddr -");
#ifdef ENABLE_PTP
    pparent = menu_append_child(psys, "PTP", 0);
    item = menu_append_child(pparent, "Time", "ptp time");
    item->edit->flags |= DISPLAY_FLAG_TIM_UPD;
    item = menu_append_child(pparent, "Offset", "ptp offset");
    item->edit->flags |= DISPLAY_FLAG_TIM_UPD;
    item = menu_append_child(pparent, "Clock id", "ptp clkid");
    item = menu_append_child(pparent, "Port id", "ptp portid");
    item = menu_append_child(pparent, "PPS", "ptp pps");
    menu_make_edit_int(item, 0, 15, 1, DISPLAY_FLAG_IMMEDIATE);
#endif
#endif
    pparent = menu_append_child(psys, "Version", 0);
    menu_append_child(pparent, "SW", "sys date");
    menu_append_child(pparent, "HW", "sys hw");

#ifdef ENABLE_VFD
    item = menu_append_child(psys, "Brightness", "vfd brightness");
    menu_make_edit_int(item, 1, 8, 1, DISPLAY_FLAG_IMMEDIATE);
#endif
#ifdef ENABLE_LCD
    item = menu_append_child(psys, "Backlight", "lcd bl");
    menu_make_edit_int(item, 0, 1, 1, DISPLAY_FLAG_IMMEDIATE);
#endif
    item = menu_append_child(psys, "Uptime", "sys uptime");
    item->edit->flags |= DISPLAY_FLAG_TIM_UPD;

    display_cls();
#ifdef ENABLE_VFD
    vfd_cp866();
    vfd_brightness(1);
#endif
    menu_draw();
}

#if 0
static void vfd_reverse(uint8_t r)
{
    char buf[16];
    mysnprintf(buf, sizeof(buf), "0x1F72%02X", r);
    vfd_str(buf);
}
#endif

#define DISPLAY_LINE_SZ 16
void menu_line(char *buf, uint16_t num)
{
    uint16_t index = 0;
    if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT) == 0)
    {
        menu_item_t *item = pstate->scroll;
        if(num == 0)
        {
            if(pstate->sel == item)
                buf[index++] = '>';
            else
                buf[index++] = ' ';
            index += mysnprintf(&buf[index], DISPLAY_LINE_SZ-index, "%s", item->name);
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
                index += mysnprintf(&buf[index], DISPLAY_LINE_SZ-index, "%s", item->name);
            }
        }
    }
    else if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT) == 1)
    {
        if(num == 0)
        {
            buf[index++] = '>';
            index += mysnprintf(&buf[index], DISPLAY_LINE_SZ-index, "%s", pstate->sel->name);
        }
        if(num == 1)
        {
            buf[index++] = ' ';
            if((menu_flag(pstate->sel, DISPLAY_FLAG_EDIT_INT) == 1) && (menu_flag(pstate->sel, DISPLAY_FLAG_TIM_UPD) == 0))
                index += mysnprintf(&buf[index], DISPLAY_LINE_SZ, "%d", pstate->sel->edit->data.d_i.cur);
            else
                index += menu_item_data_get(&buf[index], DISPLAY_LINE_SZ, pstate->sel);
        }
    }
    if(index > 0)
    {
        if(buf[index - 1] == 0)
            index--;
    }
    while(index < DISPLAY_LINE_SZ)
        buf[index++] = ' ';
    buf[DISPLAY_LINE_SZ] = 0;
    //dbg_send_int2(buf, num);
}

void menu_draw(void)
{
#if 0
    vfd_cls();
    if(menu_flag(pstate->sel, VFD_FLAG_EDIT) == 0)
    {
        menu_item_t *item = pstate->scroll;
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
    if(menu_flag(pstate->sel, VFD_FLAG_EDIT) == 1)
    {
        if(menu_flag(pstate->sel, VFD_FLAG_IMMEDIATE) == 1)
        {
            menu_item_t_data_set(pstate->sel);
        }
        vfd_str(pstate->sel->name);
        vfd_str("\r\n");
        char data[16];
        if(menu_flag(pstate->sel, VFD_FLAG_EDIT_INT) == 1)
        {
            mysnprintf(data, sizeof(data), "%d", pstate->sel->edit->data.d_i.cur);
        }
        else
            menu_item_t_data_get(data, sizeof(data), pstate->sel);
        vfd_str(data);
    }
#else
    char buf[17];
    menu_line(buf, 0);
    display_home();
    display_str(buf);
    menu_line(buf, 1);
#ifdef ENABLE_VFD
    display_home();
#endif
    display_crlf();
    display_str(buf);
#endif
}

static void menu_imm_upd(menu_item_t *item, uint8_t up)
{
    menu_edit_int_t *d_i = &(item->edit->data.d_i);
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
    menu_item_data_set(item);
}

void menu_down(void)
{
    dbg_send_str3("down", 1);
    if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT) == 0)
    {
        if(pstate->sel->next)
        {
            pstate->sel = pstate->sel->next;
            if(pstate->scroll != pstate->sel->prev)
                pstate->scroll = pstate->sel->prev;
            menu_draw();
        }
    }
    if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT) == 1)
    {
        if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT_INT) == 1)
        {
            menu_imm_upd(pstate->sel, 0);
            menu_draw();
        }
    }
}

void menu_up(void)
{
    dbg_send_str3("up", 1);
    if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT) == 0)
    {
        if(pstate->sel->prev)
        {
            pstate->sel = pstate->sel->prev;
            if(pstate->scroll != pstate->sel)
                pstate->scroll = pstate->sel;
            menu_draw();
        }
    }
    if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT) == 1)
    {
        if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT_INT) == 1)
        {
            menu_imm_upd(pstate->sel, 1);
            menu_draw();
        }
    }
}

void menu_right(void)
{
    dbg_send_str3("right", 1);
    if(pstate->sel->child)
    {
        dbg_send_str3(pstate->sel->name, 1);
        pstate->sel = pstate->sel->child;
        pstate->scroll = pstate->sel;
        dbg_send_str3(pstate->sel->name, 0);
        menu_draw();
        return;
    }
    if(pstate->sel->edit)
    {
        if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT) == 0)
        {
            pstate->sel->edit->flags |= DISPLAY_FLAG_EDIT;
            menu_draw();
            return;
        }
    }
}

void menu_left(void)
{
    dbg_send_str3("left", 1);
    if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT) == 1)
    {
        pstate->sel->edit->flags &= ~DISPLAY_FLAG_EDIT;
        menu_draw();
        return;
    }
    if(pstate->sel->parent)
    {
        pstate->sel = pstate->sel->parent;
        pstate->scroll = pstate->sel;
        if((pstate->scroll->next == 0) && pstate->scroll->prev)
            pstate->scroll = pstate->scroll->prev;
        menu_draw();
        return;
    }
}

__weak void menu_ok(void)
{

}

void menu_tim_upd(void)
{
    static uint16_t display_tim_counter = 0;
    if(pstate->sel->edit)
    {
        if(menu_flag(pstate->sel, DISPLAY_FLAG_EDIT) == 1)
        {
            if(menu_flag(pstate->sel, DISPLAY_FLAG_TIM_UPD) == 1)
            {
                display_home();
                display_crlf();
                char buf[DISPLAY_LINE_SZ + 1];
                menu_line(buf, 1);
                display_str(buf);
            }
        }
    }
#ifdef ENABLE_VFD
    display_tim_counter++;
    if(vfd_tim_counter > 100)
    {
        vfd_brightness(2);
        vfd_tim_counter = 0;
    }
#endif
}

