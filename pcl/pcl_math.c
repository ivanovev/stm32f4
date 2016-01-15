
#include "pcl_math.h"

COMMAND(math)
{
    int a = 0, b = 0, c = -1, p;
    char buf[32] = {0};
    mysnprintf(buf, 32, "%s a b", argv[0]);
    if ((SUBCMD0("/") || SUBCMD0("%")) && !b) {return picolErr(i, "divide by zero");}
    if (0) {}
    else if (SUBCMD0("+")) {FOLD(c=0,c += a,1)}
    return picolSetIntResult(i,c);
}

COMMAND(mathf)
{
    double a = 0, b = 0, c = -1;
    int p;
    char buf[32] = {0};
    mysnprintf(buf, 32, "%s a b", argv[0]);
    if ((SUBCMD0("/") || SUBCMD0("%")) && !b) {return picolErr(i, "divide by zero");}
    if (0) {}
    else if (SUBCMD0("+")) {FOLD(c=0,c += a,1)}
    double2str(buf, sizeof(buf), c, "6");
    return picolSetResult(i,buf);
}

void pcl_math_init(picolInterp *i)
{
    int j;
    char *name[] =
    {"+"};
    for (j = 0; j < (int)(sizeof(name)/sizeof(char*)); j++)
        picolRegisterCmd(i, name[j], picol_mathf, 0);
}

