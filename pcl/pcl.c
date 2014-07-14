
#include "main.h"

static struct picolInterp *pcl_interp = 0;

void pcl_init(void)
{
    pcl_interp = picolCreateInterp();
    //register_stm32f4_cmds(pcl_interp);
    register_misc_cmds(pcl_interp);
}

uint32_t pcl_exec(char *buf)
{
    strip_str(buf);
    int retcode = picolEval(pcl_interp, buf);
    return mysnprintf(buf, IO_BUF_SZ, "[%d] %s", retcode, pcl_interp->result);
}

