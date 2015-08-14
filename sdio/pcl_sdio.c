
#ifdef ENABLE_PCL

#include <main.h>
#ifdef ENABLE_PCL
#include "sdio/pcl_sdio.h"
#endif

COMMAND(sdio) {
    ARITY(argc >= 2, "sdio cmd..");
    uint32_t v = 0;
    char buf[IO_BUF_SZ];
    buf[0] = 0;
    if(SUBCMD1("init"))
    {
        sdio_init();
        return PICOL_OK;
    }
    if(SUBCMD1("cmd"))
    {
        v = str2int(argv[2]);
        return picolSetIntResult(i, v);
    }
    return picolSetResult(i, buf);
}

void pcl_sdio_init(picolInterp *i)
{
    picolRegisterCmd(i, "sdio", picol_sdio, 0);
}

#endif

