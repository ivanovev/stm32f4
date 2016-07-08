
#include <main.h>
#include "display/menu.h"

COMMAND(menu) {
    char buf[MAXSTR] = "";
    char tmp[IO_BUF_SZ];
    ARITY(argc >= 2 || argc == 3, "menu up|down|left|right...");
    if(SUBCMD1("line"))
    {
        ARITY(argc == 3, "lcd line index"); 
        menu_line(tmp, str2int(argv[2]));
        bytes2str(tmp, buf, 16);
        return picolSetResult(i, buf);
    }
    if(SUBCMD1("home"))
    {
        menu_home(1);
        return PICOL_OK;
    }
    if(SUBCMD1("draw"))
    {
        menu_draw();
        return PICOL_OK;
    }
    if(SUBCMD1("down"))
    {
        menu_down();
        return PICOL_OK;
    }
    if(SUBCMD1("up"))
    {
        menu_up();
        return PICOL_OK;
    }
    if(SUBCMD1("left"))
    {
        menu_left();
        return PICOL_OK;
    }
    if(SUBCMD1("right"))
    {
        menu_right();
        return PICOL_OK;
    }
    return PICOL_ERR;
}

#ifdef ENABLE_LCD
#include "display/lcd/lcd.h"

COMMAND(lcd) {
    ARITY(argc >= 2 || argc == 3, "lcd cls|... ...");
    if(SUBCMD1("crlf"))
    {
        lcd_crlf();
        return PICOL_OK;
    }
    if(SUBCMD1("cls"))
    {
        lcd_cls();
        return PICOL_OK;
    }
    if(SUBCMD1("home"))
    {
        lcd_home();
        return PICOL_OK;
    }
    if(SUBCMD1("bl"))
    {
        if(argc > 2)
        {
            lcd_bl(argv[2]);
            return picolSetResult(i, argv[2]);
        }
        return picolSetIntResult(i, 0);
    }
    return PICOL_ERR;
}
#endif

#ifdef ENABLE_VFD
#include "display/vfd/vfd.h"

COMMAND(vfd) {
    char buf[MAXSTR] = "";
    char tmp[IO_BUF_SZ];
    ARITY(argc >= 2 || argc == 3, "vfd cls|... ...");
    if(SUBCMD1("crlf"))
    {
        vfd_crlf();
        return PICOL_OK;
    }
    if(SUBCMD1("cls"))
    {
        vfd_cls();
        return PICOL_OK;
    }
    if(SUBCMD1("home"))
    {
        vfd_home();
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
    return PICOL_ERR;
}
#endif

void pcl_display_init(picolInterp *i)
{
    picolRegisterCmd(i, "menu", picol_menu, 0);
#ifdef ENABLE_LCD
    picolRegisterCmd(i, "lcd", picol_lcd, 0);
#endif
#ifdef ENABLE_VFD
    picolRegisterCmd(i, "vfd", picol_vfd, 0);
#endif
}

