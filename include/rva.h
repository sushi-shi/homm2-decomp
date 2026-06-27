// rva.h - RVA/data label macros for the binary-matching pipeline.
//
// Matched code/data carries its retail address as a clang `annotate` ATTRIBUTE.
// labels.py compiles each TU to LLVM IR and reads @llvm.global.annotations -
// pairing the MANGLED SYMBOL directly with the annotation string (no fragile
// "nearest definition" positional join).
//
//   RVA(addr, size)  - on a matched FUNCTION definition. `size` is the retail
//                      byte extent (authoritative function boundary; we have it
//                      exactly from CodeView).
//   RVAU(addr)       - matched function with NO known size ("U" = unsized).
//   SYMBOL(mangled)  - explicit mangled-name override when clang's MS-mangling
//                      differs from the retail symbol.
//   DATA(addr)       - on an `extern` decl of a matched GLOBAL.
//   SIZE(type,bytes) - file-scope sizeof assert; a REAL compile-time check under
//                      BOTH clang and MSVC 4.2 (whose sizeof is matching ground
//                      truth). Emits no code -> matching-neutral.
//   OVERRIDE         - compile-time-only check that a method overrides a base
//                      virtual (clang enforces; MSVC 4.2 has no `override`).
//
// IMPORTANT - the same source is compiled by clang (the label step) AND by MSVC
// 4.2 under wine (the base objs). MSVC 4.2 predates __attribute__, [[...]], and
// C99 variadic macros, so each macro is FIXED-arity and compiles to nothing
// under any non-clang compiler. Under MSVC the attributes vanish -> purely
// clang-side labels, never perturbing matched bytes. (SIZE is the one deliberate
// exception: a real sizeof assert active under MSVC too, still emitting no code.)
//
// IR caveat: clang only emits annotations for DEFINED globals; an `extern` decl's
// annotation is dropped from IR. So DATA labels are text-scanned by labels.py and
// bound to the clang-AST VarDecl mangledName; functions/SYMBOL go through IR.
#ifndef HOMM2_RVA_H
#define HOMM2_RVA_H

#include <Ints.h>

#ifdef __clang__

#define RVA(addr, size) __attribute__((annotate("rva:" #addr " size:" #size)))
#define RVAU(addr) __attribute__((annotate("rva:" #addr)))
#define SYMBOL(mangled) __attribute__((annotate("symbol:" #mangled)))
#define DATA(addr) __attribute__((annotate("data:" #addr)))
#define OVERRIDE override
#define SIZE(type, bytes) static_assert(sizeof(type) == (bytes), "sizeof(" #type ") != " #bytes)

#else // MSVC 4.2 (and any non-clang compiler): compile the labels out.

#define RVA(addr, size)
#define RVAU(addr)
#define SYMBOL(mangled)
#define DATA(addr)
#define OVERRIDE

// MSVC 4.2 has no static_assert: classic negative-size typedef, name uniquified
// by __LINE__ so aggregating many SIZE()s into one TU cannot clash.
#define HOMM2_SIZE_CAT_(a, b) a##b
#define HOMM2_SIZE_CAT(a, b) HOMM2_SIZE_CAT_(a, b)
#define SIZE(type, bytes)                                                                          \
    typedef char HOMM2_SIZE_CAT(homm2_size_check_, __LINE__)[(sizeof(type) == (bytes)) ? 1 : -1]

#endif

#endif // HOMM2_RVA_H
