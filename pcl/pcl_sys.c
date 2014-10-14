
#include "pcl_sys.h"
#include "util/util.h"

#ifdef MY_ETH
#include "eth/myip/mytcp.h"
extern TCP_CON tcp_con;
extern volatile uint8_t reset;
#endif

COMMAND(version) {
    char buf[MAXSTR] = "";
    version(buf, MAXSTR);
    return picolSetResult(i, buf);
}
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

void register_sys_cmds(picolInterp *i)
{
    picolRegisterCmd(i, "version", picol_version, 0);
#ifdef MY_ETH
    picolRegisterCmd(i, "exit", picol_exit, 0);
    picolRegisterCmd(i, "reset", picol_reset, 0);
#endif
}

