
#ifdef ENABLE_PCL

#include <main.h>
#ifdef ENABLE_PCL
#include "sdio/pcl_sdio.h"
#endif

COMMAND(sdio) {
    ARITY(argc >= 2, "sdio cmd..");
    uint32_t cmd = 0, v, *ptr;
    uint8_t n1 = 0, n2 = 0;
    char buf[IO_BUF_SZ];
    buf[0] = 0;
    ptr = (uint32_t*)sdio_get_reg_ptr(argv[1]);
    if(SUBCMD1("cmd") && (argc != 2))
        ptr = 0;
    if(ptr)
    {
        if(SUBCMD1("icr"))
        {
            *ptr = 0xFFFFFFFF;
            return picolSetResult(i, argv[1]);
        }
        if(argc == 2)
            return picolSetHex4Result(i, *ptr);
        if(argc >= 3)
        {
            *ptr = str2int(argv[2]);
            return picolSetResult(i, argv[2]);
        }
    }
    if(SUBCMD1("widbus"))
    {
        if(argc == 2)
        {
            v = sdio_get_reg_bits("clkcr", 11, 12);
            if(v == 2) v = 8;
            else if(v == 1) v = 4;
            else if (v == 0) v = 1;
            return picolSetIntResult(i, v);
        }
        if(argc >= 3)
        {
            v = str2int(argv[2]);
            if(v == 8) v = 2;
            else if(v == 4) v = 1;
            else if(v == 1) v = 0;
            v = sdio_set_reg_bits("clkcr", 11, 12, v);
            return picolSetHex4Result(i, v);
        }
    }
    if(SUBCMD1("dten"))
    {
        n1 = 0; n2 = 0; mystrncpy(buf, "dctrl", 5);
    }
    if(SUBCMD1("dtdir"))
    {
        n1 = 1; n2 = 1; mystrncpy(buf, "dctrl", 5);
    }
    if(SUBCMD1("dtmode"))
    {
        n1 = 2; n2 = 2; mystrncpy(buf, "dctrl", 5);
    }
    if(buf[0])
    {
        if(argc == 2)
            return picolSetIntResult(i, sdio_get_reg_bits(buf, n1, n2));
        if(argc == 3)
            return picolSetHex4Result(i, sdio_set_reg_bits(buf, n1, n2, str2int(argv[2])));
    }
    if(SUBCMD1("rx"))
    {
        if(SUBCMD2("start"))
        {
            v = 32;
            if(argc >= 4)
                v = str2int(argv[3]);
            sdio_rx_start(v);
            return PICOL_OK;
        }
    }
    if(SUBCMD1("init"))
    {
        sdio_init();
        return PICOL_OK;
    }
    if(SUBCMD1("cmd") && (argc >= 3))
    {
        cmd = str2int(argv[2]);
        if(argc >= 4)
            v = str2int(argv[3]);
        else
            v = 0;
        cmd = sdio_cmd(cmd, &v);
        if(cmd == SD_OK)
            return picolSetHex4Result(i, v);
        return PICOL_ERR;
    }
    return picolSetResult(i, buf);
}

void pcl_sdio_init(picolInterp *i)
{
    picolRegisterCmd(i, "sdio", picol_sdio, 0);
}

#endif

