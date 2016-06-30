
#include "pcl.h"
#include "pcl_math.h"
#include "pcl_misc.h"
#include "pcl_math.h"
#include "pcl_stm.h"
#include "pcl_eth.h"
#include "pcl_lcd.h"
#include "pcl_vfd.h"
#include "pcl_sys.h"
#include "util/heap1.h"

struct picolInterp *pcl_interp = 0;
static char pcl_prefix[32];
volatile uint8_t main_evt;

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

static volatile uint32_t g_timer_dt = 0;
COMMAND(timer) {
    if(argc == 1)
        return picolSetIntResult(i, g_timer_dt);
    if(argc == 2)
        g_timer_dt = str2int(argv[1]);
    return PICOL_OK;
}


__weak void pcl_extra_init(picolInterp *i)
{
}

#ifdef ENABLE_FLASH
extern uint16_t pcl_load(picolInterp *i, uint32_t addr);
#endif

void pcl_init(void)
{
    if(!pcl_interp)
    {
        pcl_interp = picolCreateInterp();
        pcl_math_init(pcl_interp);
        pcl_misc_init(pcl_interp);
        pcl_math_init(pcl_interp);
        pcl_stm_init(pcl_interp);
        pcl_sys_init(pcl_interp);
#ifdef ENABLE_LCD
        pcl_lcd_init(pcl_interp);
#endif
#ifdef ENABLE_VFD
        pcl_vfd_init(pcl_interp);
#endif
#ifdef ENABLE_ETH
        pcl_eth_init(pcl_interp);
#endif
        pcl_extra_init(pcl_interp);
        picolRegisterCmd(pcl_interp, "clear", picol_clear, 0);
        picolRegisterCmd(pcl_interp, "prefix", picol_prefix, 0);
        picolRegisterCmd(pcl_interp, "timer", picol_timer, 0);
        dbg_send_str3("pcl_init", 1);
        pcl_prefix[0] = 0;
    }
#ifdef ENABLE_FLASH
    pcl_load(pcl_interp, USER_FLASH_MID_ADDR);
    pcl_exec("boot_cb");
#endif
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

void pcl_try_timer_cb(void)
{
    if(!g_timer_dt)
        return;
    uint32_t tnow = 0;
    static uint32_t timestamp = 0;
    tnow = uptime();
    if((tnow >= timestamp) ? ((tnow - timestamp) < g_timer_dt) : (tnow < g_timer_dt))
        return;
    pcl_exec("timer_cb");
    timestamp = tnow;
}

#define WAIT_TIMEOUT 1000

void pcl_io(void)
{
    static char buf[IO_BUF_SZ];
    char buf2[IO_BUF_SZ];
    uint8_t newline = 1;
    uint16_t sz = io_recv_str(buf);
    uint16_t rc = PICOL_WAIT;
    uint32_t wait = pcl_interp->wait;
    static char *dev = 0;
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
            newline = 0;
            wait = 0;
            rc = PICOL_OK;
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
    {
        pcl_try_timer_cb();
        return;
    }
    picolVar *pv = picolGetVar2(pcl_interp, "DEV", 1);
    if(pv)
        dev = pv->val;
    if(rc != PICOL_WAIT)
    {
        io_send_str4(buf);
        io_prompt(newline, dev, pcl_prefix);
        pcl_interp->wait = 0;
    }
    else if(wait == 0)
    {
        pcl_interp->wait = HAL_GetTick();
    }
}

void pcl_gpio_exti_cb(void)
{
    static uint32_t t = 0;
    uint32_t t1 = HAL_GetTick();
    if((t1 - t) < 300)
        return;
    t = t1;
    main_evt |= EVT_BTN;
}

