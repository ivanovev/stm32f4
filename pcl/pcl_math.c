
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
    if(EQ(argv[0], "<")) result = (arg1 < arg2);
    if(EQ(argv[0], ">")) result = (arg1 > arg2);
    if(EQ(argv[0], "<<")) result = (arg1 << arg2);
    if(EQ(argv[0], ">>")) result = (arg1 >> arg2);
    return picolSetIntResult(i, result);
}

COMMAND(mathn) {
    ARITY(argc >= 2, "op arg1 arg2 ...");
    int j, aj;
    int result = str2int(argv[1]);
    for(j = 2; j < argc; j++)
    {
        aj = str2int(argv[j]);
        if(EQ(argv[0], "+")) result = (result + aj);
        if(EQ(argv[0], "-")) result = (result - aj);
        if(EQ(argv[0], "min")) result = (result < aj) ? result : aj;
        if(EQ(argv[0], "max")) result = (result > aj) ? result : aj;
    }
    return picolSetIntResult(i, result);
}

COMMAND(clamp) {
    ARITY(argc == 4, "clamp min max value");
    int arg1 = str2int(argv[1]);
    int arg2 = str2int(argv[2]);
    int arg3 = str2int(argv[3]);
    arg3 = (arg1 > arg3) ? arg1 : arg3;
    arg3 = (arg2 < arg3) ? arg2 : arg3;
    return picolSetIntResult(i, arg3);
}

void pcl_math_init(picolInterp *i)
{
    int j;
    char *name2[] =
        {"==", "!=", "/", "%", "<", ">", "<<", ">>"};
    for (j = 0; j < (int)(sizeof(name2)/sizeof(char*)); j++)
        picolRegisterCmd(i, name2[j], picol_math2, 0);
    char *namen[] =
        {"+", "-", "min", "max"};
    for (j = 0; j < (int)(sizeof(namen)/sizeof(char*)); j++)
        picolRegisterCmd(i, namen[j], picol_mathn, 0);
    picolRegisterCmd(i, "clamp", picol_clamp, 0);
}

