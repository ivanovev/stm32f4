
#include "pcl_misc.h"

COMMAND(inthex) {
    char buf[MAXSTR] = "";
    ARITY(argc == 2 || argc == 3, "int|hex ...");
    int32_t value = str2int(argv[1]);
    int len = 0;
    if(argc >= 3)
        len = str2int(argv[2]);
    if(SUBCMD0("int")) {
        int2str(value, buf, 10, len);
    }
    else if(SUBCMD0("hex")) {
        int2str(value, buf, 16, 0);
    }
    else
        return PICOL_ERR;
    return picolSetResult(i, buf);
}

COMMAND(list) {
    char buf[MAXSTR] = "";
    int a;
    for(a=1; a<argc; a++) LAPPEND(buf,argv[a]);
    return picolSetResult(i,buf);
}

void picolListParse(picolList *l, char *str)
{
    l->size = 0;
    int len = mystrnlen(str, MAXSTR), j;
    char *ptr = str, *ptr1;
    for(j = 0; j < DEFAULT_CONTSIZE; j++)
    {
        while((ptr[0] == ' ') && (len > 0))
        {
            len--;
            ptr++;
        }
        if(len == 0)
            break;
        l->table[l->size] = ptr;
        l->size++;
        ptr1 = (char*)mymemchr((uint8_t*)ptr, ' ', len);
        if(!ptr1)
            break;
        len -= (ptr1 - ptr);
        ptr = ptr1;
    }
}

COMMAND(llength)
{
    picolList l;
    picolListParse(&l, argv[1]);
    return picolSetIntResult(i, l.size);
}

COMMAND(lsearch)
{
    ARITY(argc == 3, "lsearch list pattern")
    picolList l;
    picolListParse(&l, argv[1]);
    int j = 0, len = mystrnlen(argv[2], MAXSTR);
    for(j = 0; j < l.size; j++)
    {
        if(!mystrncmp(l.table[j], argv[2], len))
            break;
    }
    if(j != l.size)
        return picolSetIntResult(i, j);
    return picolSetResult(i, "-1");
}

void pcl_misc_init(picolInterp *i)
{
    picolRegisterCmd(i, "int", picol_inthex, 0);
    picolRegisterCmd(i, "hex", picol_inthex, 0);
    picolRegisterCmd(i, "list", picol_list, 0);
    picolRegisterCmd(i, "llength", picol_llength, 0);
    picolRegisterCmd(i, "lsearch", picol_lsearch, 0);
}

