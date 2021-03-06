
#ifdef ENABLE_ETH

#include "pcl_sys.h"
#include "eth/eth.h"
#include "eth/mdio.h"

#ifdef ENABLE_EEPROM
#include "i2c/eeprom/eeprom.h"
#endif

#ifdef ENABLE_ICMP
#include "eth/myip/icmp/icmp.h"

extern uint8_t local_ipaddr[4];
extern ARP_ENTRY arp_table[ARP_TABLE_SZ];
#endif

#ifdef ENABLE_PTP
#include "eth/myip/ptp/ptpd.h"
#endif

#ifdef ENABLE_STREAM
#include "eth/myip/stream/stream.h"
#endif

extern volatile uint8_t main_evt;

#ifdef ENABLE_TELNET
COMMAND(exit) {
    myip_tcp_con_close();
    return picolSetIntResult(i, 0);
}
COMMAND(reboot) {
    myip_tcp_con_close();
    main_evt = EVT_REBOOT;
    return picolSetIntResult(i, 0);
}
#endif

COMMAND(mdio) {
    ARITY(argc >= 2, "mdio reg [val]");
    uint16_t reg = (uint16_t)str2int(argv[1]), val = 0;
    if(argc == 2)
        val = (uint16_t)mdio_read(reg);
    if(argc == 3)
        val = (uint16_t)mdio_write(reg, (uint16_t)str2int(argv[2]));
    return picolSetHex2Result(i, val);
}

static uint16_t parse_eth_addr(char *in, uint8_t *out, char delim, uint8_t len, uint16_t base)
{
    uint8_t j, k, ii;
    char *a = in;
    for(j = 0, k = 0, ii = 0; j <= mystrnlen(in, 20); j++)
    {
        if((a[j] == 0) || a[j] == delim)
            out[ii++] = (base == 10) ? atoi(&a[k]) : htoi(&a[k]);
        if(a[j] == 0)
            break;
        if(a[j] == delim)
            k = j + 1;
    }
    if(len != ii)
        return 0;
    return len;
}

COMMAND(eth) {
    ARITY(argc >= 2, "eth ipaddr|macaddr ...");
    char buf[MAXSTR];
    if(SUBCMD1("reset")) {
        eth_reset();
        return PICOL_OK;
    }
    else if(SUBCMD1("ipaddr"))
    {
        uint8_t ipaddr[4] = {IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3};
        if(argc == 2)
        {
#ifdef ENABLE_EEPROM
            eeprom_ipaddr_read(ipaddr);
#endif
        }
        if(argc == 3)
        {
            if(parse_eth_addr(argv[2], ipaddr, '.', 4, 10) != 4)
                return PICOL_ERR;
#ifdef ENABLE_EEPROM
            eeprom_ipaddr_write(ipaddr);
#endif
        }
        mysnprintf(buf, sizeof(buf), "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
        return picolSetResult(i, buf);
    }
    else if(SUBCMD1("macaddr"))
    {
        uint8_t macaddr[6] = {MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5};
        if((argc == 2) || ((argc == 3) ? SUBCMD2("-") : 0))
        {
#ifdef ENABLE_EEPROM
            eeprom_macaddr_read(macaddr);
#endif
        }
        else if(argc == 3)
        {
            if(SUBCMD2("rand"))
            {
#ifdef ENABLE_RNG
                rng_macaddr(macaddr);
#endif
            }
            else if(mystrnlen(argv[2], 20) > 12)
            {
                if(parse_eth_addr(argv[2], macaddr, ':', 6, 16) != 6)
                    return PICOL_ERR;
#ifdef ENABLE_EEPROM
                eeprom_macaddr_write(macaddr);
#endif
            }
        }
        if(argc == 2)
            mysnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
        else if(SUBCMD2("-"))
            mysnprintf(buf, sizeof(buf), "%02X%02X%02X%02X%02X%02X", macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
        return picolSetResult(i, buf);
    }
    else if(SUBCMD1("init"))
    {
        eth_init();
        return PICOL_OK;
    }
    else
        return PICOL_ERR;
    return PICOL_OK;
}

#ifdef ENABLE_PTP
static char* format_delay(ptpdt_t *dt, char *ptr, uint32_t sz)
{
    if(dt->s)
    {
        ptr += mysnprintf(ptr, sz, "%d", dt->s);
        *ptr++ = 's';
    }
    else
    {
        ptr += mysnprintf(ptr, sz, "%d", dt->ns);
        *ptr++ = 'n';
        *ptr++ = 's';
    }
    *ptr = 0;
    return ptr;
}
COMMAND(ptp) {
    char buf[MAXSTR];
    if(argc == 1)
    {
        uint16_t state = myip_ptpd_get_state();
        if(state == PTP_OFF)
            mysnprintf(buf, sizeof(buf), "%s", "off");
        else
            mysnprintf(buf, sizeof(buf), "%s %s", (state & PTP_E2E) ? "e2e" : "p2p", (state & PTP_MASTER) ? "master" : "slave");
        return picolSetResult(i, buf);
    }
    uint8_t ipaddr[4];
    ptpdt_t dt;
    if(SUBCMD1("time"))
    {
        format_time(buf, sizeof(buf), eth_ptpclk_seconds());
        return picolSetResult(i, buf);
    }
    if(SUBCMD1("pps"))
    {
        if(argc == 2)
            return picolSetIntResult(i, eth_ptp_ppsfreq(-1));
        return picolSetIntResult(i, eth_ptp_ppsfreq(str2int(argv[2])));
    }
    if(SUBCMD1("portid"))
    {
        if(argc == 2)
            return picolSetHex2Result(i, myip_ptpd_get_port_id());
        return picolSetHex2Result(i, myip_ptpd_set_port_id(str2int(argv[2])));
    }
    if(SUBCMD1("clkid"))
    {
        uint8_t *clock_id = (uint8_t*)&buf[sizeof(buf) - 8];
        myip_ptpd_get_clock_id(clock_id);
        bytes2str((char*)clock_id, (uint8_t*)buf, 8);
        return picolSetResult(i, buf);
    }
    if(SUBCMD1("offset"))
    {
        myip_ptpd_get_offset(&dt);
        char *ptr = buf;
        format_delay(&dt, ptr, sizeof(buf)-1);
        return picolSetResult(i, buf);
    }
    if(SUBCMD1("off"))
    {
        myip_ptpd_set_state(PTP_OFF);
        return PICOL_OK;
    }
    if(SUBCMD1("e2e") || SUBCMD1("p2p"))
    {
        ARITY(argc >= 3, "ptp e2e|p2p master|slave");
        uint16_t state = 0;
        if(SUBCMD1("e2e"))
            state |= PTP_E2E;
        if(SUBCMD1("p2p"))
            state |= PTP_P2P;
        if(SUBCMD2("master"))
            state |= PTP_MASTER;
        else if(SUBCMD2("slave"))
            state |= PTP_SLAVE;
        else
            return PICOL_ERR;
        myip_ptpd_set_state(state);
        return PICOL_OK;
    }
    if(SUBCMD1("p2p"))
    {
        if(SUBCMD2("master"))
        {
            myip_ptpd_set_state(PTP_P2P_MASTER);
            return PICOL_OK;
        }
        if(SUBCMD2("slave"))
        {
            myip_ptpd_set_state(PTP_P2P_SLAVE);
            return PICOL_OK;
        }
        return PICOL_ERR;
    }
    if(SUBCMD1("pdelay"))
    {
        ARITY(argc >= 3, "ptp pdelay ipaddr");
        if(parse_eth_addr(argv[2], ipaddr, '.', 4, 10) != 4)
            return PICOL_ERR;
        if(i->wait == 0)
        {
            myip_ptpd_pdelay(ipaddr, 0);
            picolSetResult(i, "N/A");
            return PICOL_WAIT;
        }
        if(i->wait)
        {
            if(!myip_ptpd_pdelay(0, &dt))
            {
                format_delay(&dt, buf, sizeof(buf));
                picolSetResult(i, buf);
                return PICOL_OK;
            }
            return PICOL_WAIT;
        }
        return PICOL_ERR;
    }
    return PICOL_ERR;
}
#endif

#ifdef ENABLE_STREAM
COMMAND(stream) {
    char buf[MAXSTR];
    uint8_t ipaddr[4];
    const uint8_t *ptr;
    uint16_t j;
    ARITY(argc >= 2, "stream in|out|io|status [...]");
    static uint8_t dir = 0;
    if(SUBCMD1("in"))
        dir = STREAM_IN;
    if(SUBCMD1("out"))
        dir = STREAM_OUT;
    if(SUBCMD1("io"))
        dir = STREAM_IO;
    for(j = 2; j < argc; j++)
    {
        if(SUBCMD(j, "start"))
        {
            myip_stream_start(dir);
            return picolSetIntResult(i, myip_stream_status());
        }
        if(SUBCMD(j, "stop"))
        {
            myip_stream_stop(dir);
            return picolSetIntResult(i, myip_stream_status());
        }
        if(SUBCMD(j, "src") || SUBCMD(j, "dst"))
        {
            if(++j >= argc)
                continue;
            if(parse_eth_addr(argv[j], ipaddr, '.', 4, 10) != 4)
                continue;
            if(SUBCMD(j-1, "src") && (dir & STREAM_IN))
                myip_stream_in_src(ipaddr);
            if(SUBCMD(j-1, "dst") && (dir & STREAM_OUT))
                myip_stream_out_dst(ipaddr);
        }
    }
    if(SUBCMD1("stop"))
    {
        myip_stream_stop(STREAM_IO);
        return picolSetIntResult(i, myip_stream_status());
    }
    if(SUBCMD1("status"))
    {
        dir = myip_stream_status();
        j = 0;
        if(dir == STREAM_IN)
            j += mysnprintf(buf, sizeof(buf), "dir:\t%s\n\r", "in");
        else if(dir == STREAM_OUT)
            j += mysnprintf(buf, sizeof(buf), "dir:\t%s\n\r", "out");
        else if(dir == STREAM_IO)
            j += mysnprintf(buf, sizeof(buf), "dir:\t%s\n\r", "io");
        else
            j += mysnprintf(buf, sizeof(buf), "dir:\t%s\n\r", "none");
        ptr = myip_stream_in_src(0);
        j += mysnprintf(buf + j, sizeof(buf), "in  src:\t%d.%d.%d.%d\n\r", ptr[0], ptr[1], ptr[2], ptr[3]);
        ptr = myip_stream_out_dst(0);
        j += mysnprintf(buf + j, sizeof(buf), "out dst:\t%d.%d.%d.%d\n\r", ptr[0], ptr[1], ptr[2], ptr[3]);
        return picolSetResult(i, buf);
    }
    return PICOL_ERR;
}
#endif

#ifdef ENABLE_ICMP
COMMAND(ping) {
    ARITY(argc >= 2, "ping ipaddr");
    uint8_t ipaddr[4];
    char buf[MAXSTR];
    char *ptr = buf;
    if(parse_eth_addr(argv[1], ipaddr, '.', 4, 10) != 4)
        return PICOL_ERR;
    ptr += mysnprintf(ptr, sizeof(buf), "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    uint16_t alive = 0;
    if(ipaddr[0] == 127)
        alive = 1;
    else if(!mymemcmp(ipaddr, local_ipaddr, 4))
        alive = 1;
    else if(i->wait == 0)
    {
        myip_icmp_ping(ipaddr);
        return PICOL_WAIT;
    }
    else if(i->wait)
    {
        if(!myip_icmp_ping(0))
            alive = 1;
    }
    if(alive)
    {
        mysnprintf(ptr, sizeof(buf) - (ptr - buf), " %s", "is alive.");
        return picolSetResult(i, buf);
    }
    else
    {
        mysnprintf(ptr, sizeof(buf) - (ptr - buf), " %s", "is not available.");
        picolSetResult(i, buf);
        return PICOL_WAIT;
    }
    return PICOL_ERR;
}

COMMAND(arp) {
    char buf[MAXSTR];
    uint16_t j;
    char *ptr = buf;
    uint8_t *ipaddr;
    for(j = 0; j < ARP_TABLE_SZ; j++)
    {
        if(ptr != buf)
            *ptr++ = ' ';
        ipaddr = arp_table[j].ip_addr;
        if(ipaddr[0])
        {
            ptr += mysnprintf(ptr, sizeof(buf), "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
        }
    }
    *ptr = 0;
    return picolSetResult(i, buf);
}
#endif

void pcl_eth_init(picolInterp *i)
{
    picolRegisterCmd(i, "mdio", picol_mdio, 0);
    picolRegisterCmd(i, "eth", picol_eth, 0);
#ifdef ENABLE_TELNET
    picolRegisterCmd(i, "exit", picol_exit, 0);
    picolRegisterCmd(i, "reboot", picol_reboot, 0);
#endif
#ifdef ENABLE_ICMP
    picolRegisterCmd(i, "arp", picol_arp, 0);
    picolRegisterCmd(i, "ping", picol_ping, 0);
#endif
#ifdef ENABLE_PTP
    picolRegisterCmd(i, "ptp", picol_ptp, 0);
#endif
#ifdef ENABLE_STREAM
    picolRegisterCmd(i, "stream", picol_stream, 0);
#endif
}

#endif

