
#include "pcl.h"
#include "pcl_misc.h"
#include "pcl_stm.h"
#include "pcl_sys.h"
#include "uart/uart.h"
#include "util/heap1.h"

static struct picolInterp *pcl_interp = 0;

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
        picolRegisterCmd(pcl_interp, "clear", picol_clear, 0);
        dbg_send_str3("pcl_init", 1);
    }
}

uint16_t pcl_exec(char *buf)
{
    strip_str(buf);
    int retcode = picolEval(pcl_interp, buf);
    return mysnprintf(buf, IO_BUF_SZ, "[%d] %s", retcode, pcl_interp->result);
}

void pcl_io(void)
{
    char buf[IO_BUF_SZ];
    uint16_t sz = io_recv_str(buf);
    if(sz)
    {
        if(!myisempty(buf))
        {
            pcl_exec(buf);
            io_send_str4(buf);
            io_prompt(1);
        }
        else
            io_prompt(0);
    }
}

