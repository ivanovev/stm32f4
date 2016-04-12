
#ifdef ENABLE_LCD
#include "pcl_lcd.h"
#include "display/lcd/lcd.h"
#include "display/menu.h"

COMMAND(lcd) {
    char buf[MAXSTR] = "";
    char tmp[IO_BUF_SZ];
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
    if(SUBCMD1("line"))
    {
        ARITY(argc == 3, "vfd line index"); 
        menu_line(tmp, str2int(argv[2]));
        bytes2str(tmp, buf, 16);
        return picolSetResult(i, buf);
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

void pcl_lcd_init(picolInterp *i)
{
    picolRegisterCmd(i, "lcd", picol_lcd, 0);
}
#endif

