
#include "pcl_sys.h"
#include "util/util.h"

#ifdef MY_ETH
#include "eth/eth.h"
#include "eth/myip/mytcp.h"
extern TCP_CON tcp_con;
extern volatile uint8_t reset;
#endif

#ifdef MY_I2C
#include "i2c/eeprom.h"
#endif

#ifdef MY_ETH
COMMAND(exit) {
    tcp_con.state = TCP_CON_CLOSE;
    return picolSetIntResult(i, 0);
}
COMMAND(reset) {
    tcp_con.state = TCP_CON_CLOSE;
    reset = RESET_REBOOT;
    return picolSetIntResult(i, 0);
}
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

#ifdef MY_FLASH
uint16_t pcl_load(picolInterp *i, uint32_t addr)
{
    uint8_t *ptr = addr;
    dbg_send_hex2("pcl_load.addr", (uint32_t)ptr);
    int32_t fsz = (int32_t)flash_fsz1();
    uint32_t len = 0;
    uint16_t rc = PICOL_OK;
    uint8_t buf[MAXSTR];
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
            dbg_send_int2("ptr", ptr - addr);
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

#ifdef MY_ETH
extern uint32_t local_ip_addr;
#endif

#ifdef MY_FLASH
#include "flash/flash.h"
#endif

COMMAND(sys) {
    char buf[MAXSTR] = "";
    ARITY(argc >= 2 || argc == 3, "sys ram|version|ipaddr|... ...");
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
    if(SUBCMD1("ipaddr"))
    {
        uint32_t ipaddr = 0;
        if(argc == 2)
        {
#ifdef MY_I2C
            uint32_t ipaddr = eeprom_ipaddr_read();
#else
            uint32_t ipaddr = LOCAL_IP_ADDR;
#endif
            uint8_t *ipaddr8 = (uint8_t*)&ipaddr;
            mysnprintf(buf, sizeof(buf), "%d.%d.%d.%d", ipaddr8[3], ipaddr8[2], ipaddr8[1], ipaddr8[0]);
            return picolSetResult(i, buf);
        }
        if(argc == 3)
        {
            //ipaddr = 
            uint8_t j, k, ii;
            uint32_t tmp[4];
            char *a = argv[2];
            for(j = 0, k = 0, ii = 3; j <= 16; j++)
            {
                if(a[j] == 0)
                {
                    ipaddr |= (str2int(&(a[k])) << (8*ii--));
                    break;
                }
                if(a[j] == '.')
                {
                    ipaddr |= (str2int(&(a[k])) << (8*ii--));
                    k = j + 1;
                }
            }
#ifdef MY_I2C
            eeprom_ipaddr_write(ipaddr);
#endif
            return picolSetHex4Result(i, ipaddr);
        }
    }
    if(SUBCMD1("uptime"))
    {
        format_time(buf, sizeof(buf), HAL_GetTick()/1000);
        return picolSetResult(i, buf);
    }
#ifdef ENABLE_PTP
    if(SUBCMD1("ptptime"))
    {
        format_time(buf, sizeof(buf), eth_ptpclk_seconds());
        return picolSetResult(i, buf);
    }
#endif
    return PICOL_ERR;
}

void pcl_sys_init(picolInterp *i)
{
#ifdef MY_ETH
    picolRegisterCmd(i, "exit", picol_exit, 0);
    picolRegisterCmd(i, "reset", picol_reset, 0);
    picolRegisterCmd(i, "reload", picol_reload, 0);
#endif
#ifdef MY_FLASH
    //pcl_load(i, USER_FLASH_MID_ADDR);
#endif
    picolRegisterCmd(i, "sys", picol_sys, 0);
}

