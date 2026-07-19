#ifndef HOMM2_VA_H
#define HOMM2_VA_H

#include <Ints.h>

#ifdef __clang__

#define VA(addr, size) __attribute__((annotate("va:" #addr " size:" #size)))
#define VAU(addr) __attribute__((annotate("va:" #addr)))
#define SYMBOL(mangled) __attribute__((annotate("symbol:" #mangled)))
#define DATA(addr) __attribute__((annotate("data:" #addr)))
#define VTBL(cls, va)
#define OVERRIDE override
#define SIZE(type, bytes) static_assert(sizeof(type) == (bytes), "sizeof(" #type ") != " #bytes)

#else

#define VA(addr, size)
#define VAU(addr)
#define SYMBOL(mangled)
#define DATA(addr)
#define VTBL(cls, va)
#define OVERRIDE

#define SIZE(type, bytes)

#endif

#define OD_STEER(x) 0 [&(x)]

#endif
