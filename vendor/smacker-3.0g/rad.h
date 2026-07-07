#ifndef __RAD__
#define __RAD__
/*==========================================================================*/
/*  rad.h  --  RAD Game Tools shared macros/types (Smacker for Win32 SDK).   */
/*                                                                           */
/*  Copyright (C) 1994-96 RAD Game Tools, Inc.                               */
/*                                                                           */
/*  RECONSTRUCTED, minimal, Win32/MSVC-only slice of RAD's rad.h, sized for  */
/*  the Smacker 3.0g core API surface that HEROES2W.EXE (Price of Loyalty,   */
/*  1996) links against (smackw32.DLL, imported by ordinal).                 */
/*                                                                           */
/*  The full retail rad.h is a large multi-platform (DOS/Win16/Win386/Mac/   */
/*  Watcom) file full of inline-asm intrinsics.  Only the Win32 32-bit MSVC  */
/*  codepath is exercised here, so this reproduces just that path:           */
/*      __RADNT__  ->  RADEXPLINK = __stdcall                                */
/*                     RADEXPFUNC = extern "C" __declspec(dllimport)         */
/*  which is exactly the calling convention + import decoration the retail   */
/*  smackw32 thunks use (the observed `call dword ptr [__imp__Smack*@N]`).   */
/*==========================================================================*/

#define RADCOPYRIGHT "Copyright (C) 1994-96 RAD Game Tools, Inc."

/* C-linkage wrappers (extern "C" so exports keep their _Smack*@N C names). */
#ifdef __cplusplus
  #define RADDEFFUNC  extern "C"
  #define RADDEFSTART extern "C" {
  #define RADDEFEND   }
#else
  #define RADDEFFUNC
  #define RADDEFSTART
  #define RADDEFEND
#endif

/* Win32 (__RADNT__) calling convention + DLL import decoration. */
#define RADLINK    __stdcall
#define RADEXPLINK __stdcall
#define RADEXPFUNC RADDEFFUNC __declspec(dllimport)

/* 32-bit target: no far pointers. */
#define PTR4

/*
 * RAD's fixed-width aliases.  The retail rad.h `#define`s these (u32 ==
 * `unsigned long`); we use benign typedefs whose UNDERLYING types are chosen
 * to be *identical* to include/Ints.h's (u8/u16/u32) so that both headers can
 * appear in one TU without a typedef-redefinition error (C++ permits an
 * identical typedef repeat, but not `#define u32 ...` over Ints.h's typedef).
 * On this target `unsigned int` == `unsigned long` (4 bytes) and every Smacker
 * export has C linkage, so the int-vs-long choice is codegen- and symbol-
 * neutral.  s8/s16/s32 are unique to this header (Ints.h names them i8/i16/i32).
 */
typedef signed char    s8;
typedef unsigned char  u8;
typedef signed short   s16;
typedef unsigned short u16;
typedef signed int     s32;
typedef unsigned int   u32;

#endif /* __RAD__ */
