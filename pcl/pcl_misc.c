
#include "pcl_misc.h"

COMMAND(inthex) {
    char buf[MAXSTR] = "";
    ARITY(argc == 2 || argc == 3, "int|hex ...");
    int32_t value = str2int(argv[1]);
    if(SUBCMD0("int")) {
        int2str(value, buf, 10);
    }
    else if(SUBCMD0("hex")) {
        int2str(value, buf, 16);
    }
    else
        return PICOL_ERR;
    return picolSetResult(i, buf);
}

void register_misc_cmds(picolInterp *i)
{
    picolRegisterCmd(i, "int", picol_inthex);
    picolRegisterCmd(i, "hex", picol_inthex);
}

