
#include "pcl.h"
#include "pcl_misc.h"
#include "pcl_stm32f4.h"

static struct picolInterp *pcl_interp = 0;

void pcl_init(void)
{
    pcl_interp = picolCreateInterp();
    register_misc_cmds(pcl_interp);
    register_stm32f4_cmds(pcl_interp);
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
        if(!myisempty(buf[0]))
        {
            pcl_exec(buf);
            io_send_str4(buf);
            io_prompt(1);
        }
        else
            io_prompt(0);
    }
}

