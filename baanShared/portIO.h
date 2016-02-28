#ifndef PORTIO_H
#define PORTIO_H

#ifdef _WIN32
extern "C" short __cdecl _inp (unsigned short);
extern "C" unsigned short __cdecl _inpw (unsigned short);
extern "C" unsigned long __cdecl _inpd (unsigned short);
extern "C" int __cdecl _outp (unsigned short, int);
extern "C" unsigned short __cdecl _outpw (unsigned short, unsigned short);
extern "C" unsigned long __cdecl _outpd (unsigned short, unsigned long);

#define inportb(a) _inp(a)
#define outportb(a,b) _outp(a,b)

#else
#include <sys/io.h>

#define inportb(a) inb(a)
#define outportb(a,b) outb(b,a)

#endif


#define DATA_REG    0x378
#define STATUS_REG  0x379
#define CONTROL_REG 0x37A

#endif //PORTIO_H
