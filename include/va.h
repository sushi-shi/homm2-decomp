#ifndef HOMM2_VA_H
#define HOMM2_VA_H

#include <Ints.h>

#ifdef __clang__

#define VA(addr, size) __attribute__((annotate("va:" #addr " size:" #size)))
#define VA_COMPGEN(addr, size, kind, owner)
// Where a global definition sits in the retail image, as an absolute VA. Like
// VA(...) this is audit and delinker metadata, never a placement directive:
// the compiler branch below expands it away, and the analysis branch attaches
// a clang attribute only so a marker in a position that cannot carry one is a
// parse error rather than a silently dropped claim.
#define DATA(addr) __attribute__((annotate("data:" #addr)))
#define DATA_COMPGEN(addr, name, value) value
#define DATA_COMPGEN_GUARD(addr, name, owner)
#define OVERRIDE override
#define SIZE(type, bytes) static_assert(sizeof(type) == (bytes), "sizeof(" #type ") != " #bytes)

// Where the same thing sits in the Buka target. Gold 2.1 moved code, data and
// at least one struct's fields, so the 2.1 fact is recorded beside the 2.0 one
// rather than replacing it.
#define NEW_VA(addr, size) __attribute__((annotate("new_va:" #addr " size:" #size)))
#define NEW_SIZE(type, bytes)

#else

#define VA(addr, size)
#define VA_COMPGEN(addr, size, kind, owner)
#define DATA(addr)
#define DATA_COMPGEN(addr, name, value) value
#define DATA_COMPGEN_GUARD(addr, name, owner)
#define OVERRIDE

#define SIZE(type, bytes)

#define NEW_VA(addr, size)
#define NEW_SIZE(type, bytes)

#endif

#endif
