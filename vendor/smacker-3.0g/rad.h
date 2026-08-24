#ifndef __RAD__
#define __RAD__


#define RADCOPYRIGHT "Copyright (C) 1994-96 RAD Game Tools, Inc."


#ifdef __cplusplus
  #define RADDEFFUNC  extern "C"
  #define RADDEFSTART extern "C" {
  #define RADDEFEND   }
#else
  #define RADDEFFUNC
  #define RADDEFSTART
  #define RADDEFEND
#endif


#define RADLINK    __stdcall
#define RADEXPLINK __stdcall
#define RADEXPFUNC RADDEFFUNC __declspec(dllimport)


#define PTR4


typedef signed char    s8;
typedef unsigned char  u8;
typedef signed short   s16;
typedef unsigned short u16;
typedef signed int     s32;
typedef unsigned int   u32;

#endif
