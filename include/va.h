#ifndef HOMM2_VA_H
#define HOMM2_VA_H

#include <Ints.h>

#ifdef __clang__

#define VA(addr, size) __attribute__((annotate("va:" #addr " size:" #size)))
#define VA_COMPGEN(addr, size, kind, owner)
#define DATA(addr) __attribute__((annotate("data:" #addr)))
#define DATA_COMPGEN(addr, name, value) value
#define DATA_COMPGEN_GUARD(addr, name, owner)
#define VTBL(cls, va)
#define VTBL2(derived, base, va)
#define OVERRIDE override
#define SIZE(type, bytes) static_assert(sizeof(type) == (bytes), "sizeof(" #type ") != " #bytes)

#else

#define VA(addr, size)
#define VA_COMPGEN(addr, size, kind, owner)
#define DATA(addr)
#define DATA_COMPGEN(addr, name, value) value
#define DATA_COMPGEN_GUARD(addr, name, owner)
#define VTBL(cls, va)
#define VTBL2(derived, base, va)
#define OVERRIDE

#define SIZE(type, bytes)

#endif

#define OD_STEER(x) 0 [&(x)]
#define OR_STEER(x) (x | 0)

#endif
