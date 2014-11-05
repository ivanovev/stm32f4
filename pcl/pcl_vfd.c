
#include "pcl_vfd.h"
#include "util/util.h"

#ifdef MY_VFD
COMMAND(vfd) {
    char buf[MAXSTR] = "";
    ARITY(argc >= 2 || argc == 3, "vfd cls|... ...");
    if(SUBCMD1("cls"))
    {
        vfd_cls();
        return PICOL_OK;
    }
    if(SUBCMD1("str"))
    {
        vfd_str(argv[2]);
        return PICOL_OK;
    }
    if(SUBCMD1("reset"))
    {
        vfd_reset();
        return PICOL_OK;
    }
    if(SUBCMD1("brightness"))
    {
        return picolSetIntResult(i, vfd_brightness((argc == 3) ? str2int(argv[2]) : 0));
    }
    if(SUBCMD1("draw"))
    {
        vfd_menu_draw();
        return PICOL_OK;
    }
    if(SUBCMD1("down"))
    {
        vfd_menu_down();
        return PICOL_OK;
    }
    if(SUBCMD1("up"))
    {
        vfd_menu_up();
        return PICOL_OK;
    }
    return PICOL_ERR;
}
#endif

void pcl_vfd_init(picolInterp *i)
{
#ifdef MY_VFD
    picolRegisterCmd(i, "vfd", picol_vfd, 0);
#endif
}

