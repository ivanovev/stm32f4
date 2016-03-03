
#include <main.h>

#ifdef ENABLE_VFD
#include "vfd/vfd_menu.h"

extern vfd_menu_state_t   *pstate;
extern vfd_menu_item_t    *pmain;

void lnah_init()
{
    vfd_menu_item_t *item = vfd_menu_append_child(pmain, "Out Enable", "enable");
    vfd_menu_make_edit_int(item, 0, 1, 1, VFD_FLAG_IMMEDIATE);
    item = vfd_menu_append_child(pmain->next, "Voltage, V", "udc");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
    item = vfd_menu_append_child(pmain->next, "Current, A", "idc");
    item->edit->flags |= VFD_FLAG_TIM_UPD;
}

#else
void lnah_init()
{
}
#endif

