
#ifdef ENABLE_PCL

#include <main.h>
#ifdef ENABLE_PCL
#include "sdio/pcl_sdio.h"
#endif

COMMAND(sdio) {
    ARITY(argc >= 2, "sdio cmd..");
    uint32_t cmd = 0, v, *ptr;
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

