
#include "pcl.h"
#include "pcl_misc.h"
#include "pcl_stm32f4.h"
#include "pcl_sys.h"
#include "uart/uart.h"

static struct picolInterp *pcl_interp = 0;

void pcl_init(void)
{
    pcl_interp = picolCreateInterp();
    register_misc_cmds(pcl_interp);
    register_stm32f4_cmds(pcl_interp);
    register_sys_cmds(pcl_interp);
    dbg_send_str3("pcl_init", 1);
    pcl_load();

}

int32_t pcl_get_chunksz(uint8_t *ptr, int32_t fsz)
{
    uint8_t *ptr1 = mymemchr(ptr, 0x0A, fsz);
    //print2("ptr1[0]", ptr1[0]);
    //print2("ptr1[1]", ptr1[1]);
    if(!ptr1)
        return 0;
    uint32_t len1 = ptr1 - ptr;
    if(ptr1[1] == 0x0A)
        return len1;
    len1 += 1;
    ptr += len1;
    fsz -= len1;
    if(fsz <= 0)
        return len1;
    len1 += pcl_get_chunksz(ptr, fsz);
    return len1;
}

#ifdef MY_FLASH
uint16_t pcl_load(void)
{
    uint8_t *ptr = USER_FLASH_MID_ADDR;
    dbg_send_hex2("pcl_load.addr", (uint32_t)ptr);
    uint32_t fsz = flash_fsz1();
    uint32_t len = 0;
    uint8_t buf[MAXSTR];
    if(!fsz)
        return;
    while(1)
    {
        while((ptr[0] == 0x0A) && (fsz > 0))
        {
            ptr++;
            fsz--;
        }
        len = pcl_get_chunksz(ptr, fsz);
        if(len == 0)
            break;
        if(len >= sizeof(buf))
        {
            dbg_send_int2("chunk too big", len);
            dbg_send_int2("fsz", fsz);
            dbg_send_int2("ptr", ptr - USER_FLASH_MID_ADDR);
            break;
        }
        dbg_send_int2("pcl_load.len", len);
        mymemcpy(buf, ptr, len);
        buf[len] = 0;
        dbg_send_str3(buf, 1);
        pcl_exec(buf);
        dbg_send_str3(buf, 1);
        ptr += len + 1;
        fsz -= len + 1;
    }
    dbg_send_int2("pcl_load.fsz", fsz);
    return 0;
}
#else
uint16_t pcl_load(void)
{
    return 0;
}
#endif

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

