#include "hexdump.h"
//#include <alloca.h>

inline char hdigit(int n){return "0123456789abcdef"[n&0xf];};

#define LEN_LIMIT 8
#define SUBSTITUTE_CHAR '`'

static const char* dumpline(char*dest, int linelen, const char*src, const char*srcend)
{
    if(src>=srcend) {
        return 0;
    }
    int i;
    unsigned long s = (unsigned long)src;
    for(i=0; i<8; i++) {
        dest[i] = hdigit(s>>(28-i*4));
    }
    dest[8] = ' ';
    dest += 9;
    for(i=0; i<linelen/4 ; i++) {
        if(src+i<srcend) {
            dest[i*3] = hdigit(src[i]>>4);
            dest[i*3+1] = hdigit(src[i]);
            dest[i*3+2] = ' ';
            dest[linelen/4*3+i] = src[i] >= ' ' && src[i] < 0x7f ? src[i] : SUBSTITUTE_CHAR;
        }else{
            dest[i*3] = dest[i*3+1] = dest[i*3+2] = dest[linelen/4*3+i] = ' ';
        }
    }
    return src+i;
}

void log_dumpf(const char*fmt,const void*addr,int len,int linelen)
{
#if LEN_LIMIT
    if(len>linelen*LEN_LIMIT) {
        len=linelen*LEN_LIMIT;
    }
#endif
    linelen *= 4;
    static char _buf[4096];
    char*buf = _buf;//(char*)alloca(linelen+1); // alloca() causes the initialization to fail!!!!
    buf[linelen]=0;
    const char*start = (char*)addr;
    const char*cur = start;
    const char*end = start+len;
    while(!!(cur = dumpline(buf,linelen,cur,start+len))){DLOG(fmt,buf);}
}

void log_dump(const void*addr,int len,int linelen)
{
    log_dumpf("%s\n",addr,len,linelen);
}