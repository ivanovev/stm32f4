
#include <main.h>
#include "stdint.h"
#include "stddef.h"

#ifndef __PICOL_H__
#define __PICOL_H__

#define MAXSTR 1024

#define MAXRECURSION 10

/* -------------------------- Macros mostly need picol_ environment (argv,i) */
#define APPEND(dst,src) {if((mystrnlen(dst,MAXSTR)+mystrnlen(src,MAXSTR))>=sizeof(dst)-1) \
                        return picolErr(i, "string too long"); \
                        mystrncat(dst,src,MAXSTR);}

#define ARITY(x,s)     if (!(x)) return picolErr(i, s);
#define COLONED(_p)     (*(_p) == ':' && *(_p+1) == ':') /* global indicator */

#define COMMAND(c)      int picol_##c(picolInterp* i, int argc, char** argv)
#define EQ(a,b)         ((*a)==(*b) && !mystrncmp(a,b,IO_BUF_SZ))

#define FOLD(init,step,n) {init;for(p=n;p<argc;p++) {SCAN_INT(a,argv[p]);step;}}

#define FOREACH(_v,_p,_s) \
                 for(_p = picolParseList(_s,_v); _p; _p = picolParseList(_p,_v))

#define LAPPEND(dst,src) {int needbraces = (mystrchr(src,' ') || mystrnlen(src,MAXSTR)==0); \
                        if(*dst!='\0') APPEND(dst," "); if(needbraces) APPEND(dst,"{");\
                        APPEND(dst,src); if(needbraces) APPEND(dst,"}");}

/* this is the unchecked version, for functions without access to 'i' */
#define LAPPEND_X(dst,src) {int needbraces = (strchr(src,' ')!=NULL)||mystrnlen(src,MAXSTR)==0; \
                if(*dst!='\0') mystrncat(dst," ",MAXSTR); if(needbraces) mystrncat(dst,"{",MAXSTR); \
                mystrncat(dst,src,MAXSTR); if(needbraces) mystrncat(dst,"}",MAXSTR);}

#define GET_VAR(_v,_n) _v = picolGetVar(i,_n); \
                        if(!_v) return picolErr1(i,"can't read \"%s\": no such variable", _n);

#define PARSED(_t)        {p->end = p->p-1; p->type = _t;}
#define RETURN_PARSED(_t) {PARSED(_t);return PICOL_OK;}

#define SUBCMD0(x)         (EQ(argv[0],x))
#define SUBCMD1(x)         (EQ(argv[1],x))
#define SUBCMD2(x)         (EQ(argv[2],x))
#define SUBCMD3(x)         (EQ(argv[3],x))

#define picolEval(_i,_t)  picolEval2(_i,_t,1)
#define picolSubst(_i,_t) picolEval2(_i,_t,0)

#define picolSetBoolResult(i,x) picolSetFmtResult(i,"%d",!!x)
#define picolSetIntResult(i,x)  picolSetFmtResult(i,"%d",x)
#define picolSetHexResult(i,x)  picolSetFmtResult(i,"%X",x)
#define picolSetHex2Result(i,x)  picolSetFmtResult(i,"0x%4X",x)
#define picolSetHex4Result(i,x)  picolSetFmtResult(i,"0x%8X",x)

enum {PICOL_OK, PICOL_ERR, PICOL_WAIT};
enum {PT_ESC,PT_STR,PT_CMD,PT_VAR,PT_SEP,PT_EOL,PT_EOF, PT_XPND};
        
/* ------------------------------------------------------------------- types */
typedef struct picolParser {
  char  *text;
  char  *p;           /* current text position */
  size_t len;         /* remaining length */
  char  *start;       /* token start */
  char  *end;         /* token end */
  int    type;        /* token type, PT_... */
  int    insidequote; /* True if inside " " */
  int    expand;      /* true after {*} */
} picolParser;
        
typedef struct picolVar {
  char            *name, *val;
  struct picolVar *next;
} picolVar;

struct picolInterp; /* forward declaration */
typedef int (*picol_Func)(struct picolInterp *i, int argc, char **argv);

typedef struct picolCmd {
  char            *name;
  picol_Func       func;
  void            *privdata;
  struct picolCmd *next;
} picolCmd;

typedef struct picolCallFrame {
  picolVar              *vars;
  //char                  *command;
  struct picolCallFrame *parent; /* parent is NULL at top level */
} picolCallFrame;

typedef struct picolInterp {
  int             level;              /* Level of nesting */
  picolCallFrame *callframe;
  picolCmd       *commands;
  char           *current;        /* currently executed command */
  char           result[MAXSTR];
  int             trace; /* 1 to display each command, 0 if not */
  char          *ass; /* assert str */
  unsigned long wait;
} picolInterp;

#define DEFAULT_CONTSIZE 16

typedef struct picolArray {
  picolVar  *table[DEFAULT_CONTSIZE];
  int       size;
} picolArray;

typedef struct picolList {
  char      *table[DEFAULT_CONTSIZE];
  int       size;
} picolList;

picolInterp*    picolCreateInterp(void);
int             picolEval2(picolInterp *i, const char *t, int mode);
int             picolErr(picolInterp *i, char* str);
int             picolRegisterCmd(picolInterp *i, char *name, picol_Func f, void *pd);
int             picolSetResult(picolInterp *i, char *s);
int             picolSetFmtResult(picolInterp* i, char* fmt, int result);

#define   picolGetVar(_i,_n)       picolGetVar2(_i,_n,0)
#define   picolGetGlobalVar(_i,_n) picolGetVar2(_i,_n,1)
picolVar*       picolGetVar2(picolInterp *i, char *name, int glob);

#define picolSetVar(_i,_n,_v)       picolSetVar2(_i,_n,_v,0)
#define picolSetGlobalVar(_i,_n,_v) picolSetVar2(_i,_n,_v,1)
int             picolSetVar2(picolInterp *i, char *name, char *val,int glob);

#endif

