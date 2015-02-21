
#include <main.h>
#include <cptr.h>

#ifdef ENABLE_PCL
COMMAND(wifi) {
    ARITY(argc >= 2, "wifi subcmd");
    if(SUBCMD1("reset"))
        cptr_wifi_reset();
    if(SUBCMD1("reload"))
        cptr_wifi_reload();
    return PICOL_OK;
}

void pcl_wifi_init(struct picolInterp *pcl_interp)
{
    picolRegisterCmd(pcl_interp, "wifi", picol_wifi, 0);
}
#endif
