
#include "pcl_sys.h"
#include "util/util.h"
#include "util/version.h"
#include "util/heap1.h"

#ifdef ENABLE_FLASH
#include "flash/flash.h"
#endif

int32_t pcl_get_chunksz(uint8_t *ptr, int32_t fsz)
{
    if(fsz <= 0)
        return 0;
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

#ifdef ENABLE_FLASH
uint16_t pcl_load(picolInterp *i, uint32_t addr)
{
    uint8_t *ptr = (uint8_t*)addr;
    dbg_send_hex2("pcl_load.addr", (uint32_t)ptr);
    int32_t fsz = (int32_t)flash_fsz1();
    uint32_t len = 0;
    uint16_t rc = PICOL_OK;
    char buf[MAXSTR];
    if(!fsz)
        return rc;
    while(1)
    {
        dbg_send_int2("pcl_load.fsz", fsz);
        while((ptr[0] == 0x0A) && (fsz > 0))
        {
            ptr++;
            fsz--;
        }
        len = pcl_get_chunksz(ptr, fsz);
        dbg_send_int2("pcl_load.len", len);
        if(len == 0)
            break;
        if(len >= sizeof(buf))
        {
            dbg_send_int2("chunk too big", len);
            dbg_send_int2("fsz", fsz);
            dbg_send_int2("ptr", (int32_t)(ptr - addr));
            break;
        }
        mymemcpy(buf, ptr, len);
        buf[len] = 0;
        dbg_send_str3(buf, 1);
        //pcl_exec(buf);
        dbg_send_str3(buf, 1);
        rc = picolEval(i, buf);
        dbg_send_int2("pcl_load_eval_result", rc);
        if(rc != PICOL_OK)
        {
            dbg_send_str3("pcl_load error", 1);
            dbg_send_str3(i->result, 1);
            break;
        }
        ptr += len + 1;
        fsz -= len + 1;
        if(fsz <= 0)
            break;
    }
    dbg_send_int2("pcl_load.fsz", fsz);
    return rc;
}
COMMAND(reload) {
    return pcl_load(i, USER_FLASH_MID_ADDR);
}
#else
uint16_t pcl_load(picolInterp *i, uint32_t addr)
{
    return 0;
}
#endif

COMMAND(sys) {
    char buf[MAXSTR] = "";
    ARITY(argc >= 2 || argc == 3, "sys ram|version|...");
    if(SUBCMD1("ram"))
    {
        return picolSetIntResult(i, mymemory());
    }
    if(SUBCMD1("version"))
    {
        version(buf, sizeof(buf));
        return picolSetResult(i, buf);
    }
    if(SUBCMD1("date"))
    {
        version_date(buf, sizeof(buf));
        return picolSetResult(i, buf);
    }
    if(SUBCMD1("time"))
    {
        version_time(buf, sizeof(buf));
        return picolSetResult(i, buf);
    }
    if(SUBCMD1("hw"))
    {
        return picolSetIntResult(i, version_hw());
    }
    if(SUBCMD1("uptime"))
    {
        format_time(buf, sizeof(buf), uptime());
        return picolSetResult(i, buf);
    }
    return PICOL_ERR;
}

void pcl_sys_init(picolInterp *i)
{
#ifdef ENABLE_FLASH
    picolRegisterCmd(i, "reload", picol_reload, 0);
    //pcl_load(i, USER_FLASH_MID_ADDR);
#endif
    picolRegisterCmd(i, "sys", picol_sys, 0);
}

