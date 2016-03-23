
#include "util/util.h"
#include "stdarg.h"

#ifndef size_t
#define size_t uint32_t
#endif

void* __wrap_memset(void *s, int c, size_t n)
{
    return mymemset(s, c, n);
}

char* __wrap_memclr(char *s, size_t n)
{
    return (char*)mymemset(s, 0, n);
}

io_send_func io_send_str_ptr = 0;
io_recv_func io_recv_str_ptr = 0;

inline uint8_t myisspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\12');
}

inline uint8_t myisnewline(char c)
{
    return (c == '\n' || c == '\r');
}

inline uint8_t myisalnum(char c)
{
    return ((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (('0' <= c) && (c <= '9')));
}

inline uint8_t myisdigit(char c)
{
    return (('0' <= c) && (c <= '9'));
}

inline uint8_t myisxdigit(char c)
{
    return (('0' <= c) && (c <= '9')) || (('A' <= c) && (c <= 'F')) || (('a' <= c) && (c <= 'f'));
}


#if 0
unsigned int mr(unsigned int addr)
{
	unsigned int base = addr & 0xFFFFFF00;
	unsigned int offset = addr & 0xFF;
	return IORD_32DIRECT(base, offset);
}

unsigned int mw(unsigned int addr, unsigned int value)
{
	unsigned int base = addr & 0xFFFFFF00;
	unsigned int offset = addr & 0xFF;
	IOWR_32DIRECT(base, offset, value);
	return value;
}
#endif

int atoi(const char *p)
{
    int k = 0, sign = 1;
    if(*p == '-')
    {
        sign = -1;
        p++;
    }
    while (myisdigit(*p)) {
        k = (k<<3)+(k<<1)+(*p)-'0';
        p++;
    }
    return sign*k;
}

uint8_t itoa(int32_t i, char *b)
{
    uint8_t ret = 0;
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i = -i;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    ret = p - b;
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return ret;
}

double atof(char* num)
{
    if (!num || !*num)
        return 0; 
    double integerPart = 0;
    double fractionPart = 0;
    int divisorForFraction = 1;
    int sign = 1;
    uint8_t inFraction = 0;
    /*Take care of +/- sign*/
    if (*num == '-')
    {
        ++num;
        sign = -1;
    }
    else if (*num == '+')
    {
        ++num;
    }
    while (*num != '\0')
    {
        if (*num >= '0' && *num <= '9')
        {
            if (inFraction)
            {
                if(fractionPart < 1000000)
                {
                    /*See how are we converting a character to integer*/
                    fractionPart = fractionPart*10 + (*num - '0');
                    divisorForFraction *= 10;
                }
            }
            else
            {
                integerPart = integerPart*10 + (*num - '0');
            }
        }
        else if (*num == '.')
        {
            if (inFraction)
                return sign * (integerPart + fractionPart/divisorForFraction);
            else
                inFraction = 1;
        }
        else
        {
            return sign * (integerPart + fractionPart/divisorForFraction);
        }
        ++num;
    }
    return sign * (integerPart + fractionPart/divisorForFraction);
}

char to_upper(char myChar)
{
    if(myChar>91) myChar-=32;
    return myChar;
}

uint32_t htoi(char *myArray)
{
    unsigned int tempInt=0, tmp = 1;
    int tempArray[8];
    int i = 0, n = 0;
    for(n = 0; myisxdigit(myArray[n]); n++);
    if(n > 8) n = 8;
    for(i = 0; i < n; tempArray[i++] = 0);

    // we are setting hex value to lone ints
    for(i=0; i<n; i++) {
      // if lower case, then change to upper case to deal with
      if(myArray[i]>'9') {
        myArray[i] = to_upper(myArray[i]);
        // if it's a letter make it a whole number
        tempArray[i] = (myArray[i]-55);
        // if it's a character number, make a value of that number
      } else {
        // else we got a char of a number, make it a real number
        tempArray[i] = (myArray[i] - 48);
      }
    }

    // we multiply ints together to get final int
    for(i = n - 1; i >= 0; i--)
    {
        tempInt += tempArray[i]*tmp;
        tmp *= 16;
    }

    return tempInt;
}

uint8_t itoh_(uint32_t v, char *buf, uint8_t bytes)
{
	uint8_t i = 0;
	int digit_shift, digit, c;
	/* Find first non-zero digit. */
    if(bytes > 0)
        digit_shift = (bytes - 1)*8 + 4;
    else
    {
        digit_shift = 28;
        while (!(v & (0xF << digit_shift)))
            digit_shift -= 4;
    }

	/* Print digits. */
	for (; digit_shift >= 0; digit_shift -= 4)
	{
		digit = (v & (0xF << digit_shift)) >> digit_shift;
		if (digit <= 9)
			c = '0' + digit;
		else
			c = 'A' + digit - 10;
		buf[i++] = c;
	}
	buf[i] = 0;
	return i;
}

uint8_t itoh(uint32_t v, char *buf, uint8_t bytes)
{
    buf[0] = '0';
    buf[1] = 'x';
    return itoh_(v, &(buf[2]), bytes);
}

uint8_t int2str(int32_t i, char *buf, uint8_t base, uint8_t len)
{
    char buf1[10];
    int len1, j;
    if(base == 10)
    {
        itoa(i, buf1);
        len1 = mystrnlen(buf1, sizeof(buf1));
        if(len)
        {
            for(j = len1; j < len; j++)
                *buf++ = '0';
        }
        mystrncpy(buf, buf1, len1);
        return len;
    }
    else if(base == 16)
        return itoh(i, buf, len);
    else
        return 0;
}

int double2str(char *buf, int len, double f, char *pr)
{
    unsigned int d1, d2, i;
    double af = (f >= 0) ? f : -f;
    d1 = af;
    double f2 = af - d1;
    double f3 = 1;
    int p = 0, half = 0;
    char *ptr = buf;
    if(!mystrncmp(pr, "0.5", 3))
    {
        p = 1;
        half = 1;
    }
    else
        p = str2int(pr);
    if(p <= 0)
    {
        return mysnprintf(buf, len, "%d", (int)(f + .5));
    }
    if(p >= 6) p = 6;
    for(i = 0; i < p; i++)
    {
        f2 *= 10;
        f3 *= 10;
    }
    if(half == 0)
    {
        f2 += 0.5;
        if(f2 >= f3)
        {
            f2 = 0;
            d1 += 1;
        }
    }
    else
    {
        if((f2 + 2.5) >= f3)
        {
            f2 = 0;
            d1 += 1;
        }
        if((f2 - 2.5) <= (f3 - 10))
            f2 = 0;
        else
            f2 = 5;

    }
    for(d2 = f2; d2 ? !(d2 % 10) : 0; d2 /= 10, p--);
    char fmt[16];
    if(f < 0)
        ptr += mysnprintf(ptr, len - (ptr - buf), "-");
    ptr += mysnprintf(ptr, len - (ptr - buf), "%d", d1);
    if(!d2)
        return (ptr - buf);
    ptr += mysnprintf(ptr, len - (ptr - buf), ".");
    ptr += int2str(d2, ptr, 10, p);
    //ptr += mysnprintf(ptr, len - (ptr - buf), "%d", d2);
    return (ptr - buf);
}

int32_t str2int(const char *str)
{
    const char *ptr = str + 2;
    if(!mystrncmp(str, "0x", 2))
        return (int32_t)htoi((char*)ptr);
    return atoi(str);
}

int str2bytes(const char *in, uint8_t *out, int maxbytes)
{
    int len;
    char ch[3] = {0, 0, 0};
    const char *tmp;
    if(!mystrncmp(in, "0x", 2))
        in += 2;
    tmp = in;
    len = mystrnlen(tmp, 2*maxbytes);
    if(maxbytes > len/2)
        maxbytes = len/2;
    for(len = 0; (tmp - in) < (2*maxbytes); tmp++)
    {
        ch[0] = tmp[0];
        ch[1] = tmp[1];
        if(('0' <= ch[0]) && (ch[0] <= 'F') &&
                ('0' <= ch[1]) && (ch[1] <= 'F'))
        {
            out[len++] = htoi(ch);
            tmp++;
        }
        else
            break;
    }
    out[len] = 0;
    return len;
}

uint16_t bytes2str(const char *in, char *out, uint16_t sz)
{
    uint16_t i = 0;
    out[0] = '0';
    out[1] = 'x';
    for(i = 0; i < sz; i++)
    {
        itoh_(in[i], (char*)&out[2*i+2], 1);
    }
    return sz;
}

void strip_str(char *str)
{
    uint16_t len = mystrnlen(str, IO_BUF_SZ);
    for(; (str[len-1] == '\n') || (str[len-1] == '\r'); len--)
        str[len-1] = 0;
}

int clip(int min, int value, int max)
{
    return MAX(min, MIN(value, max));
}

uint16_t mystrnlen(const char *array, uint16_t maxlen)
{
    uint16_t i;
    for(i = 0; i < maxlen; i++)
    {
        if(!array[i])
            return i;
    }
    return maxlen;
}

int mystrncmp(const char* s1, const char* s2, uint32_t n)
{
#if 1
    for ( ; n > 0; s1++, s2++, --n)
    {
        if (*s1 != *s2)
            return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
        else if (*s1 == '\0')
            return 0;
    }
    return 0;
#else
    while(*s1 && (*s1==*s2) && n--)
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
#endif
}

char* mystrncpy(char *dest, const char *src, uint32_t n)
{
    uint32_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;
}

void* mymemcpy(void *dst, const void *src, uint32_t n)
{
    if((uintptr_t)dst % sizeof(long) == 0 &&
       (uintptr_t)src % sizeof(long) == 0 &&
       n % sizeof(long) == 0)
    {
        uint32_t i;
        long *d = dst;
        const long *s = src;
        for (i=0; i<n/sizeof(long); i++)
        {
            d[i] = s[i];
        }
    }
    else
    {
        char* dst8 = (char*)dst;
        char* src8 = (char*)src;
        while (n--)
            *dst8++ = *src8++;
    }
    return dst;
}

int mymemcmp(const void *s1, const void *s2, uint32_t n)
{
    int v = 0;
    uint8_t *p1 = (uint8_t*)s1;
    uint8_t *p2 = (uint8_t*)s2;
    while(n-- > 0 && v == 0) {
        v = *(p1++) - *(p2++);
    }
    return v;
}

uint32_t mysnprintf(char *buf, uint32_t sz, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const char *w;
    uint32_t i = 0;
    char pad = '0';
    int8_t width = 4;
    char c;

    w = fmt;
    while (((c = *w++) != 0) && (i < sz))
    {
        if (c != '%')
        {
            buf[i++] = c;
        }
        else
        {
            if ((c = *w++) != 0)
            {
#if 0
                if (c == '.')
                {
                    hexsz = *w++ - '0';
                    if((hexsz <= 0) || (4 < hexsz))
                        hexsz = 4;
                    c = *w++;
                }
#endif
                if((c == '0') || (c == ' '))
                {
                    pad = c;
                    c = *w++;
                }
                if(myisdigit(c))
                {
                    width = atoi(w-1);
                    while(myisdigit(c = *w++));
                }
                if (c == '%')
                {
                    buf[i++] = c;
                }
                else if (c == 'd')
                {
                    unsigned long v = va_arg(args, unsigned long);
                    if (v == 0)
                    {
                        buf[i++] = '0';
                        continue;
                    }
                    else
                    {
                        if((sz - i) > 10)
                            i += itoa(v, &(buf[i]));
                    }
                }
                else if((c == 'x') || (c == 'X'))
                {
                    unsigned long v = va_arg(args, unsigned long);

                    if((sz - i) > width)
                        i += itoh_(v, &(buf[i]), width/2);
                    width = 4;
                }
                else if (c == 's')
                {
                    char *s = va_arg(args, char *);
                    while(*s && (i < sz))
                        buf[i++] = *s++;
                }
            }
            else
            {
                break;
            }
        }
    }
    buf[i] = 0;
    return i;
}

uint8_t myisempty(char *str)
{
    if(str[0] == 0)
        return 1;
    if(str[0] == '\n')
        return 1;
    if(str[0] == '\r')
        return 1;
    return 0;
}

char *mystrchr(const char *s, int c)
{
    const char ch = c;
    for ( ; *s != ch; s++)
        if (*s == '\0')
            return 0;
    return (char *)s;
}

char* mystrncat(char *dest, const char *src, uint32_t n)
{
    uint32_t dest_len = mystrnlen(dest, IO_BUF_SZ);
    uint32_t i;

    for (i = 0 ; i < n && src[i] != '\0' ; i++)
        dest[dest_len + i] = src[i];
    dest[dest_len + i] = '\0';

    return dest;
}

uint8_t* mymemchr(const uint8_t *s, uint8_t c, uint32_t n)
{
    if (n != 0) {
        const uint8_t *p = s;
        do {
            if (*p++ == c)
                return ((uint8_t*)(p - 1));
        } while (--n != 0);
    }
    return 0;
}

void* mymemset(void *s, int c, uint32_t n)
{
    uint8_t *p = s;
    while(n--)
        *p++ = (uint8_t)c;
    return s;
}

uint16_t io_recv_str(char *buf)
{
    if(io_recv_str_ptr)
        return io_recv_str_ptr(buf);
    return 0;
}

void io_send_str(const char *str, uint16_t n)
{
    if(io_send_str_ptr)
        io_send_str_ptr(str, n);
}

void io_send_str2(const char *str)
{
    io_send_str(str, mystrnlen(str, IO_BUF_SZ));
}

void io_send_str3(const char *str, uint8_t newline)
{
    if(newline)
        io_newline();
    io_send_str2(str);
}

void io_send_str4(const char *str)
{
    uint16_t i;
    for(i = mystrnlen(str, IO_BUF_SZ) - 1; i > 0; i--)
    {
        if(!myisnewline(str[i]))
            break;
    }
    if(i || !myisnewline(str[0]))
        io_send_str(str, i + 1);
}

void io_send_int(int n)
{
    char buf[10];
    itoa(n, buf);
    io_send_str2(buf);
}

void io_send_int2(const char *str, int n)
{
    io_send_str3(str, 1);
    io_send_str2(": ");
    io_send_int(n);
}

void io_send_hex(unsigned int h, uint8_t bytes)
{
    char buf[15];
    itoh(h, buf, bytes);
    io_send_str2(buf);
}

void io_send_hex2(const char *str, uint32_t h)
{
    io_send_str3(str, 1);
    io_send_str2(": ");
    io_send_hex(h, 4);
}

void io_send_hex4(const char *str, const uint8_t *bytes, uint32_t n)
{
    uint32_t i;
    io_send_str3(str, 1);
    io_send_str2("(");
    io_send_int(n);
    io_send_str2("): ");
    for(i = 0; i < n; i++)
    {
        io_send_hex(bytes[i], 1);
        io_send_str2(" ");
    }
}

void io_newline(void)
{
    io_send_str("\n\r", 2);
}

void io_prompt(uint8_t newline, const char *prefix1, const char *prefix2)
{
    if(newline)
        io_newline();
    uint8_t i;
    const char *pp[2] = {prefix1, prefix2}, *p;
    for(i = 0; i <= sizeof(pp)/sizeof(pp[0]); i++)
    {
        p = pp[i];
        if(p)
        {
            if(p[0])
            {
                io_send_str(p, mystrnlen(p, 32));
                io_send_str(" ", 1);
            }
        }
    }
    io_send_str("#> ", 3);
}

void io_echo(void)
{
    char buf[IO_BUF_SZ];
    uint16_t sz = io_recv_str(buf);
    if(sz)
    {
        if(!myisnewline(buf[0]))
        {
            io_send_str4(buf);
            io_prompt(1, 0, 0);
        }
        else
            io_prompt(0, 0, 0);
    }
}

void format_time(char *buf, uint16_t sz, uint32_t t)
{
    int ss = t % 60;
    t /= 60;
    int mm = t % 60;
    t /= 60;
    int hh = t % 24;
    t /= 24;
    int dd = t;
    if(dd)
        mysnprintf(buf, sz, "%dd %dh %dm %ds", dd, hh, mm, ss);
    else
        mysnprintf(buf, sz, "%dh %dm %ds", hh, mm, ss);
}

