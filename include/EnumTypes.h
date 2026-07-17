// Dual-mode integer domain declarations.
//
// Production MSVC sees integral constants plus an ABI-neutral int alias. The
// strict Clang audit sees a genuine enum and diagnoses cross-domain data flow.
#ifndef HOMM2_ENUM_TYPES_H
#define HOMM2_ENUM_TYPES_H

#include <Ints.h>

#ifdef HOMM2_STRICT_ENUM_TYPES
#define HOMM2_ENUM_BEGIN(name) typedef enum name {
#define HOMM2_ENUM_END(name) } name;
#else
#define HOMM2_ENUM_BEGIN(name) enum {
#define HOMM2_ENUM_END(name) }; typedef i32 name;
#endif

#endif // HOMM2_ENUM_TYPES_H
