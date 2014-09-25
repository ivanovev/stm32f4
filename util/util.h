
#ifndef __UTIL_H__
#define __UTIL_H__

#include "stdint.h"

#if 0
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
#endif

uint32_t    mr(uint32_t addr);
uint32_t    mw(uint32_t addr, uint32_t value);

char        to_upper(char myChar);
int         atoi(const char *p);
uint8_t     itoa(int32_t i, char *b);
uint32_t    htoi(char *myArray);
uint8_t     itoh(uint32_t v, char *buf, uint8_t bytes);
int32_t     str2int(const char *buf);
uint8_t     int2str(int32_t i, char *buf, uint8_t base);
void        strip_str(char *str);

int         mystrncmp(const char* s1, const char *s2, uint32_t n);
uint16_t    mystrnlen(const char* s, uint16_t maxlen);
void*       mymemcpy(void* dest, const void* src, uint32_t count);
char*       mystrncpy(char *dest, const char *src, uint32_t n);
uint32_t    mysnprintf(char *str, uint32_t sz, const char *fmt, ...);
inline uint8_t myisspace(char c);
inline uint8_t myisnewline(char c);
uint8_t     myisempty(char *str);
char*       mystrchr(const char *s, int c);
char*       mystrncat(char *dest, const char *src, uint32_t n);

#define IO_BUF_SZ 512

typedef uint16_t (*io_recv_func)(char *buf);
extern io_recv_func io_recv_str_ptr;

uint16_t io_recv_str(char *buf);

typedef void (*io_send_func)(const char *buf, uint16_t sz);
extern io_send_func io_send_str_ptr;

void io_send_str(const char *str, uint16_t n);
void io_send_str2(const char *str);
void io_send_str3(const char *str, uint8_t newline);
void io_send_str4(const char *str);
void io_send_int(int n);
void io_send_int2(const char *str, int n);
void io_send_hex(unsigned int h, uint8_t bytes);
void io_send_hex2(const char *str, uint32_t h);
void io_send_hex3(const char *str, int n, uint32_t h);
void io_send_hex4(const char *str, const uint8_t *bytes, uint32_t n);
void io_newline(void);
void io_prompt(uint8_t newline);
void io_echo(void);

#endif /* __UTIL_H__ */

