
#include "pcl_sys.h"
#include "util/util.h"
#include "eth/myip/mytcp.h"

#ifdef MY_ETH
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
    reset = 1;
    return picolSetIntResult(i, 0);
}
#endif

void register_sys_cmds(picolInterp *i)
{
    picolRegisterCmd(i, "version", picol_version);
#ifdef MY_ETH
    picolRegisterCmd(i, "exit", picol_exit);
    picolRegisterCmd(i, "reset", picol_reset);
#endif
}

