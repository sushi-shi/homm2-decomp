// va.h - VA/data label macros for the binary-matching pipeline.
//
// Matched code/data carries its retail address as a clang `annotate` ATTRIBUTE.
// The address is a VA (image-base-included, e.g. 0x0040b070) - the value a
// disassembler shows. (symbol_names.csv's rva column is image-base-RELATIVE;
// tooling that joins the two bridges by the image base.)
//
//   VA(addr, size)   - on a matched FUNCTION definition. `size` is the retail
//                      byte extent (authoritative function boundary; we have it
//                      exactly from CodeView).
//   VAU(addr)        - matched function with NO known size ("U" = unsized).
//   SYMBOL(mangled)  - explicit mangled-name override when clang's MS-mangling
//                      differs from the retail symbol.
//   DATA(addr)       - on an `extern` decl of a matched GLOBAL (also a VA).
//   VTBL(cls, va)    - census marker at a TU's tail: "this TU emits class `cls`'s vtable at
//                      VA `va`". Vtables are compiler-emitted (not hand-written), so this is a
//                      pure source-level claim the vtable gate cross-checks against CodeView +
//                      the emitted objects. Expands to NOTHING under clang AND MSVC.
//   SIZE(type,bytes) - file-scope sizeof assert; a REAL compile-time check under
//                      BOTH clang and MSVC 4.2 (whose sizeof is matching ground
//                      truth). Emits no code -> matching-neutral.
//   OVERRIDE         - compile-time-only check that a method overrides a base
//                      virtual (clang enforces; MSVC 4.2 has no `override`).
//
// IMPORTANT - the same source is compiled by clang (editor tooling) AND by MSVC
// 4.2 under wine (the base objs). MSVC 4.2 predates __attribute__, [[...]], and
// C99 variadic macros, so each macro is FIXED-arity and compiles to nothing
// under any non-clang compiler. Under MSVC the attributes vanish -> never
// perturbing matched bytes. (SIZE is the one deliberate exception: a real sizeof
// assert active under MSVC too, still emitting no code.)
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

#else // MSVC 4.2 (and any non-clang compiler): compile the labels out.

#define VA(addr, size)
#define VAU(addr)
#define SYMBOL(mangled)
#define DATA(addr)
#define VTBL(cls, va)
#define OVERRIDE

// MSVC 4.2 has no static_assert: classic negative-size typedef, name uniquified
// by __LINE__ so aggregating many SIZE()s into one TU cannot clash.
#define HOMM2_SIZE_CAT_(a, b) a##b
#define HOMM2_SIZE_CAT(a, b) HOMM2_SIZE_CAT_(a, b)
#define SIZE(type, bytes)                                                                          \
    typedef char HOMM2_SIZE_CAT(homm2_size_check_, __LINE__)[(sizeof(type) == (bytes)) ? 1 : -1]

#endif

#endif // HOMM2_VA_H
