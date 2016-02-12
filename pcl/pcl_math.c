
#include "pcl_math.h"

COMMAND(math2) {
    ARITY(argc == 3, "op arg1 arg2");
    int arg1 = str2int(argv[1]);
    int arg2 = str2int(argv[2]);
    int result;
    if(EQ(argv[0], "==")) result = (arg1 == arg2);
    if(EQ(argv[0], "!=")) result = (arg1 != arg2);
    if(EQ(argv[0], "/")) result = (arg1 / arg2);
    if(EQ(argv[0], "%")) result = (arg1 % arg2);
    return picolSetIntResult(i, result);
}

COMMAND(mathn) {
    ARITY(argc >= 2, "op arg1 arg2 ...");
    int j;
    int result = str2int(argv[1]);
    for(j = 2; j < argc; j++)
    {
        if(EQ(argv[0], "+")) result = (result + str2int(argv[j]));
        if(EQ(argv[0], "-")) result = (result - str2int(argv[j]));
    }
    return picolSetIntResult(i, result);
}

void pcl_math_init(picolInterp *i)
{
    int j;
    char *name2[] =
        {"==", "!=", "/", "%"};
    for (j = 0; j < (int)(sizeof(name2)/sizeof(char*)); j++)
        picolRegisterCmd(i, name2[j], picol_math2, 0);
    char *namen[] =
        {"+", "-"};
    for (j = 0; j < (int)(sizeof(namen)/sizeof(char*)); j++)
        picolRegisterCmd(i, namen[j], picol_mathn, 0);
}

