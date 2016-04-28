
#include "picol.h"
#include "heap1.h"

#ifndef __arm__
static void ___print_token(picolParser *p)
{
    char buf[MAXSTR];
    snprintf(buf, p->end - p->start + 2, "%s", p->start);
    printf("token (%s)(%d)", buf, p->type);
    if(p->type == PT_ESC) printf("(PT_ESC)");
    if(p->type == PT_STR) printf("(PT_STR)");
    if(p->type == PT_CMD) printf("(PT_CMD)");
    if(p->type == PT_VAR) printf("(PT_VAR)");
    if(p->type == PT_SEP) printf("(PT_SEP)");
    if(p->type == PT_EOL) printf("(PT_EOL)");
    if(p->type == PT_EOF) printf("(PT_EOF)");
    if(p->type == PT_XPND) printf("(PT_XPND)");
    printf("\n");
}
static void ___print_list(picolList *l)
{
    printf("size = %d\n", l->size);
    if(l->size == 0)
        return;
    int i;
    for(i = 0; i < l->size; i++)
        printf("l[%d] = %s\n", i, l->table[i]);
}
#endif

/* ----------------------------------------------------- parser functions */
void picolInitParser(picolParser *p, const char *text) {
    p->text  = p->p = (char*)text;
    p->len   = mystrnlen(text, MAXSTR);
    p->start = 0; p->end = 0; p->insidequote = 0;
    p->type  = PT_EOL;
    p->expand = 0;
}
int picolParseSep(picolParser *p) {
    p->start = p->p;
    while(myisspace(*p->p) || (*p->p=='\\' && *(p->p+1)=='\n')) {
        p->p++; p->len--;
    }
    RETURN_PARSED(PT_SEP);
}
int picolParseBrace(picolParser *p) {
  int level = 1;
  p->start = ++p->p; p->len--;
  while(1) {
    if (p->len >= 2 && *p->p == '\\') {
      p->p++; p->len--;
    } else if (p->len == 0 || *p->p == '}') {
      level--;
      if (level == 0 || p->len == 0) {
        p->end = p->p-1;
        if (p->len) { p->p++; p->len--; } /* Skip final closed brace */
        p->type = PT_STR;
        return PICOL_OK;
      }
    } else if (*p->p == '{') level++;
    p->p++; p->len--;
  }
}
int picolParseCmd(picolParser *p) {
  int level = 1, blevel = 0;
  p->start = ++p->p; p->len--;
  while (p->len) {
    if (*p->p == '[' && blevel == 0)       level++;
    else if (*p->p == ']' && blevel == 0) {if (!--level) break;}
    else if (*p->p == '\\')               {p->p++; p->len--;}
    else if (*p->p == '{')                 blevel++;
    else if (*p->p == '}')                {if (blevel != 0) blevel--;}
    p->p++; p->len--;
  }
  p->end  = p->p-1;
  p->type = PT_CMD;
  if (*p->p == ']') { p->p++; p->len--; }
  return PICOL_OK;
}
int picolParseEol(picolParser *p) {
    p->start = p->p;
    while(myisspace(*p->p) || *p->p == ';') { p->p++; p->len--; }
    RETURN_PARSED(PT_EOL);
}
int picolParseString(picolParser *p) {
  int newword = (p->type == PT_SEP || p->type == PT_EOL || p->type == PT_STR);
  if(p->len >= 3 && !mystrncmp(p->p,"{*}",3)) {
    p->expand = 1;
    p->p += 3; p->len -= 3; /* skip the {*} expand indicator */
  }
  if (newword && *p->p == '{') {return picolParseBrace(p);}
  else if (newword && *p->p == '"') {
    p->insidequote = 1;
    p->p++; p->len--;
  }
  for(p->start = p->p; 1; p->p++, p->len--) {
    if (p->len == 0)       RETURN_PARSED(PT_ESC);
    switch(*p->p) {
    case '\\':             if (p->len >= 2) { p->p++; p->len--; }; break;
    case '$': case '[':    RETURN_PARSED(PT_ESC);
    case ' ': case '\t': case '\n': case '\r': case ';':
      if (!p->insidequote) RETURN_PARSED(PT_ESC);
      break;
    case '"':
      if (p->insidequote) {
        p->end = p->p-1;
        p->type = PT_ESC;
        p->p++; p->len--;
        p->insidequote = 0;
        return PICOL_OK;
      }
      break;
    }
  }
}
int picolParseVar(picolParser *p) {
    int parened = 0;
    p->start = ++p->p; p->len--; /* skip the $ */
    if (*p->p == '{') {
        picolParseBrace(p);
        p->type = PT_VAR;
        return PICOL_OK;
    }
    if(COLONED(p->p)) {p->p += 2; p->len -= 2;}
    /*  while(isalnum(*p->p) || strchr("()_",*p->p)) { p->p++; p->len--; }*/
#if 1
    while(myisalnum(*p->p) || *p->p == '_' || *p->p=='('||*p->p==')') {
        if(*p->p=='(') parened = 1;
        p->p++; p->len--;
    }
    if(!parened && *(p->p-1)==')') {p->p--; p->len++;}
#endif
    if (p->start == p->p) { /* It's just a single char string "$" */
        picolParseString(p);
        p->start--; p->len++; /* back to the $ sign */
        p->type = PT_VAR;
        return PICOL_OK;
    } else RETURN_PARSED(PT_VAR);
}
/*------------------------------------ General functions: variables, errors */
void picolInitInterp(picolInterp *i) {
    i->level    = 0;
    i->commands = 0;
    i->callframe = mycalloc(1,sizeof(picolCallFrame));
    i->ass = 0;
    i->wait = 0;
}
picolCmd* picolGetCmd(picolInterp *i, char *name) {
    picolCmd *c;
    for(c = i->commands; c; c = c->next) if (EQ(c->name,name)) return c;
    return 0;
}
void picolListAppend(picolList *l, char *str, char *buf, char sep) {
    if(l->size == 0)
        l->table[0] = buf;
    else
    {
        l->table[l->size] = l->table[l->size-1] + mystrnlen(l->table[l->size-1], MAXSTR);
        if(sep)
            l->table[l->size][0] = sep;
        l->table[l->size] += 1;
    }
    mysnprintf(l->table[l->size], mystrnlen(str, MAXSTR), "%s", str);
    l->size += 1;
}
char* picolList1(char* buf, int argc, char** argv) {
    int a;
    buf[0] = '\0'; /* caller responsible for supplying large enough buffer */
    for(a=0; a<argc; a++) LAPPEND_X(buf,argv[a]);
    return buf;
}
int picolSetResult(picolInterp *i, char *s) {
    mysnprintf(i->result, MAXSTR, "%s", s);
    return PICOL_OK;
}
int picolSetFmtResult(picolInterp* i, char* fmt, int result) {
    char buf[32];
    mysnprintf(buf,sizeof(buf),fmt,result);
    return picolSetResult(i,buf);
}
int picolErr(picolInterp *i, char* str) {
    picolSetResult(i,str);
    return PICOL_ERR;
}
int picolErr1(picolInterp *i, char* format, char* arg) {
    /* 'format' should contain exactly one %s specifier */
    char buf[MAXSTR];
    mysnprintf(buf, MAXSTR, format, arg);
    return picolErr(i, buf);
}
#define   picolGetVar(_i,_n)       picolGetVar2(_i,_n,0)
#define   picolGetGlobalVar(_i,_n) picolGetVar2(_i,_n,1)
picolVar *picolGetVar2(picolInterp *i, char *name, int glob) {
    picolVar *v = i->callframe->vars;
    int  global = COLONED(name);
    //char buf[MAXSTR], buf2[MAXSTR], *cp, *cp2;
    if(global || glob) {
        picolCallFrame *c = i->callframe;
        while(c->parent) c = c->parent;
        v = c->vars;
        if(global) name += 2;  /* skip the "::" */
    }
#if 0
    if((cp = mymemchr((uint8_t*)name,'(',mystrnlen(name,MAXSTR)))) { /* array element syntax? */
        picolArray* ap;
        int found = 0;
        strncpy(buf,name,cp-name);
        buf[cp-name] = '\0';
        for( ;v; v = v->next) if (EQ(v->name,buf)) {found = 1; break;}
        if(!found) return NULL;
        if(!((ap = picolIsPtr(v->val)))) return NULL;
        strcpy(buf2,cp+1); /* copy the key from after the opening paren*/
        if(!((cp = strchr(buf2,')')))) return NULL;
        *cp = '\0';       /* overwrite closing paren */
        v = picolArrGet1(ap,buf2);
        if(!v) {
            if(!((cp2 = getenv(buf2)))) return NULL;
            strcpy(buf,"::env("); strcat(buf,buf2); strcat(buf,")");
            return picolArrSet1(i, buf, cp2);
        }
        return v;
    }
#endif
    for( ;v; v = v->next) {if (EQ(v->name,name)) return v;}
    return NULL;
}
#define picolSetVar(_i,_n,_v)       picolSetVar2(_i,_n,_v,0)
#define picolSetGlobalVar(_i,_n,_v) picolSetVar2(_i,_n,_v,1)
int     picolSetVar2(picolInterp *i, char *name, char *val,int glob) {
    picolVar       *v = picolGetVar(i,name);
    picolCallFrame *c = i->callframe, *localc = c;
    int             global = COLONED(name);
    if(glob||global) v = picolGetGlobalVar(i,name);
    if(!v) {           /* non-existing variable */
        if(glob || global) {
            if(global) name += 2;
            while(c->parent) c = c->parent;
        } else {
            v       = mymalloc(sizeof(*v));
            v->name = mystrdup(name);
            v->next = c->vars;
            c->vars = v;
            i->callframe = localc;
        }
    }
    if(v)
    {
        if(glob || global)
            mystrncpy(v->val, val, mystrnlen(v->val, MAXSTR));
        else
            v->val = mystrdup(val);
    }
    return PICOL_OK;
}
int picolGetToken(picolInterp *i, picolParser *p) {
    int rc;
    while(1) {
        if (!p->len) {
            if (p->type != PT_EOL && p->type != PT_EOF)
                p->type = PT_EOL;
            else p->type = PT_EOF;
            return PICOL_OK;
        }
        switch(*p->p) {
            case ' ': case '\t':
                if (p->insidequote) return picolParseString(p);
                else                return picolParseSep(p);
            case '\n': case '\r': case ';':
                if (p->insidequote) return picolParseString(p);
                else                return picolParseEol(p);
            case '[': rc = picolParseCmd(p);
                      if(rc == PICOL_ERR) return picolErr(i,"missing close-bracket");
                      return rc;
            case '$':   return picolParseVar(p);
                                  // case '#': if (p->type == PT_EOL) {picolParseComment(p);continue;}
            default:              return picolParseString(p);
        }
    }
}
#define picolEval(_i,_t)  picolEval2(_i,_t,1)
#define picolSubst(_i,_t) picolEval2(_i,_t,0)
int picolEval2(picolInterp *i, const char *t, int mode) { /*----------- EVAL! */
    /* mode==0: subst only, mode==1: full eval */
    picolParser p;
    char        buf[MAXSTR], tmp[MAXSTR], sep = 0;
    if(mode == 0)
        sep = ' ';
    uint32_t    len;
    int         rc = PICOL_OK;
    picolList args; args.size = 0;
    picolSetResult(i, "");
    picolInitParser(&p, t);
    while(1) {
        picolGetToken(i, &p);
        //___print_token(&p);
        if (p.type == PT_EOF)
            break;
        len = p.end - p.start + 1;
        mystrncpy(tmp, p.start, len);
        //___print_list(&args);
        if (p.type == PT_SEP) {
            continue;
        }
        else if (p.type == PT_VAR) {
            picolVar *v = picolGetVar(i,tmp);
            if (v && !v->val) v = picolGetGlobalVar(i,tmp);
            if(!v)
                return picolErr(i,"no such variable");
            picolListAppend(&args, v->val, buf, sep);
            //___print_list(&args);
            continue;
        } else if (p.type == PT_CMD) {
            rc = picolEval(i,tmp);
            if (rc != PICOL_OK)
                break;
            picolListAppend(&args, i->result, buf, sep);
            continue;
        }
        /* We have a complete command + args. Call it! */
        else if (p.type == PT_EOL) {
            picolCmd *c;
            //args.size -= 1;
            if(mode==0) { /* do a quasi-subst only */
                rc = picolSetResult(i, buf);
                goto err; /* not an error, if rc == PICOL_OK */
            }
            if (args.size) {
                if ((c = picolGetCmd(i,args.table[0])) == NULL) {
                    rc = picolErr(i,"invalid command name");
                    goto err;
                }
                rc = c->func(i,args.size,args.table);
                if (rc != PICOL_OK) goto err;
                args.size = 0;
                continue;
            }
            args.size = 0;
            continue;
        }
        else
            picolListAppend(&args, tmp, buf, sep);
    }
err:
    return rc;
}
int picolCallProc(picolInterp *i, int argc, char **argv)
{
    if(i->level>MAXRECURSION)
        return picolErr(i,"too many nested evaluations (infinite loop?)");
    picolCmd *c = picolGetCmd(i,argv[0]);
    if(!c)
        return PICOL_ERR;
    void *pd = c->privdata;
    if(!pd)
        return PICOL_ERR;
    char **x=pd;
    char *alist=x[0], *body=x[1];
    char buf[MAXSTR];
    picolCallFrame *cf = mycalloc(1,sizeof(*cf));
    int a = 0, done = 0, errcode = PICOL_ERR;
#ifndef __arm__
    if(!cf) {printf("could not allocate callframe\n"); exit(1);}
#else
    extern void print1(char *str);
    if(!cf) {dbg_send_str3("could not allocate callframe", 1); return PICOL_ERR;}
#endif
    cf->parent   = i->callframe;
    i->callframe = cf;
    i->level++;
    char *p = mystrdup(alist);
    while(1) {
        char *start = p;
        while(*p != ' ' && *p != '\0') p++;
        if (*p != '\0' && p == start) { p++; continue; }
        if (p == start) break;
        if (*p == '\0') done=1; else *p = '\0';
        if(EQ(start,"args") && done) {
            dbg_send_hex2("eq", p - alist);
            picolSetVar(i,start,picolList1(buf,argc-a-1,argv+a+1));
            a = argc-1;
            break;
        }
        if (++a > argc-1)
            break;
        picolSetVar(i,start,argv[a]);
        p++;
        if (done) break;
    }
    if (a == argc-1)
        errcode = picolEval(i,body);
    else
        errcode = picolErr1(i,"wrong # args for '%s'",argv[0]);
    if (errcode == PICOL_RETURN)
        errcode = PICOL_OK;
    i->callframe = cf->parent;
    myfree(cf);
    i->level--;
    return errcode;
}
int picolCondition(picolInterp *i, char* str)
{
    if(str) {
        char buf[MAXSTR], buf2[MAXSTR], *argv[3], *cp;
        int a = 0, rc;
        rc = picolSubst(i,str);
        if(rc != PICOL_OK) return rc;
        //mysnprintf(buf, MAXSTR, "Condi: (%s) ->(%s)\n",str,i->result);
        //dbg_send_str3(buf, 1);
        mystrncpy(buf2,i->result, MAXSTR);
        /* ------- try whether the format suits [expr]... */
        mystrncpy(buf,"llength ", MAXSTR); LAPPEND(buf,i->result);
        //dbg_send_str3(buf, 1);
        rc = picolEval(i,buf);
        if(rc != PICOL_OK) return rc;
#if 0
        if(EQ(i->result,"3")) {
            FOREACH(buf,cp,buf2) argv[a++] = mystrdup(buf);
            if(picolGetCmd(i,argv[1])) { /* defined operator in center */
                mystrncpy(buf,argv[1], MAXSTR);       /* translate to Polish :) */
                LAPPEND(buf,argv[0]);      /* e.g. {1 > 2} -> {> 1 2} */
                LAPPEND(buf,argv[2]);
                rc = picolEval(i, buf);
                return rc;
            }
        } /* .. otherwise, check for inequality to zero */
#endif
        if(*str == '!') {mystrncpy(buf, "== 0 ", MAXSTR); str++;} /* allow !$x */
        else             mystrncpy(buf, "!= 0 ", MAXSTR);
        mystrncat(buf, str, MAXSTR);
        return picolEval(i, buf); // todo: compare without eval
    }
    else
        return picolErr(i, "NULL condition");
}
int picolRegisterCmd(picolInterp *i, char *name, picol_Func f, void *pd) {
    picolCmd *c = picolGetCmd(i,name);
    if (c) return picolErr(i,"command already defined");
    c = mymalloc(sizeof(picolCmd));
    c->name     = mystrdup(name);
    c->func     = f;
    c->privdata = pd;
    c->next     = i->commands;
    i->commands = c;
    return PICOL_OK;
}
COMMAND(set) {
    picolVar* pv;
    ARITY(argc == 2 || argc == 3, "set var [val]")
    if (argc == 2) {
        GET_VAR(pv,argv[1]);
        if(pv && pv->val) return picolSetResult(i,pv->val);
        else pv = picolGetGlobalVar(i,argv[1]);
        if(!(pv && pv->val)) return picolErr1(i,"no value of '%s'\n",argv[1]);
        return picolSetResult(i,pv->val);
    } else {
        picolSetVar(i,argv[1],argv[2]);
        return picolSetResult(i,argv[2]);
    }
}
COMMAND(proc) {
    char **procdata = NULL;
    picolCmd* c = picolGetCmd(i,argv[1]);
    ARITY(argc == 4, "proc name args body");
    if(c) procdata = c->privdata;
    if(!procdata) {
        procdata = mycalloc(2, sizeof(char*));
        if(c) {
            c->privdata = procdata;
            c->func = picolCallProc; /* may override C-coded cmds */
        }
    }
    procdata[0] = mystrdup(argv[2]); /* arguments list */
    procdata[1] = mystrdup(argv[3]); /* procedure body */
    if(!c) picolRegisterCmd(i,argv[1],picolCallProc,procdata);
    return PICOL_OK;
}
COMMAND(return) {
    ARITY(argc == 1 || argc == 2, "return [result]");
    picolSetResult(i, (argc == 2) ? argv[1] : "");
    return PICOL_RETURN;
}
COMMAND(puts) {
#ifndef __arm__
    printf("%s\n", argv[1]);
#else
    io_send_str2(argv[1]);
    io_newline();
#endif
    return picolSetResult(i,"");
}
COMMAND(test) {
    return picolSetResult(i,"test");
}
COMMAND(info) {
    char buf[MAXSTR];
    buf[0] = 0;
    picolCmd *c = i->commands;
    ARITY(argc == 2 || argc == 3, "info args|body|commands|procs|globals|vars");
    int procs = SUBCMD1("procs");
    if(SUBCMD1("commands") || procs)
    {
        for(; c; c = c->next)
        {
            if(!procs || c->privdata)
                LAPPEND(buf,c->name);
        }
        return picolSetResult(i, buf);
    }
    else if (SUBCMD1("args") || SUBCMD1("body"))
    {
        if(argc==2) return picolErr1(i,"usage: info %s procname", argv[1]);
        for( ; c; c = c->next) {
            if(EQ(c->name,argv[2])) {
                char **pd = c->privdata;
                if(pd) return picolSetResult(i,pd[(EQ(argv[1],"args")?0 : 1)]);
                else   return picolErr1(i,"\"%s\" isn't a procedure", c->name);
            }
        }
    }
    if(SUBCMD1("vars") || SUBCMD1("globals"))
    {
        picolCallFrame *cf = i->callframe;
        picolVar       *v;
        if(SUBCMD1("globals")) {while(cf->parent) cf = cf->parent;}
        for(v = cf->vars; v; v = v->next) {
            LAPPEND(buf,v->name);
        }
        return picolSetResult(i, buf);
    }
    return PICOL_ERR;
}

COMMAND(if)
{
    int rc;
    ARITY(argc==3 || argc==5, "if test script1 ?else script2?")
        if ((rc = picolCondition(i,argv[1])) != PICOL_OK) return rc;
    if ((rc = atoi(i->result))) return picolEval(i,argv[2]);
    else if (argc == 5)         return picolEval(i,argv[4]);
    return picolSetResult(i,"");
}

picolInterp* picolCreateInterp(void) {
    picolInterp* i = mymalloc(sizeof(picolInterp));
    picolInitInterp(i);
    picolRegisterCmd(i, "set",  picol_set, 0);
    picolRegisterCmd(i, "proc", picol_proc, 0);
    picolRegisterCmd(i, "return", picol_return, 0);
    picolRegisterCmd(i, "puts", picol_puts, 0);
    //picolRegisterCmd(i, "test", picol_test, 0);
    picolRegisterCmd(i, "if", picol_if, 0);
    picolRegisterCmd(i, "info", picol_info, 0);
    return i;
}

#ifndef __arm__
#include "pcl.h"
#include "stdio.h"
int main(int argc, char **argv) {
    picolInterp *i = picolCreateInterp();
    register_misc_cmds(i);
    printf("interpsz: %X\n", sizeof(picolInterp));
    printf("proc test1 {a} {puts $a}\n");
    printf("test1 345\n");
    char buf[MAXSTR] = "";
    int rc;
    while(1)
    {
        printf("picol> "); fflush(stdout);
        if (fgets(buf,sizeof(buf),stdin) == NULL) return 0;
        rc = picolEval(i, buf);
        printf("[%d] %s\n", rc, i->result);
    }
}
#endif

