
#include "pcl.h"
#include "pcl_misc.h"
#include "pcl_stm.h"
#include "pcl_eth.h"
#include "pcl_vfd.h"
#include "pcl_sys.h"
#include "util/heap1.h"

struct picolInterp *pcl_interp = 0;
static char pcl_prefix[32];

void pcl_clear(void)
{
    myfree(pcl_interp);
    pcl_interp = 0;
    pcl_init();
}

COMMAND(clear) {
    if(i->level == 0)
        pcl_clear();
    return PICOL_OK;
}

COMMAND(prefix) {
    if(argc == 2)
        mystrncpy(pcl_prefix, argv[1], mystrnlen(argv[1], sizeof(pcl_prefix)));
    else
        pcl_prefix[0] = 0;
    return picolSetResult(i, pcl_prefix);
}

void pcl_init(void)
{
    if(!pcl_interp)
    {
        pcl_interp = picolCreateInterp();
        pcl_misc_init(pcl_interp);
        pcl_stm_init(pcl_interp);
        pcl_sys_init(pcl_interp);
#ifdef ENABLE_VFD
        pcl_vfd_init(pcl_interp);
#endif
#ifdef ENABLE_ETH
        pcl_eth_init(pcl_interp);
#endif
        picolRegisterCmd(pcl_interp, "clear", picol_clear, 0);
        picolRegisterCmd(pcl_interp, "prefix", picol_prefix, 0);
        dbg_send_str3("pcl_init", 1);
        pcl_prefix[0] = 0;
    }
}

struct picolInterp* pcl_get_interp(void)
{
    return pcl_interp;
}

uint16_t pcl_exec(char *buf)
{
    strip_str(buf);
    uint16_t retcode = picolEval(pcl_interp, buf);
    if(retcode != PICOL_WAIT)
        mysnprintf(buf, IO_BUF_SZ, "[%d] %s", retcode, pcl_interp->result);
    return retcode;
}

#define WAIT_TIMEOUT 1000

void pcl_io(void)
{
    static char buf[IO_BUF_SZ];
    char buf2[IO_BUF_SZ];
    uint16_t sz = io_recv_str(buf);
    uint16_t rc = PICOL_WAIT;
    uint32_t wait = pcl_interp->wait;
    if(sz)
        wait = 0;
    if(sz)
    {
        if(!myisempty(buf))
        {
            //dbg_send_str3("pcl_exec", 1);
            if(pcl_prefix[0] && mystrncmp(buf, "prefix", 6))
            {
                buf2[0] = 0;
                mystrncat(buf2, pcl_prefix, sizeof(pcl_prefix));
                mystrncat(buf2, " ", 1);
                mystrncat(buf2, buf, sz - sizeof(pcl_prefix));
                mystrncpy(buf, buf2, mystrnlen(buf2, IO_BUF_SZ));
            }
            rc = pcl_exec(buf);
        }
        else
        {
            io_prompt(0, pcl_prefix);
            wait = 0;
            return;
        }
    }
    else if(wait)
    {
        if(HAL_GetTick() < (wait + WAIT_TIMEOUT))
        {
            rc = pcl_exec(buf);
        }
        else
        {
            mysnprintf(buf, IO_BUF_SZ, "[%d] %s", rc, pcl_interp->result);
            rc = PICOL_ERR;
        }
    }
    else
        return;
    if(rc != PICOL_WAIT)
    {
        io_send_str4(buf);
        io_prompt(1, pcl_prefix);
        pcl_interp->wait = 0;
    }
    else if(wait == 0)
    {
        pcl_interp->wait = HAL_GetTick();
    }
}

