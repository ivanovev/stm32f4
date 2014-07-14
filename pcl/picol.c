
#include "main.h"
#if 0
#include "heap1.h"
#include "picol.h"
#include "ctype.h"
#include "stdio.h"
#include "string.h"
#endif

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
void picolInitParser(picolParser *p, char *text) {
    p->text  = p->p = text;
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
/*------------------------------------ General functions: variables, errors */
void picolInitInterp(picolInterp *i) {
    i->level    = 0;
    i->commands = 0;
}
picolCmd* picolGetCmd(picolInterp *i, char *name) {
    picolCmd *c;
    for(c = i->commands; c; c = c->next) if (EQ(c->name,name)) return c;
    return 0;
}
void picolListAppend(picolList *l, picolParser *p, char *buf) {
    if(l->size == 0)
        l->table[0] = buf;
    else
        l->table[l->size] = l->table[l->size-1] + mystrnlen(l->table[l->size-1], MAXSTR) + 1;
    mysnprintf(l->table[l->size], p->end - p->start + 1, "%s", p->start);
    l->size += 1;
}
int picolSetResult(picolInterp *i, char *s) {
    mysnprintf(i->result, MAXSTR, "%s", s);
    return PICOL_OK;
}
int picolErr(picolInterp *i, char* str) {
    picolSetResult(i,str);
    return PICOL_ERR;
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
                      //case '$':             return picolParseVar(p);
                      // case '#': if (p->type == PT_EOL) {picolParseComment(p);continue;}
            default:              return picolParseString(p);
        }
    }
}

#define picolEval(_i,_t)  picolEval2(_i,_t,1)
#define picolSubst(_i,_t) picolEval2(_i,_t,0)
int     picolEval2(picolInterp *i, char *t, int mode) { /*----------- EVAL! */
    /* mode==0: subst only, mode==1: full eval */
    picolParser p;
    char        buf[MAXSTR];
    int         rc = PICOL_OK;
    picolList args; args.size = 0;
    picolSetResult(i, "");
    picolInitParser(&p, t);
    while(1) {
        picolGetToken(i, &p);
        if (p.type == PT_EOF) break;
        if(p.end >= p.start)
            picolListAppend(&args, &p, buf);
        //___print_token(&p);
        //___print_list(&args);
        if (p.type == PT_SEP) {
            if(args.size)
                args.size -= 1;
            continue;
        }
#if 0
        else if (p.type == PT_VAR) {
            picolVar *v = picolGetVar(i,t);
            if (v && !v->val) v = picolGetGlobalVar(i,t);
            if(!v)
                return picolErr(i,"no such variable");
            t = strdup(v->val);
        } else if (p.type == PT_CMD) {
            rc = picolEval(i,t);
            if (rc != PICOL_OK) goto err;
            t = strdup(i->result);
#endif
        /* We have a complete command + args. Call it! */
        if (p.type == PT_EOL) {
            picolCmd *c;
            args.size -= 1;
            if(mode==0) { /* do a quasi-subst only */
                //rc = picolSetResult(i,picolList(buf,argc,argv)); // !!!!!!!!!!!!!!!!!!!!!!!!!!!
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
    }
err:
    return rc;
}
int picolRegisterCmd(picolInterp *i, char *name, picol_Func f) {
  picolCmd *c = picolGetCmd(i,name);
  if (c) return picolErr(i,"command already defined");
  c = malloc1(sizeof(picolCmd));
  c->name     = strdup1(name);
  c->func     = f;
  c->next     = i->commands;
  i->commands = c;
  return PICOL_OK;
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
    char buf[MAXSTR] = "";
    picolCmd *c = i->commands;
    ARITY(argc == 2 || argc == 3, "info commands");
    if(SUBCMD1("commands")) {
        for(; c; c = c->next)
          LAPPEND(buf,c->name);
    }
    return picolSetResult(i, buf);
}

picolInterp* picolCreateInterp(void) {
    picolInterp* i = malloc1(sizeof(picolInterp));
    picolInitInterp(i);
    picolRegisterCmd(i, "puts", picol_puts);
    picolRegisterCmd(i, "test", picol_test);
    picolRegisterCmd(i, "info", picol_info);
    return i;
}

#ifndef __arm__
#include "pcl_util.h"
int main(int argc, char **argv) {
    picolInterp *i = picolCreateInterp();
    register_util_cmds(i);
    printf("interpsz: %X\n", sizeof(picolInterp));
    char buf[MAXSTR] = "";
    int rc;
    while(1)
    {
        printf("picol> "); fflush(stdout);
        if (fgets(buf,sizeof(buf),stdin) == NULL) return 0;
        rc = picolEval(i, buf);
        if (i->result[0] != '\0' || rc != PICOL_OK)
            printf("[%d] %s\n", rc, i->result);
    }
}
#endif

