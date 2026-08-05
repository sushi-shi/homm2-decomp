
/*-----------------------------------------------------------*/
/*--- A block-sorting, lossless compressor         bzip.c ---*/
/*-----------------------------------------------------------*/

/*--
  This program is BZIP, a lossless, block-sorting data compressor,
  version 0.21, dated 25-August-1996.

  Copyright (C) 1996 by Julian Seward.
     Department of Computer Science, University of Manchester,
     Oxford Road, Manchester M13 9PL, UK.
     email: sewardj@cs.man.ac.uk

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  The GNU General Public License is contained in the file LICENSE.

  This program is based on (at least) the work of:
     Mike Burrows
     David Wheeler
     Peter Fenwick
     Alistair Moffat
     Radford Neal
     Ian H. Witten

  For more information on these sources, see the file ALGORITHMS.
--*/

/*----------------------------------------------------*/
/*--- IMPORTANT                                    ---*/
/*----------------------------------------------------*/

/*--
   WARNING:
      This program (attempts to) compress data by performing several
      non-trivial transformations on it.  Unless you are 100% familiar
      with *all* the algorithms contained herein, and with the
      consequences of modifying them, you should NOT meddle with the
      compression or decompression machinery.  Incorrect changes can
      and very likely *will* lead to disasterous loss of data.

   DISCLAIMER:
      I TAKE NO RESPONSIBILITY FOR ANY LOSS OF DATA ARISING FROM THE
      USE OF THIS PROGRAM, HOWSOEVER CAUSED.

      Every compression of a file implies an assumption that the
      compressed file can be decompressed to reproduce the original.
      Great efforts in design, coding and testing have been made to
      ensure that this program works correctly.  However, the
      complexity of the algorithms, and, in particular, the presence
      of various special cases in the code which occur with very low
      but non-zero probability make it impossible to rule out the
      possibility of bugs remaining in the program.  DO NOT COMPRESS
      ANY DATA WITH THIS PROGRAM UNLESS YOU ARE PREPARED TO ACCEPT THE
      POSSIBILITY, HOWEVER SMALL, THAT THE DATA WILL NOT BE RECOVERABLE.

      That is not to say this program is inherently unreliable.
      Indeed, I very much hope the opposite is true.  BZIP has been
      carefully constructed and extensively tested.
--*/



/*----------------------------------------------------*/
/*--- and now for something much more pleasant :-) ---*/
/*----------------------------------------------------*/

/*---------------------------------------------*/
/*--
  Place a 1 beside your platform, and 0 elsewhere.
--*/

#define BZ_UNIX_32          1  /*-- generic 32-bit Unix          --*/
#define BZ_UNIX_64          0  /*-- generic 64-bit Unix          --*/
#define BZ_WIN32_BORLANDC50 0  /*-- Win32/95/NT, Borland C++ 5.0 --*/
#define BZ_WIN32_MSVC20     0  /*-- Win32/95/NT, MS VC++ 2.0     --*/ 

#define BZ_UNIX (BZ_UNIX_32 | BZ_UNIX_64)


/*---------------------------------------------*/
/*--
  Some stuff for all platforms.
--*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define ERROR_IF_EOF(i)       { if ((i) == EOF)  ioError(); }
#define ERROR_IF_NOT_ZERO(i)  { if ((i) != 0)    ioError(); }
#define ERROR_IF_MINUS_ONE(i) { if ((i) == (-1)) ioError(); }


/*---------------------------------------------*/
/*--
   Platform-specific stuff.
--*/

#if BZ_UNIX_32
   #include <utime.h>
   #include <unistd.h>
   #include <malloc.h>
   #include <sys/stat.h>
   #include <sys/times.h>

   #define Int32   int
   #define UInt32  unsigned int
   #define Char    char
   #define UChar   unsigned char

   #define PATH_SEP    '/'
   #define MY_LSTAT    lstat
   #define MY_S_IFREG  S_ISREG
   #define MY_STAT     stat

   #define APPEND_FILESPEC(root, name) \
      root=snocString((root), (name))

   #define SET_BINARY_MODE(fd) /**/

   /*--
      You should try very hard to persuade your C compiler
      to inline the bits marked INLINE.  Otherwise bzip will
      run rather slowly.  gcc version 2.x is recommended.
   --*/
   #ifdef __GNUC__
      #define INLINE   inline
      #define NORETURN __attribute__ ((noreturn))
   #else
      #define INLINE   /**/
      #define NORETURN /**/
   #endif   
#endif


#if BZ_UNIX_64
   Any volunteers?  If so, please mail me the relevant bits.
#endif


#if BZ_WIN32_BORLANDC50
   #include <dir.h>
   #include <io.h>
   #include <fcntl.h>
   #include <sys\stat.h>
   #include <utime.h>

   #define Int32   int
   #define UInt32  unsigned int
   #define Char    char
   #define UChar   unsigned char

   #define INLINE         /**/
   #define NORETURN       /**/
   #define PATH_SEP       '\\'
   #define MY_S_IFREG(x)  ((x) & S_IFREG)
   #define MY_LSTAT       stat
   #define MY_STAT        stat

   #define APPEND_FILESPEC(root, spec)              \
      do {                                          \
         if ((spec)[0] == '-') {                    \
            root = snocString((root), (spec));      \
         } else {                                   \
            struct ffblk ffblk;                     \
            int done;                               \
            done = findfirst((spec), &ffblk, 0);    \
            if ( done ) {                           \
               root = snocString ((root), (spec));  \
            } else {                                \
               while ( !done ) {                    \
                  root = snocString((root),         \
                            &ffblk.ff_name[0]);     \
                  done = findnext(&ffblk);          \
               }                                    \
            }                                       \
         }                                          \
      } while ( 0 )

   #define SET_BINARY_MODE(fd)                     \
      do {                                         \
         int retVal = setmode ( fileno ( fd ),     \
                               O_BINARY );         \
         ERROR_IF_MINUS_ONE ( retVal );            \
      } while ( 0 )

#endif


#if BZ_WIN32_MSVC20
   #include <io.h>
   #include <fcntl.h>
   #include <sys\stat.h>
   #include <sys\utime.h>

   #define Int32   int
   #define UInt32  unsigned int
   #define Char    char
   #define UChar   unsigned char

   #define INLINE         /**/
   #define NORETURN       /**/
   #define PATH_SEP       '\\'
   #define MY_LSTAT       _stat
   #define MY_STAT        _stat
   #define MY_S_IFREG(x)  ((x) & _S_IFREG)

   #define APPEND_FILESPEC(root, spec)                \
      do {                                            \
         if ((spec)[0] == '-') {                      \
            root = snocString((root), (spec));        \
         } else {                                     \
            struct _finddata_t c_file;                \
            long hFile;                               \
            hFile = _findfirst((spec), &c_file);      \
            if ( hFile == -1L ) {                     \
               root = snocString ((root), (spec));    \
            } else {                                  \
               int anInt = 0;                         \
               while ( anInt == 0 ) {                 \
                  root = snocString((root),           \
                            &c_file.name[0]);         \
                  anInt = _findnext(hFile, &c_file);  \
               }                                      \
            }                                         \
         }                                            \
      } while ( 0 )

   #define SET_BINARY_MODE(fd)                        \
      do {                                            \
         int retVal = setmode ( fileno ( fd ),        \
                               O_BINARY );            \
         ERROR_IF_MINUS_ONE ( retVal );               \
      } while ( 0 )

#endif


/*---------------------------------------------*/
/*--
  Some more stuff for all platforms :-)
--*/

#define Bool   int
#define True   1
#define False  0

/*--
  IntNative is your platform's `native' int size.
  Only here to avoid probs with 64-bit platforms.
--*/
#define IntNative int


/*--
   change to 1, or compile with -DDEBUG=1 to debug
--*/
#ifndef DEBUG
#define DEBUG 0   
#endif


/*---------------------------------------------------*/
/*---                                             ---*/
/*---------------------------------------------------*/

/*-- 
   Implementation notes, July 1996
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   Memory allocation
   ~~~~~~~~~~~~~~~~~ 
   All large data structures are allocated on the C heap,
   for better or for worse.  That includes the various 
   arrays of pointers, striped words, bytes and frequency
   tables for compression and decompression.  The only non-heap
   allocated structures are the coding models and the 
   BitStream structures, but these are both small.

   BZIP can operate at various block-sizes, ranging from
   100k to 900k in 100k steps, and it allocates only as
   much as it needs to.  When compressing, we know from the
   command-line options what the block-size is going to be,
   so all allocation can be done at start-up; if that
   succeeds, there can be no further allocation problems.

   Decompression is more complicated.  Each compressed file
   contains, in its header, a byte indicating the block
   size used for compression.  This means BZIP potentially
   needs to reallocate memory for each file it deals with,
   which in turn opens the possibility for a memory allocation
   failure part way through a run of files, by encountering
   a file requiring a much larger block size than all the
   ones preceding it.

   The policy is to simply give up if a memory allocation
   failure occurs.  During decompression, it would be
   possible to move on to subsequent files in the hope that
   some might ask for a smaller block size, but the
   complications for doing this seem more trouble than they
   are worth.


   Compressed file formats
   ~~~~~~~~~~~~~~~~~~~~~~~
   Perhaps the most important point is that compressed
   files start with a 4-byte preamble:

      'B' 'Z'     -- a crude `magic number'

      '0'         -- file format version

      '1' to '9'  -- block size indicator

   The third byte gives a trapdoor mechanism so that we can
   change file formats and/or algorithm later, without
   losing backwards compatibility.  The fourth byte
   indicates the compression block size; '1' stands for
   100k, '2' for 200k, &c.

   In the present file format (call it version '0') *all*
   material after this 4-byte preamble is written via the
   arithmetic coder, using various different models,
   including the `bogus' non-adaptive 256-entry model used
   to send bytes through the arithmetic coder.  The overall
   structure of this part is a sequence of blocks, each of 
   the format

      origPtr    run-length-coded MTF values   EOB

   origPtr is a 32-bit number (sent via bogusModel)
   indicating the position in the sorted block of the
   un-rotated original string.  A negative value of
   origPtr indicates that this is the last block.

   Finally, after all the blocks, there is a 32-bit 
   CRC value, again sent using the `bogus' model.
   The CRC applies to the entirety of the uncompressed 
   data stream.

   The MTF values are coded exactly as with Fenwick's
   `structured model', augmented with Wheeler's run-length
   coding scheme for zeros.  The basis model has an extra
   symbol, EOB, denoting the end of the block; if that is
   not encountered within the block size indicated by the
   preamble, something is wrong.


   Error conditions
   ~~~~~~~~~~~~~~~~
   Dealing with error conditions is the least satisfactory
   aspect of BZIP.  The policy is to try and leave the
   filesystem in a consistent state, then quit, even if it
   means not processing some of the files mentioned in the
   command line.  `A consistent state' means that a file
   exists either in its compressed or uncompressed form,
   but not both.  This boils down to the rule `delete the
   output file if an error condition occurs, leaving the
   input intact'.  Input files are only deleted when we can
   be pretty sure the output file has been written and
   closed successfully.
   
   Errors are a dog because there's so many things to 
   deal with.  The following can happen mid-file, and
   require cleaning up.

     internal `panics' -- indicating a bug
     corrupted compressed file -- block overrun in MTF decode
     bad magic number or version on compressed file
     can't allocate enough memory to decompress this file
     I/O error reading/writing/opening/closing
     signal catches -- Control-C, SIGTERM, SIGHUP.

   Other conditions, primarily pertaining to file names,
   can be checked in-between files, which makes dealing
   with them easier.
--*/

  


/*---------------------------------------------*/

Int32   bytesIn, bytesOut;
Bool    verbose, veryVerbose;
Bool    compressing, keepInputFiles;
UInt32  globalCrc;

#define OM_FILES_TO_FILES   1
#define OM_FILE_TO_STDOUT   2
#define OM_STDIN_TO_STDOUT  3
Int32   opMode;

Int32   longestFileName;
Char    inName[1024];
Char    outName[1024];
Char    *progName;
Char    progNameReally[1024];
FILE    *outputHandleJustInCase;

void    panic                 ( Char* )          NORETURN;
void    ioError               ( void )           NORETURN;
void    compressOutOfMemory   ( Int32, Int32 )   NORETURN;
void    uncompressOutOfMemory ( Int32, Int32 )   NORETURN;
void    blockOverrun          ( void )           NORETURN;
void    unblockError          ( void )           NORETURN;
void    crcError              ( UInt32, UInt32 ) NORETURN;
void    bitStreamEOF          ( void )           NORETURN;
void    cleanUpAndFail        ( void )           NORETURN;
void    compressedStreamEOF   ( void )           NORETURN;


/*---------------------------------------------------*/
/*--- 32-bit CRC grunge                           ---*/
/*---------------------------------------------------*/

/*--
  I think this is an implementation of the AUTODIN-II,
  Ethernet & FDDI 32-bit CRC standard.  Vaguely derived
  from code by Rob Warnock, in Section 51 of the 
  comp.compression FAQ.
--*/

UInt32 crc32Table[256] = {

   /*-- Ugly, innit? --*/

   0x00000000L, 0x04c11db7L, 0x09823b6eL, 0x0d4326d9L,
   0x130476dcL, 0x17c56b6bL, 0x1a864db2L, 0x1e475005L,
   0x2608edb8L, 0x22c9f00fL, 0x2f8ad6d6L, 0x2b4bcb61L,
   0x350c9b64L, 0x31cd86d3L, 0x3c8ea00aL, 0x384fbdbdL,
   0x4c11db70L, 0x48d0c6c7L, 0x4593e01eL, 0x4152fda9L,
   0x5f15adacL, 0x5bd4b01bL, 0x569796c2L, 0x52568b75L,
   0x6a1936c8L, 0x6ed82b7fL, 0x639b0da6L, 0x675a1011L,
   0x791d4014L, 0x7ddc5da3L, 0x709f7b7aL, 0x745e66cdL,
   0x9823b6e0L, 0x9ce2ab57L, 0x91a18d8eL, 0x95609039L,
   0x8b27c03cL, 0x8fe6dd8bL, 0x82a5fb52L, 0x8664e6e5L,
   0xbe2b5b58L, 0xbaea46efL, 0xb7a96036L, 0xb3687d81L,
   0xad2f2d84L, 0xa9ee3033L, 0xa4ad16eaL, 0xa06c0b5dL,
   0xd4326d90L, 0xd0f37027L, 0xddb056feL, 0xd9714b49L,
   0xc7361b4cL, 0xc3f706fbL, 0xceb42022L, 0xca753d95L,
   0xf23a8028L, 0xf6fb9d9fL, 0xfbb8bb46L, 0xff79a6f1L,
   0xe13ef6f4L, 0xe5ffeb43L, 0xe8bccd9aL, 0xec7dd02dL,
   0x34867077L, 0x30476dc0L, 0x3d044b19L, 0x39c556aeL,
   0x278206abL, 0x23431b1cL, 0x2e003dc5L, 0x2ac12072L,
   0x128e9dcfL, 0x164f8078L, 0x1b0ca6a1L, 0x1fcdbb16L,
   0x018aeb13L, 0x054bf6a4L, 0x0808d07dL, 0x0cc9cdcaL,
   0x7897ab07L, 0x7c56b6b0L, 0x71159069L, 0x75d48ddeL,
   0x6b93dddbL, 0x6f52c06cL, 0x6211e6b5L, 0x66d0fb02L,
   0x5e9f46bfL, 0x5a5e5b08L, 0x571d7dd1L, 0x53dc6066L,
   0x4d9b3063L, 0x495a2dd4L, 0x44190b0dL, 0x40d816baL,
   0xaca5c697L, 0xa864db20L, 0xa527fdf9L, 0xa1e6e04eL,
   0xbfa1b04bL, 0xbb60adfcL, 0xb6238b25L, 0xb2e29692L,
   0x8aad2b2fL, 0x8e6c3698L, 0x832f1041L, 0x87ee0df6L,
   0x99a95df3L, 0x9d684044L, 0x902b669dL, 0x94ea7b2aL,
   0xe0b41de7L, 0xe4750050L, 0xe9362689L, 0xedf73b3eL,
   0xf3b06b3bL, 0xf771768cL, 0xfa325055L, 0xfef34de2L,
   0xc6bcf05fL, 0xc27dede8L, 0xcf3ecb31L, 0xcbffd686L,
   0xd5b88683L, 0xd1799b34L, 0xdc3abdedL, 0xd8fba05aL,
   0x690ce0eeL, 0x6dcdfd59L, 0x608edb80L, 0x644fc637L,
   0x7a089632L, 0x7ec98b85L, 0x738aad5cL, 0x774bb0ebL,
   0x4f040d56L, 0x4bc510e1L, 0x46863638L, 0x42472b8fL,
   0x5c007b8aL, 0x58c1663dL, 0x558240e4L, 0x51435d53L,
   0x251d3b9eL, 0x21dc2629L, 0x2c9f00f0L, 0x285e1d47L,
   0x36194d42L, 0x32d850f5L, 0x3f9b762cL, 0x3b5a6b9bL,
   0x0315d626L, 0x07d4cb91L, 0x0a97ed48L, 0x0e56f0ffL,
   0x1011a0faL, 0x14d0bd4dL, 0x19939b94L, 0x1d528623L,
   0xf12f560eL, 0xf5ee4bb9L, 0xf8ad6d60L, 0xfc6c70d7L,
   0xe22b20d2L, 0xe6ea3d65L, 0xeba91bbcL, 0xef68060bL,
   0xd727bbb6L, 0xd3e6a601L, 0xdea580d8L, 0xda649d6fL,
   0xc423cd6aL, 0xc0e2d0ddL, 0xcda1f604L, 0xc960ebb3L,
   0xbd3e8d7eL, 0xb9ff90c9L, 0xb4bcb610L, 0xb07daba7L,
   0xae3afba2L, 0xaafbe615L, 0xa7b8c0ccL, 0xa379dd7bL,
   0x9b3660c6L, 0x9ff77d71L, 0x92b45ba8L, 0x9675461fL,
   0x8832161aL, 0x8cf30badL, 0x81b02d74L, 0x857130c3L,
   0x5d8a9099L, 0x594b8d2eL, 0x5408abf7L, 0x50c9b640L,
   0x4e8ee645L, 0x4a4ffbf2L, 0x470cdd2bL, 0x43cdc09cL,
   0x7b827d21L, 0x7f436096L, 0x7200464fL, 0x76c15bf8L,
   0x68860bfdL, 0x6c47164aL, 0x61043093L, 0x65c52d24L,
   0x119b4be9L, 0x155a565eL, 0x18197087L, 0x1cd86d30L,
   0x029f3d35L, 0x065e2082L, 0x0b1d065bL, 0x0fdc1becL,
   0x3793a651L, 0x3352bbe6L, 0x3e119d3fL, 0x3ad08088L,
   0x2497d08dL, 0x2056cd3aL, 0x2d15ebe3L, 0x29d4f654L,
   0xc5a92679L, 0xc1683bceL, 0xcc2b1d17L, 0xc8ea00a0L,
   0xd6ad50a5L, 0xd26c4d12L, 0xdf2f6bcbL, 0xdbee767cL,
   0xe3a1cbc1L, 0xe760d676L, 0xea23f0afL, 0xeee2ed18L,
   0xf0a5bd1dL, 0xf464a0aaL, 0xf9278673L, 0xfde69bc4L,
   0x89b8fd09L, 0x8d79e0beL, 0x803ac667L, 0x84fbdbd0L,
   0x9abc8bd5L, 0x9e7d9662L, 0x933eb0bbL, 0x97ffad0cL,
   0xafb010b1L, 0xab710d06L, 0xa6322bdfL, 0xa2f33668L,
   0xbcb4666dL, 0xb8757bdaL, 0xb5365d03L, 0xb1f740b4L
};


/*---------------------------------------------*/
void initialiseCRC ( void )
{
   globalCrc = 0xffffffffL;
}


/*---------------------------------------------*/
UInt32 getFinalCRC ( void )
{
   return ~globalCrc;
}


/*---------------------------------------------*/
UInt32 getGlobalCRC ( void )
{
   return globalCrc;
}


/*---------------------------------------------*/
void setGlobalCRC ( UInt32 newCrc )
{
   globalCrc = newCrc;
}


/*---------------------------------------------*/
#define UPDATE_CRC(crcVar,cha)              \
{                                           \
   crcVar = (crcVar << 8) ^                 \
            crc32Table[(crcVar >> 24) ^     \
                       ((UChar)cha)];       \
}


/*---------------------------------------------------*/
/*--- Bit stream I/O                              ---*/
/*---------------------------------------------------*/

/*--
   Although it seems a bit silly, we restrict
   ourselves to one bitstream at a time, so as to
   avoid malloc-ing the structure.  This avoids any
   possible fragmentation-style interactions with
   the repeated malloc/free cycles of large areas
   which happen during decompression.

   bsInUse is a consistency-check feature.
--*/

typedef 
   struct { 
      FILE*  handle;
      Int32  buffer;
      Int32  buffLive;
      Char   mode;
   }
   BitStream;

BitStream  aBitStreamBuffer;
Bool       bsInUse;


/*---------------------------------------------*/
BitStream* bsOpenReadStream ( FILE* stream )
{
   BitStream *bs;

   if (bsInUse) panic ( "bsOpenReadStream" );
   bsInUse = True;
   bs = &aBitStreamBuffer;

   bs->handle = stream;
   bs->buffer = 0;
   bs->buffLive = 0;
   bs->mode = 'r';
   
   return bs;
}


/*---------------------------------------------*/
BitStream* bsOpenWriteStream ( FILE* stream )
{
   BitStream *bs;

   if (bsInUse) panic ( "bsOpenWriteStream" );
   bsInUse = True;
   bs = &aBitStreamBuffer;

   bs->handle = stream;
   bs->buffer = 0;
   bs->buffLive = 0;
   bs->mode = 'w';
   
   return bs;
}


/*---------------------------------------------*/
INLINE void bsPutBit ( BitStream* bs, Int32 bit )
{
   if (bs->buffLive == 8) {
      IntNative retVal = putc ( (UChar) bs->buffer, bs->handle );
      ERROR_IF_EOF ( retVal );
      bytesOut++;
      bs->buffLive = 1;
      bs->buffer = bit & 0x1;
   } else {
      bs->buffer = ( (bs->buffer << 1) | (bit & 0x1) );
      bs->buffLive++;
   };
}


/*---------------------------------------------*/
/*-- 
  We abort if an EOF appears, regardless of whether 
  an I/O error has happened, or we've really hit 
  the end of the file.  The pseudo-justification for
  this is that the caller should interpret the bit
  stream and decide for itself when the stream has
  ended.
--*/
           
INLINE Int32 bsGetBit ( BitStream* bs )
{
   if (bs->buffLive > 0) {
      bs->buffLive --;
      return ( ((bs->buffer) >> (bs->buffLive)) & 0x1 );
   } else {
      IntNative retVal = getc ( bs->handle );
      if ( retVal == EOF ) compressedStreamEOF();
      bs->buffLive = 7;
      bs->buffer = retVal;
      if (bs->buffer == EOF) bitStreamEOF();
      return ( ((bs->buffer) >> 7) & 0x1 );
   }
}


/*---------------------------------------------*/
UChar bsGetUChar ( BitStream* bs )
{
   Int32  i;
   UInt32 c;

   c = 0;
   for (i = 0; i <= 7; i++)
      c = (c << 1) | bsGetBit ( bs );

   return (UChar)c;
}


/*---------------------------------------------*/
void bsPutUChar ( BitStream* bs, UChar c )
{
   Int32 i;
   for (i = 7; i >= 0; i--)
      bsPutBit ( bs, (((UInt32) c) >> i) & 0x1 );
}


/*---------------------------------------------*/
void bsClose ( BitStream* bs )
{
   IntNative retVal;
   if (!bsInUse) panic ( "bsClose" );
   bsInUse = False;

   if ( bs->mode == 'w' ) {
      while ( bs->buffLive < 8 ) {
         bs->buffLive++;
         bs->buffer <<= 1;
      };
      retVal = putc ( (UChar) (bs->buffer), bs->handle );
      ERROR_IF_EOF ( retVal );
      bytesOut++;
      retVal = fflush ( bs->handle );
      ERROR_IF_EOF ( retVal );
   }
   ERROR_IF_NOT_ZERO( ferror(bs->handle) );
   retVal = fclose ( bs->handle );
   ERROR_IF_EOF ( retVal );
}


/*---------------------------------------------------*/
/*--- Generic frequency-table stuff [data defn]   ---*/
/*---------------------------------------------------*/

#define MAX_SYMBOLS 256

/*-- freq[0] is unused, and is kept at zero.
     freq[MAX_SYMBOLS+1] is also unused and kept at zero.
     This is for historical reasons, and is no longer
     necessary.

     The counts for symbols 1..numSymbols are 
     stored at freq[1] .. freq[numSymbols].

     Presumably one should make sure that 
       ((incVal + noExceed) / 2) < noExceed
     so that scaling always produces sensible results.

     We take incValue == 0 to mean that the counts shouldn't
     be incremented or scaled.

     This data decl has to go before the arithmetic coding stuff.
--*/

typedef 
   struct {
      UInt32  numScalings;
      UInt32  numTraffic;
      UInt32  totFreq;
      UInt32  numSymbols;
      UInt32  incValue;
      UInt32  noExceed;
      Char   *name;
      UInt32  freq[MAX_SYMBOLS + 2];
   }
   Model;


/*---------------------------------------------------*/
/*--- The DCC95 arithmetic coder.                 ---*/
/*---------------------------------------------------*/

/*--
  This is a clean-room (ie, my own) implementation of the
  coder described in ``Arithmetic Coding Revisited'',
  by Alistair Moffat, Radford Neal and Ian Witten,
  originally presented at the 1995 IEEE Data Compression
  Conference, Snowbird, Utah, USA in March 1995.

  The paper has evolved somewhat since then.  This
  implementation pertains to the June 1996 version of
  the paper.  In particular, we have an initial value
  for R of 2^(b-1) rather than 2^(b-1) - 1, and termination
  of coding (overly conservative here) is different.

  I don't use the shift-add multiply/divide machinery;
  I could, but it adds complexity & I'm not convinced about
  the long-term architectural benefit of that approach.
  I could be wrong.
--*/

#define TWO_TO_THE(n)        (1 << (n))
#define MAX_BITS_OUTSTANDING 500000000

#define smallB 26
#define smallF 18

UInt32  bigL;
UInt32  bigR;
UInt32  bigD;
UInt32  bitsOutstanding;


/*---------------------------------------------*/
INLINE UInt32 minUInt32 ( UInt32 a, UInt32 b )
{
   if (a < b) return a; else return b;
}


/*---------------------------------------------*/
INLINE void arithCodeBitPlusFollow ( BitStream *bs, UInt32 bit )
{
    bsPutBit ( bs, bit );
    while ( bitsOutstanding > 0 ) {
        bsPutBit ( bs, 1 - bit );
        bitsOutstanding --;
    }
}


/*---------------------------------------------*/
void arithCodeStartEncoding ( BitStream *bs )
{
   bigL = 0;
   bigR = TWO_TO_THE ( smallB - 1 );
   bitsOutstanding = 0;
}


/*---------------------------------------------*/
void arithCodeDoneEncoding ( BitStream *bs )
{
    Int32 i;

    for (i = smallB; i >= 1; i--)
       arithCodeBitPlusFollow ( bs, (bigL >> (i-1)) & 0x1 );
}


/*---------------------------------------------*/
void arithCodeStartDecoding ( BitStream *bs )
{
   Int32 i;

   bigL = 0;
   bigR = TWO_TO_THE ( smallB - 1 );
   bigD = 0;
   for (i = 1; i <= smallB; i++)
      bigD = (bigD << 1) + bsGetBit ( bs );
}


/*---------------------------------------------*/
void arithCodeDoneDecoding ( BitStream *bs )
{
   /*--- No action necessary. ---*/
}


/*---------------------------------------------*/
INLINE void arithCodeRenormalise_Encode ( BitStream* bs )
{
   while (bigR <= TWO_TO_THE ( smallB-2 ) ) {
      if ( (bigL + bigR) <= TWO_TO_THE ( smallB-1 ) ) {
         arithCodeBitPlusFollow ( bs, 0 );
      } else
      if ( TWO_TO_THE ( smallB-1 ) <= bigL ) {
         arithCodeBitPlusFollow ( bs, 1 );
         bigL = bigL - TWO_TO_THE ( smallB-1 );
      } else {
         bitsOutstanding++;
         bigL = bigL - TWO_TO_THE ( smallB-2 );
      }
      bigL = 2 * bigL;
      bigR = 2 * bigR;
   }
}


/*---------------------------------------------*/
void arithCodeSymbol ( BitStream *bs, Model *m, Int32 symbol )
{
   UInt32 smallL, smallH, smallT, smallR, smallR_x_smallL;
   Int32  i;

   #if DEBUG
      assert ( TWO_TO_THE ( smallB-2 ) < bigR );
      assert ( bigR <= TWO_TO_THE ( smallB-1 ) );
      assert ( 0 <= bigL );
      assert ( bigL < TWO_TO_THE ( smallB ) - TWO_TO_THE ( smallB-2 ) );
      assert ( (bigL + bigR) <= TWO_TO_THE ( smallB ) );
   #endif

   /*--- Set smallL and smallH to the cumfreq values 
         respectively prior to and including symbol.
   ---*/
   smallT = m->totFreq;
   smallL = 0;
   for (i = 1; i < symbol; i++) smallL += m->freq[i];
   smallH = smallL + m->freq[symbol];

   smallR = bigR / smallT;

   smallR_x_smallL = smallR * smallL;
   bigL = bigL + smallR_x_smallL;

   if (smallH < smallT)
      bigR = smallR * (smallH - smallL); else
      bigR = bigR - smallR_x_smallL;

   arithCodeRenormalise_Encode ( bs );
 
   if (bitsOutstanding > MAX_BITS_OUTSTANDING)
      panic ( "arithCodeSymbol: too many bits outstanding" );
}


/*---------------------------------------------*/
Int32 arithDecodeSymbol ( BitStream *bs, Model *m )
{
   UInt32 smallL, smallH, smallT, smallR;
   UInt32 smallR_x_smallL, target, symbol;

   smallT = m->totFreq;

   /*--- Get target value. ---*/
   smallR = bigR / smallT;
   target = minUInt32 ( smallT-1, bigD / smallR );

   symbol = 0;
   smallH = 0;
   while (smallH <= target) {
      symbol++;
      smallH += m->freq[symbol];
   }
   smallL = smallH - m->freq[symbol];

   smallR_x_smallL = smallR * smallL;
   bigD = bigD - smallR_x_smallL;
   
   if (smallH < smallT)
      bigR = smallR * (smallH - smallL); else
      bigR = bigR - smallR_x_smallL;

   while ( bigR <= TWO_TO_THE ( smallB-2 ) ) {
      bigR = 2 * bigR;
      bigD = 2 * bigD + bsGetBit ( bs );
   }

   return symbol;
}


/*---------------------------------------------------*/
/*--- Generic frequency-table stuff [fn defns]    ---*/
/*---------------------------------------------------*/

/*---------------------------------------------*/
void initModel ( Model  *m, 
                 Char   *initName, 
                 Int32  initNumSymbols,
                 Int32  initIncValue,
                 Int32  initNoExceed
               )
{
   Int32 i;

   if (initIncValue == 0) {
      m->totFreq = initNumSymbols;
      for (i = 1; i <= initNumSymbols; i++) 
         m->freq[i] = 1;
   } else {
      m->totFreq = initNumSymbols * initIncValue;
      for (i = 1; i <= initNumSymbols; i++) 
         m->freq[i] = initIncValue;
   };

   m->numSymbols                = initNumSymbols;
   m->incValue                  = initIncValue;
   m->noExceed                  = initNoExceed;
   m->name                      = initName;
   m->freq[0]                   = 0;
   m->freq[initNumSymbols + 1]  = 0;
   m->numScalings               = 0;
}


/*---------------------------------------------*/
void dumpModelStats ( Model *m )
{
    fprintf ( 
       stderr, 
       "model %s:\t scalings %d\n",
       m->name, 
       m->numScalings
    );
}


/*---------------------------------------------*/
INLINE void updateModel ( Model *m, Int32 symbol )
{
   UInt32 i;

   m->totFreq      += m->incValue;
   m->freq[symbol] += m->incValue;
   if (m->totFreq > m->noExceed) {
      m->totFreq = 0;
      m->numScalings++;
      for (i = 1; i <= m->numSymbols; i++) {
         m->freq[i] = (m->freq[i] + 1) >> 1;
         m->totFreq += m->freq[i];
      }
   }
}


/*---------------------------------------------*/
INLINE void putSymbol ( Model *m, Int32 symbol, BitStream *bs )
{
   #if DEBUG
      if (! (symbol >= 1 && symbol <= m->numSymbols) ) 
          fprintf ( stderr, 
                    "BAD, mod = %s, sym = %d, max = %d\n",
                    m->name, symbol, m->numSymbols );
   #endif

   arithCodeSymbol ( bs, m, symbol );
   updateModel ( m, symbol );
}


/*---------------------------------------------*/
INLINE Int32 getSymbol ( Model *m, BitStream *bs )
{
   Int32 symbol;
   
   symbol = arithDecodeSymbol ( bs, m );
   updateModel ( m, symbol );

   #if DEBUG
      assert (symbol >= 1 && symbol <= m->numSymbols);
   #endif

   return symbol;
}


/*---------------------------------------------------*/
/*--- For sending bytes/words thru arith coder    ---*/
/*---------------------------------------------------*/

Model bogusModel;


/*---------------------------------------------*/
void initBogusModel ( void )
{
   initModel ( &bogusModel, "bogus", 256, 0, 256 );
}


/*---------------------------------------------*/
void putUChar ( BitStream *bs, UChar c )
{
   putSymbol ( &bogusModel, 1 + (UInt32)c, bs );
}


/*---------------------------------------------*/
void putInt32 ( BitStream *bs, Int32 i )
{
   putUChar ( bs, (UChar) (((UInt32)i >> 24) & 0xFF) );
   putUChar ( bs, (UChar) (((UInt32)i >> 16) & 0xFF) );
   putUChar ( bs, (UChar) (((UInt32)i >>  8) & 0xFF) );
   putUChar ( bs, (UChar) ( (UInt32)i        & 0xFF) );
}


/*---------------------------------------------*/
void putUInt32 ( BitStream *bs, UInt32 i )
{
   putUChar ( bs, (UChar) ((i >> 24) & 0xFF) );
   putUChar ( bs, (UChar) ((i >> 16) & 0xFF) );
   putUChar ( bs, (UChar) ((i >>  8) & 0xFF) );
   putUChar ( bs, (UChar) ( i        & 0xFF) );
}


/*---------------------------------------------*/
UChar getUChar ( BitStream *bs )
{
   return (UChar) (getSymbol ( &bogusModel, bs ) - 1);
}


/*---------------------------------------------*/
Int32 getInt32 ( BitStream *bs )
{
   UInt32 res = 0;

   res |= (getUChar ( bs ) << 24);
   res |= (getUChar ( bs ) << 16);
   res |= (getUChar ( bs ) <<  8);
   res |= (getUChar ( bs )      );
   return (Int32)res;
}


/*---------------------------------------------*/
UInt32 getUInt32 ( BitStream *bs )
{
   UInt32 res = 0;

   res |= (getUChar ( bs ) << 24);
   res |= (getUChar ( bs ) << 16);
   res |= (getUChar ( bs ) <<  8);
   res |= (getUChar ( bs )      );
   return res;
}


/*---------------------------------------------------*/
/*--- The structured model proper                 ---*/
/*---------------------------------------------------*/

#define BASIS           0
#define MODEL_2_3       1
#define MODEL_4_7       2
#define MODEL_8_15      3
#define MODEL_16_31     4
#define MODEL_32_63     5
#define MODEL_64_127    6
#define MODEL_128_255   7

Model models[8];


/*---------------------------------------------*/
/*-- 
  The parameters in these models and bogusModel 
  -- specifically, the value of 1000 for
  max-total-frequency -- determine the lowest
  acceptable values for smallF and indirectly smallB
  in the arithmetic coder above.
--*/
void initModels ( void )
{
   initModel ( &models[BASIS],         "basis",   11,  12,  1000 );
   initModel ( &models[MODEL_2_3],     "2-3",     2,   4,   1000 );
   initModel ( &models[MODEL_4_7],     "4-7",     4,   3,   1000 );
   initModel ( &models[MODEL_8_15],    "8-15",    8,   3,   1000 );
   initModel ( &models[MODEL_16_31],   "16-31",   16,  3,   1000 );
   initModel ( &models[MODEL_32_63],   "32-63",   32,  3,   1000 );
   initModel ( &models[MODEL_64_127],  "64-127",  64,  2,   1000 );
   initModel ( &models[MODEL_128_255], "128-255", 128, 1,   1000 );
}


/*---------------------------------------------*/
void dumpAllModelStats ( void )
{
   dumpModelStats ( &bogusModel );
   dumpModelStats ( &models[BASIS] );
   dumpModelStats ( &models[MODEL_2_3] );
   dumpModelStats ( &models[MODEL_4_7] );
   dumpModelStats ( &models[MODEL_8_15] );
   dumpModelStats ( &models[MODEL_16_31] );
   dumpModelStats ( &models[MODEL_32_63] );
   dumpModelStats ( &models[MODEL_64_127] );
   dumpModelStats ( &models[MODEL_128_255] );
}



#define VAL_RUNA     1
#define VAL_RUNB     2
#define VAL_ONE      3
#define VAL_2_3      4
#define VAL_4_7      5
#define VAL_8_15     6
#define VAL_16_31    7
#define VAL_32_63    8
#define VAL_64_127   9
#define VAL_128_255  10
#define VAL_EOB      11

#define RUNA    257
#define RUNB    258
#define EOB     259
#define INVALID 260


/*---------------------------------------------*/
Int32 getMTFVal ( BitStream *bs )
{
   Int32 retVal;

   switch ( getSymbol ( &models[BASIS], bs ) ) {
      case VAL_EOB:
         retVal = EOB; break;
      case VAL_RUNA:
         retVal = RUNA; break;
      case VAL_RUNB:
         retVal = RUNB; break;
      case VAL_ONE: 
         retVal = 1; break;
      case VAL_2_3:
         retVal = getSymbol ( &models[MODEL_2_3], bs ) + 2 - 1; break;
      case VAL_4_7:
         retVal = getSymbol ( &models[MODEL_4_7], bs ) + 4 - 1; break;
      case VAL_8_15:
         retVal = getSymbol ( &models[MODEL_8_15], bs ) + 8 - 1; break;
      case VAL_16_31:
         retVal = getSymbol ( &models[MODEL_16_31], bs ) + 16 - 1; break;
      case VAL_32_63:
         retVal = getSymbol ( &models[MODEL_32_63], bs ) + 32 - 1; break;
      case VAL_64_127:
         retVal = getSymbol ( &models[MODEL_64_127], bs ) + 64 - 1; break;
      default:
         retVal = getSymbol ( &models[MODEL_128_255], bs ) + 128 - 1; break;
   }
   return retVal;
}


/*---------------------------------------------*/
void sendMTFVal ( BitStream *bs, Int32 n )
{
   if (n == RUNA) putSymbol ( &models[BASIS], VAL_RUNA, bs ); else
   if (n == RUNB) putSymbol ( &models[BASIS], VAL_RUNB, bs ); else
   if (n == EOB ) putSymbol ( &models[BASIS], VAL_EOB,  bs ); else

   if (n == 1) putSymbol ( &models[BASIS], VAL_ONE, bs ); else

   if (n >= 2 && n <= 3) {
      putSymbol ( &models[BASIS], VAL_2_3, bs );
      putSymbol ( &models[MODEL_2_3], n - 2 + 1, bs );
   } else

   if (n >= 4 && n <= 7) {
      putSymbol ( &models[BASIS], VAL_4_7, bs );
      putSymbol ( &models[MODEL_4_7], n - 4 + 1, bs );
   } else

   if (n >= 8 && n <= 15) {
      putSymbol ( &models[BASIS], VAL_8_15, bs );
      putSymbol ( &models[MODEL_8_15], n - 8 + 1, bs );
   } else

   if (n >= 16 && n <= 31) {
      putSymbol ( &models[BASIS], VAL_16_31, bs );
      putSymbol ( &models[MODEL_16_31], n - 16 + 1, bs );
   } else

   if (n >= 32 && n <= 63) {
      putSymbol ( &models[BASIS], VAL_32_63, bs );
      putSymbol ( &models[MODEL_32_63], n - 32 + 1, bs );
   } else

   if (n >= 64 && n <= 127) {
      putSymbol ( &models[BASIS], VAL_64_127, bs );
      putSymbol ( &models[MODEL_64_127], n - 64 + 1, bs );
   } else

   if (n >= 128 && n <= 255) {
      putSymbol ( &models[BASIS], VAL_128_255, bs );
      putSymbol ( &models[MODEL_128_255], n - 128 + 1, bs );
   } else {

      panic ( "sendMTFVal: bad value!" );
   }
}


/*---------------------------------------------------*/
/*--- Move-to-front encoding/decoding             ---*/
/*---------------------------------------------------*/

/*--
  These are the main data structures for
  the Burrows-Wheeler transform.
--*/

/*-- 
  For good performance, fullGt() allows pointers
  to get partially denormalised.  As a consequence,
  we have to copy some small quantity of data
  from the beginning of a block to the end of it
  so things still work right.  These constants control
  that.
--*/  
#define NUM_FULLGT_UNROLLINGS 4
#define MAX_DENORM_OFFSET (4 * NUM_FULLGT_UNROLLINGS)


/*--
  Pointers to compression and decompression
  structures.  Set by
     allocateCompressStructures   and
     setDecompressStructureSizes

  The structures are always set to be suitable
  for a block of size 100000 * blockSize100k.
--*/
UInt32   *words;    /*-- compress              --*/
Int32    *zptr;     /*-- compress & uncompress --*/
Int32    *ftab;     /*-- compress             --*/

UChar    *block;    /*-- uncompress --*/
UChar    *ll;       /*-- uncompress --*/



/*-- 
  always: lastPP == last+1.  
  See discussion in sortIt(). 
--*/
Int32  last;
Int32  lastPP;  


/*--
  index in ptr[] of original string after sorting.
--*/
Int32  origPtr;


/*-- 
  always: in the range 0 .. 9.  
  The current block size is 100000 * this number.
--*/
Int32  blockSize100k;


/*---------------------------------------------*/
/*--
  Manage memory for compression/decompression.
  When compressing, a single block size applies to
  all files processed, and that's set when the 
  program starts.  But when decompressing, each file
  processed could have been compressed with a
  different block size, so we may have to free
  and reallocate on a per-file basis.  

  A call with argument of zero means 
  `free up everything.'  And a value of zero for
  blockSize100k means no memory is currently allocated.
--*/


/*---------------------------------------------*/
void allocateCompressStructures ( void )
{
   Int32 n = 100000 * blockSize100k;
   words   = malloc ( (n + MAX_DENORM_OFFSET) * sizeof(Int32) );
   zptr    = malloc ( n                       * sizeof(Int32) );
   ftab    = malloc ( 65537                   * sizeof(Int32) );

   if (words == NULL || zptr == NULL || ftab == NULL) {
      Int32 totalDraw = (n + MAX_DENORM_OFFSET) * sizeof(Int32) +
                      n * sizeof(Int32) +
                      65537 * sizeof(Int32);

      compressOutOfMemory ( totalDraw, n );
   }
}


/*---------------------------------------------*/
void setDecompressStructureSizes ( Int32 newSize100k )
{
   assert (0 <= newSize100k   && newSize100k   <= 9);
   assert (0 <= blockSize100k && blockSize100k <= 9);

   if (newSize100k == blockSize100k)
      return;

   blockSize100k = newSize100k;

   if (block != NULL) free ( block );
   if (ll    != NULL) free ( ll    );
   if (zptr  != NULL) free ( zptr  );

   if (newSize100k == 0) {
      block = NULL;
      ll    = NULL;
      zptr  = NULL;
   } else {
      Int32 n = 100000 * newSize100k;
      block   = malloc ( n * sizeof(UChar) );
      ll      = malloc ( n * sizeof(UChar) );
      zptr    = malloc ( n * sizeof(Int32) );

      if (block == NULL || ll == NULL || zptr == NULL) {
         Int32 totalDraw = 6 * n * sizeof(UChar);
         uncompressOutOfMemory ( totalDraw, n );
      }
   }
}


/*---------------------------------------------*/
#define IF_THEN_ELSE(c,t,e) ((c) ? (t) : (e))

#define GETFIRST(a)    ((UChar)(words[a] >> 24))
#define GETREST(a)     (words[a] & 0x00ffffff)
#define SETALL(a,w)    words[a] = (w)
#define GETFIRST16(a)  ((UInt32)(words[a] >> 16))
#define GETREST16(a)   (words[a] & 0x0000ffff)

INLINE UInt32 GETALL ( Int32 a )
{  
   #if DEBUG
      assert (a >= 0 && a < lastPP + 4 * NUM_FULLGT_UNROLLINGS);
      if (a >= lastPP) assert (words[a] == words[a-lastPP]);
   #endif
   return words[a];
}

INLINE void SETREST16 ( Int32 a, UInt32 w )
{
   words[a] = (words[a] & 0xffff0000) | (((UInt32)(w)) & 0x0000ffff);
}

INLINE void SETFIRST16 ( Int32 a, UInt32 w )
{
   words[a] = (words[a] & 0x0000ffff) | (((UInt32)(w)) << 16);
}

INLINE void SETREST ( Int32 a, UInt32 w )
{
   words[a] = (words[a] & 0xff000000) | (((UInt32)(w)) & 0x00ffffff);
}

INLINE void SETFIRST ( Int32 a, UChar c )
{
   words[a] = (words[a] & 0x00ffffff) | (((UInt32)(c)) << 24);
}

INLINE void SETSECOND ( Int32 a, UChar c ) 
{
   words[a] = (words[a] & 0xff00ffff) | (((UInt32)(c)) << 16);
}

INLINE void SETTHIRD ( Int32 a, UChar c )
{
   words[a] = (words[a] & 0xffff00ff) | (((UInt32)(c)) << 8);
}

INLINE void SETFOURTH ( Int32 a, UChar c )
{
   words[a] = (words[a] & 0xffffff00) | (((UInt32)(c)));
}


/*---------------------------------------------*/
INLINE Int32 NORMALISE ( Int32 p )
{
   return
   IF_THEN_ELSE ( ((p) < 0),
                  ((p)+lastPP),
                  IF_THEN_ELSE ( ((p)>=lastPP),
                                 ((p)-lastPP),
                                 (p)
                               )
                );
}


/*---------------------------------------------*/
INLINE Int32 NORMALISEHI ( Int32 p )
{
   return
   IF_THEN_ELSE ( ((p)>=lastPP),
                  ((p)-lastPP),
                  (p)
                );
}                     


/*---------------------------------------------*/
INLINE Int32 NORMALISELO ( Int32 p )
{
   return
   IF_THEN_ELSE ( ((p) < 0),
                  ((p)+lastPP),
                  (p)
                );
}


/*---------------------------------------------*/
/*  The above normalisers are quick but only work when
*   p exceeds the block by less than lastPP, since
*   they renormalise merely by adding or subtracting
*   lastPP.  This one always works, although slowly.
*/
INLINE Int32 STRONG_NORMALISE ( Int32 p )
{
   /* -ve number MOD +ve number always
   *  was one of life's little mysteries ...
   */
   while (p < 0) { p += lastPP; };
   return
      p % lastPP;
}


/*---------------------------------------------*/
void sendZeroes ( BitStream *outStream, Int32 zeroesPending )
{
   UInt32 bitsToSend;
   Int32  numBits;

   if (zeroesPending == 0)
      return;

   bitsToSend = 0;
   numBits = 0;
   while (zeroesPending != 0) {
      numBits++;
      bitsToSend <<= 1;
      zeroesPending--;
      if ((zeroesPending & 0x1) == 1) bitsToSend |= 1;
      zeroesPending >>= 1;
   }
   while (numBits > 0) {
      if ((bitsToSend & 0x1) == 1)
         sendMTFVal ( outStream, RUNA ); else
         sendMTFVal ( outStream, RUNB );
      bitsToSend >>= 1;
      numBits--;
   }
}


/*---------------------------------------------*/
void moveToFrontCodeAndSend ( BitStream *outStream, 
                              Bool      thisIsTheLastBlock 
                            )
{
   UChar  yy[256];
   Int32  i, j;
   UChar  tmp;
   UChar  tmp2;
   Int32  zeroesPending;

   zeroesPending = 0;
   if (thisIsTheLastBlock)
      putInt32 ( outStream, - ( origPtr+1 ) ); else
      putInt32 ( outStream,   ( origPtr+1 ) );

   initModels ();

   for (i = 0; i <= 255; i++)
      yy[i] = (UChar) i;

   for (i = 0; i <= last; i++) {
      UChar ll_i;

      ll_i = GETFIRST ( NORMALISELO ( zptr[i] - 1 ) );

      j = 0;
      tmp = yy[j];
      while ( ll_i != tmp ) {
         j++;
         tmp2 = tmp;
         tmp = yy[j];
         yy[j] = tmp2;
      };
      yy[0] = tmp;

      if (j == 0) {
         zeroesPending++;
      } else {
         sendZeroes ( outStream, zeroesPending );
         zeroesPending = 0;
         sendMTFVal ( outStream, j );
      }

   }
   sendZeroes ( outStream, zeroesPending );
   sendMTFVal ( outStream, EOB );
}


/*---------------------------------------------*/
Bool getAndMoveToFrontDecode ( BitStream *inStream )
{
   UChar  yy[256];
   Int32  i, j, tmpOrigPtr, nextSym, limit;

   limit = 100000 * blockSize100k;

   tmpOrigPtr = getInt32 ( inStream );
   if (tmpOrigPtr < 0) 
      origPtr = ( -tmpOrigPtr ) - 1; else
      origPtr =    tmpOrigPtr   - 1;

   initModels ();

   for (i = 0; i <= 255; i++)
      yy[i] = (UChar) i;
   
   last = -1;

   nextSym = getMTFVal ( inStream );

   LOOPSTART:

   if (nextSym == EOB) 
      return (tmpOrigPtr < 0);

   /*--- acquire run-length bits, most significant first ---*/
   if (nextSym == RUNA || nextSym == RUNB) {
      Int32 n = 0;
      do {
         n <<= 1;
         if (nextSym == RUNA) n |= 1;
         n++;
         nextSym = getMTFVal ( inStream );
      }
         while (nextSym == RUNA || nextSym == RUNB);
      while (n > 0) {
         last++; if (last >= limit) blockOverrun();
         ll[last] = yy[0];
         n--;
      }
      goto LOOPSTART;
   }

   if (nextSym >= 1 && nextSym <= 255) {
      last++; if (last >= limit) blockOverrun();
      ll[last] = yy[nextSym];

      /*--
         This loop is hammered during decompression,
         hence the unrolling.

         for (j = nextSym; j > 0; j--) yy[j] = yy[j-1];
      --*/

      j = nextSym;
      for (; j > 3; j -= 4) {
         yy[j]   = yy[j-1]; 
         yy[j-1] = yy[j-2];
         yy[j-2] = yy[j-3];
         yy[j-3] = yy[j-4];
      }
      for (; j > 0; j--) yy[j] = yy[j-1];

      yy[0] = ll[last];
      nextSym = getMTFVal ( inStream );
      goto LOOPSTART;
   }

   fprintf ( stderr, "bad MTF value %d\n", nextSym );
   panic ( "getAndMoveToFrontDecode\n" );
   /*--- panic never returns ---*/
   return True;
}


/*---------------------------------------------------*/
/*--- Block-sorting machinery                     ---*/
/*---------------------------------------------------*/

/*---------------------------------------------*/
/*  Doesn't work when lastPP < 4.
*/
void stripe ( void )
{
   Int32 i;

   for (i = 0; i < lastPP; i++) {
      UChar c = GETFIRST(i);
      SETSECOND ( NORMALISELO(i-1), c );
      SETTHIRD  ( NORMALISELO(i-2), c );
      SETFOURTH ( NORMALISELO(i-3), c );
   }
}


/*---------------------------------------------*/
/*  Doesn't work when 
*      lastPP < 4 * NUM_FULLGT_UNROLLINGS.
*/
void copyOffsetWords ( void )
{
   Int32 i;

   for (i = 0; i < 4 * NUM_FULLGT_UNROLLINGS; i++)
      words[lastPP+i] = words[i];
}



/*---------------------------------------------*/
/*  Doesn't work when 
*      lastPP < 4 * NUM_FULLGT_UNROLLINGS.
*/
INLINE Bool fullGt ( Int32 i1, Int32 i2 )
{
   Int32 i1orig = i1;

   if (i1 == i2) return False;

   do {
      UInt32 w1;
      UInt32 w2;

      #if DEBUG
         assert (i1 >= 0);
         assert (i2 >= 0);
         assert (i1 != i2);
         assert (i1 < lastPP);
         assert (i2 < lastPP);
      #endif

      w1 = GETALL(i1);
      w2 = GETALL(i2);
      if (w1 != w2) return (w1 > w2);
      i1 += 4;
      i2 += 4;

      #if DEBUG
         assert (i1 < lastPP + 1 * NUM_FULLGT_UNROLLINGS);
         assert (i2 < lastPP + 1 * NUM_FULLGT_UNROLLINGS);
      #endif

      w1 = GETALL(i1);
      w2 = GETALL(i2);
      if (w1 != w2) return (w1 > w2);
      i1 += 4;
      i2 += 4;

      #if DEBUG
         assert (i1 < lastPP + 2 * NUM_FULLGT_UNROLLINGS);
         assert (i2 < lastPP + 2 * NUM_FULLGT_UNROLLINGS);
      #endif

      w1 = GETALL(i1);
      w2 = GETALL(i2);
      if (w1 != w2) return (w1 > w2);
      i1 += 4;
      i2 += 4;

      #if DEBUG
         assert (i1 < lastPP + 3 * NUM_FULLGT_UNROLLINGS);
         assert (i2 < lastPP + 3 * NUM_FULLGT_UNROLLINGS);
      #endif

      w1 = GETALL(i1);
      w2 = GETALL(i2);
      if (w1 != w2) return (w1 > w2);
      i1 += 4;
      i2 += 4;

      #if DEBUG
         assert (i1 < lastPP + 4 * NUM_FULLGT_UNROLLINGS);
         assert (i2 < lastPP + 4 * NUM_FULLGT_UNROLLINGS);
      #endif

      i1 = NORMALISEHI(i1);
      i2 = NORMALISEHI(i2);

      #if DEBUG
         assert (i1 >= 0);
         assert (i2 >= 0);
         assert (i1 != i2);
         assert (i1 < lastPP);
         assert (i2 < lastPP);
      #endif

   }
      while (i1 != i1orig);
   return False;
}


/*---------------------------------------------*/
/*  Requires striping, and therefore doesn't
    work when lastPP < 4.  This qsort is 
    derived from Weiss' book "Data Structures and
    Algorithm Analysis in C", Section 7.7.
*/

#define ISORT_BELOW 10

#if DEBUG
   Int32 rangeErr ( Int32 x, Int32 wuL, Int32 wuR )
   {
      fprintf ( stderr, "\nRANGE ERROR: %d (%d, %d)\n",
                x, wuL, wuR);
      exit(2);
   }
   #define RC(x) \
      ( ((x) >= wuL && (x) <= wuR) ? (x) : rangeErr((x),wuL, wuR) )
#else
   #define RC(x) (x)
#endif

#define SWAP(za,zb)                                           \
   { Int32 zl = (za); Int32 zr = (zb);                        \
     Int32 zt = zptr[RC(zl)]; zptr[RC(zl)] = zptr[RC(zr)];    \
     zptr[RC(zr)] = zt;                                       \
   }


void qsortFull ( Int32 left, Int32 right )
{
   Int32 pivot, v;
   Int32 i, j;
   Int32 wuC;

   Int32 stackL[40];
   Int32 stackR[40];
   Int32 sp = 0;

   Int32 wuL = left;
   Int32 wuR = right;

   while (True) {

      /*-- 
         At the beginning of this loop, wuL and wuR hold the
         bounds of the next work-unit.
      --*/
      if (wuR - wuL > ISORT_BELOW) {

         /*--
            a large Work Unit; partition-exchange 
         --*/
         wuC = (wuL + wuR) >> 1;
         if (fullGt ( zptr[RC(wuL)], zptr[RC(wuC)] )) SWAP ( wuL, wuC );
         if (fullGt ( zptr[RC(wuL)], zptr[RC(wuR)] )) SWAP ( wuL, wuR );
         if (fullGt ( zptr[RC(wuC)], zptr[RC(wuR)] )) SWAP ( wuC, wuR );

         SWAP ( wuC, wuR-1 );
         pivot = zptr[RC(wuR-1)];

         i = wuL;
         j = wuR - 1;
         for (;;) {
            do i++; while (fullGt ( pivot, zptr[RC(i)] ));
            do j--; while (fullGt ( zptr[RC(j)], pivot ));
            if (i < j) SWAP ( i, j ) else break;
         }
         SWAP ( i, wuR-1 );

         if ((i - wuL) > (wuR - i)) {
            stackL[sp] = wuL; stackR[sp] = i-1; sp++; wuL = i+1;
         } else {
            stackL[sp] = i+1; stackR[sp] = wuR; sp++; wuR = i-1;
         }
         #if DEBUG
            assert (sp <= 14);
         #endif

      } else {

         /*--
            a small Work-Unit; insertion-sort it
         --*/                
         for (i = wuL + 1; i <= wuR; i++) {
            v = zptr[RC(i)];
            j = i;
            while ( fullGt ( zptr[RC(j-1)], v )) {
               zptr[RC(j)] = zptr[RC(j-1)];
               j = j - 1;
               if (j <= wuL) goto zero;
            }
            zero:
            zptr[RC(j)] = v;
         }
         if (sp == 0) return;
         sp--; wuL = stackL[sp]; wuR = stackR[sp];
         #if DEBUG
            assert (sp >= 0);
         #endif

      } /* if this is a small work-unit */
   }
}

#undef RC
#undef SWAP
#undef ISORT_BELOW


/*---------------------------------------------*/
/*  Use of NORMALISEHI is safe here, for any
*   lastPP >= 1 (which is guaranteed), since
*   the max denormalisation is 1.
*/
INLINE Bool trivialGt ( Int32 i1, Int32 i2 )
{
   Int32 k;

   for (k = 0; k <= last; k++) {
      UChar c1 = GETFIRST(i1);
      UChar c2 = GETFIRST(i2);
      if (c1 == c2) {
         i1++; i1 = NORMALISEHI(i1);
         i2++; i2 = NORMALISEHI(i2);
      } else
      if (c1 > c2) return True; else return False;
   };
   return False;
}


/*---------------------------------------------*/
/*  Always works.
*/
void shellTrivial ( void )
{
   Int32 i, j, h, bigN;
   Int32 v;

   Int32 ptrLo = 0;
   Int32 ptrHi = last;
   bigN = ptrHi - ptrLo + 1;
   h = 1;
   do { h = 3 * h + 1; } while ( ! (h > bigN) );
   do {
      h = h / 3;
      for (i = ptrLo + h; i <= ptrHi; i++) {
         v = zptr[i];
         j = i;
         while ( trivialGt ( zptr[j-h], v ) ) {
            zptr[j] = zptr[j-h];
            j = j - h;
            if (j <= (ptrLo + h - 1)) goto zero;
         }
         zero:
         zptr[j] = v;
      }
   } while (h != 1);
}


/*---------------------------------------------*/
/*  We have to be pretty careful for small block
*   sizes; the usual mechanism won't work properly,
*   all, ultimately, because the pointer normalisation
*   machinery doesn't work right whenever the amount
*   of denormalisation exceeds lastPP.  And the
*   greatest possible amount of denormalisation here
*   is generated in fullGt, as 
*      4 * NUM_FULLGT_UNROLLINGS.
*   To make blocks smaller than 4 * NUM_... sort
*   correctly, it seems easiest simply
*   to forget about striping, &c, and just do a simple
*   shellsort on the un-striped block.  The performance
*   loss has to be inconsequential, since 4 * NUM_... is
*   tiny (16 at present).
*/
void sortIt ( void )
{
   /*-- 
      lastPP is `last++', ie, is always == last+1.  
      The two (lastPP and last+1) should be interchangeable.
      lastPP is more convenient for fast renormalisation,
      that's all.  

      In the various block-sized structures, live data runs
      from 0 to last inclusive, so lastPP is the number
      of live data items.
   --*/
   lastPP = last + 1;

   if (lastPP <= 1024) {

      /*--
         shellTrivial *must* be used for
         lastPP <= 4 * NUM_FULLGT_UNROLLINGS.
         The 1024 limit is much higher; the purpose is to avoid
         lumbering sorting of small blocks with the 
         fixed overhead of the full counting-sort mechanism.
      --*/

      Int32 i;

      if (veryVerbose) fprintf ( stderr, "trivialSort ...\n" );
      for (i = 0; i <= last; i++) zptr[i] = i;
      shellTrivial();
      if (veryVerbose) fprintf ( stderr, "trivialSort done.\n" );

   } else {
      Int32 i;
      Int32 grade;
      Int32 notDone;

      stripe ();

      if (veryVerbose) fprintf ( stderr, "bucket sorting ...\n" );

      for (i = 0; i <= 65536; i++)
         ftab[i] = 0;
      for (i = 0; i <= last; i++)
         ftab[GETFIRST16(i)]++;
      for (i = 1; i <= 65536; i++)
         ftab[i] += ftab[i-1];

      for (i = 0; i <= last; i++) {
         UInt32 j = GETFIRST16(i);
         ftab[j]--;
         zptr[ftab[j]] = i;
      }

      copyOffsetWords();

      notDone = lastPP;
      for (grade = 1; grade <= 5; grade++) {
         Int32 candNo;
         Int32 loBound;
         Int32 hiBound;

         switch (grade) {
            case 1:  loBound = 2;     hiBound = 15;     break;
            case 2:  loBound = 16;    hiBound = 255;    break;
            case 3:  loBound = 256;   hiBound = 4095;   break;
            case 4:  loBound = 4096;  hiBound = 65535;  break;
            case 5:  loBound = 65536; hiBound = 900000; break;
            default: panic ( "gradedSort" );            break;
         }
         if (loBound > lastPP) continue;

         candNo = 0;
         for (i = 0; i <= 65535; i++) {

            Int32 freqHere = ftab[i+1] - ftab[i];

            if (freqHere >= loBound && freqHere <= hiBound) {
               Int32 j, k;
               Int32 lower = ftab[i];
               Int32 upper = ftab[i+1] - 1;

               candNo++;
               notDone -= freqHere;

               if (veryVerbose) {
                  fprintf ( stderr,
                            "   %d -> %d:  cand %5d,   freq = %6d,   notdone = %6d",
                            loBound, hiBound, candNo, freqHere, notDone );
                  fflush ( stderr );
               }

               qsortFull ( lower, upper );

               if (freqHere < 65535) {
                  for (j = lower, k = 0; j <= upper; j++, k++) {
                     Int32 a2update = zptr[j];
                     SETREST16(a2update, k);
                       if (a2update < (4 * NUM_FULLGT_UNROLLINGS))
                          SETREST16(a2update + lastPP, k);
                  }
               }
               if (veryVerbose)
                  fprintf ( stderr, "\n" );

            }  
         }
      }
   }
}


/*---------------------------------------------------*/
/*--- The Reversible Transformation (tm)          ---*/
/*---------------------------------------------------*/

/*---------------------------------------------*/
/*--- Use: block [0 .. last]
      Def: origPtr.  ll [0 .. last] is synthesised later.
---*/
void doReversibleTransformation ( void )
{
   Int32 i;

   if (veryVerbose) fprintf ( stderr, "\n" );

   sortIt ();

   origPtr = -1;
   for (i = 0; i <= last; i++)
       if (zptr[i] == 0) 
          { origPtr = i; break; };
 
   if (origPtr == -1) panic ( "doReversibleTransformation" );

}


/*---------------------------------------------*/
/*--- Use: ll[0 .. last] and origPtr
      Def: block[0 .. last]
---*/
void undoReversibleTransformation ( void )
{
   Int32  cc[256];
   Int32  i, j, ch, sum;

   for (i = 0; i <= 255; i++) cc[i] = 0;
   
   for (i = 0; i <= last; i++) {
      UChar ll_i = ll[i];
      zptr[i] = cc[ll_i];
      cc[ll_i] ++;
   };

   sum = 0;
   for (ch = 0; ch <= 255; ch++) {
      sum = sum + cc[ch];
      cc[ch] = sum - cc[ch];
   };

   i = origPtr;
   for (j = last; j >= 0; j--) {
      UChar ll_i = ll[i];
      block[j] = ll_i;
      i = zptr[i] + cc[ll_i];
   };
}


/*---------------------------------------------------*/
/*--- The block loader and RLEr                   ---*/
/*---------------------------------------------------*/

#define SPOT_BASIS_STEP 8000

/*---------------------------------------------*/
void spotBlock ( Bool weAreCompressing )
{
   Int32 pos, delta, newdelta;

   pos   = SPOT_BASIS_STEP;
   delta = 1;

   while (pos < last) {

      Int32 n;

      if (weAreCompressing)
         n = (Int32)GETFIRST(pos) + 1; else
         n = (Int32)block[pos]    - 1;

      if (n == 256) n = 0; else if (n == -1)  n = 255;

      if (! (n >= 0 && n <= 255) ) panic ( "spotBlock" );

      if (weAreCompressing)
         SETFIRST(pos, (UChar)n); else
         block[pos] = (UChar)n;

      switch (delta) {
         case 3:  newdelta = 1; break;
         case 1:  newdelta = 4; break;
         case 4:  newdelta = 5; break;
         case 5:  newdelta = 9; break;
         case 9:  newdelta = 2; break;
         case 2:  newdelta = 6; break;
         case 6:  newdelta = 7; break;
         case 8:  newdelta = 8; break;
         case 7:  newdelta = 3; break;
         default: newdelta = 1; break;
      }
      delta = newdelta;
      
      pos = pos + SPOT_BASIS_STEP + 17 * (newdelta - 5);
   }
} 



/*---------------------------------------------*/
/*  Top 16:   run length, 1 to 255.
*   Lower 16: the char, or MY_EOF for EOF.
*/

#define MY_EOF 257

INLINE Int32 getRLEpair ( FILE* src )
{
   Int32     runLength;
   IntNative ch, chLatest;

   ch = getc ( src );

   /*--- Because I have no idea what kind of a value EOF is. ---*/
   if (ch == EOF) {
      ERROR_IF_NOT_ZERO ( errno );
      return (1 << 16) | MY_EOF;
   }

   runLength = 0;
   do {
      chLatest = getc ( src );
      runLength++;
      bytesIn++;
   } 
      while (ch == chLatest && runLength < 255);
   
   if ( chLatest != EOF ) {
      if ( ungetc ( chLatest, src ) == EOF )
         panic ( "getRLEpair: ungetc failed" );
   } else {
      ERROR_IF_NOT_ZERO ( errno );
   }

   /*--- Conditional is just a speedup hack. ---*/
   if (runLength == 1) {
      UPDATE_CRC ( globalCrc, (UChar)ch );
      return (1 << 16) | ch;
   } else {
      Int32 i;
      for (i = 1; i <= runLength; i++)
         UPDATE_CRC ( globalCrc, (UChar)ch );
      return (runLength << 16) | ch;
   }
}


/*---------------------------------------------*/
Bool loadAndRLEsource ( FILE* src )
{
   Int32 ch, allowableBlockSize;

   last = -1;
   ch   = 0;
   
   /*--- 20 is just a paranoia constant ---*/
   allowableBlockSize = 100000 * blockSize100k - 20;

   while (last < allowableBlockSize && ch != MY_EOF) {
      Int32 rlePair, runLen;
      rlePair = getRLEpair ( src );
      ch      = rlePair & 0xFFFF;
      runLen  = (UInt32)rlePair >> 16;

      #if DEBUG
         assert (runLen >= 1 && runLen <= 255);
      #endif

      if (ch == MY_EOF)
         { last++; SETFIRST(last, ((UChar)42)); }
         else
         switch (runLen) {
            case 1:
               last++; SETFIRST(last, ((UChar)ch)); break;
            case 2:
               last++; SETFIRST(last, ((UChar)ch));
               last++; SETFIRST(last, ((UChar)ch)); break;
            case 3:
               last++; SETFIRST(last, ((UChar)ch));
               last++; SETFIRST(last, ((UChar)ch));
               last++; SETFIRST(last, ((UChar)ch)); break;
            default:
               last++; SETFIRST(last, ((UChar)ch));
               last++; SETFIRST(last, ((UChar)ch));
               last++; SETFIRST(last, ((UChar)ch));
               last++; SETFIRST(last, ((UChar)ch));
               last++; SETFIRST(last, ((UChar)(runLen-4))); break;
         }
   }
   return (ch == MY_EOF);
}


/*---------------------------------------------*/
/*--
  This new version is derived from some code
  sent to me Christian von Roques.
--*/
void unRLEandDump ( FILE* dst, Bool thisIsTheLastBlock )
{
   IntNative retVal;
   Int32     lastCharToSpew, i, count, chPrev, ch;
   UInt32    localCrc;

   if (thisIsTheLastBlock)
      lastCharToSpew = last - 1; else
      lastCharToSpew = last;

   count    = 0;
   i        = 0;
   ch       = 256;   /*-- not a char and not EOF --*/
   localCrc = getGlobalCRC();

   while ( i <= lastCharToSpew ) {
      chPrev = ch;
      ch = block[i];
      i++;

      retVal = putc ( ch, dst );
      ERROR_IF_EOF ( retVal );
      UPDATE_CRC ( localCrc, (UChar)ch );

      if (ch != chPrev) {
         count = 1;
      } else { 
         count++;
         if (count >= 4) {
            Int32 j;
            for (j = 0;  j < (Int32)block[i];  j++) {
               retVal = putc (ch, dst);
               ERROR_IF_EOF ( retVal );
               UPDATE_CRC ( localCrc, (UChar)ch );
            }
            i++;
            count = 0;
         }
      }
   }

   setGlobalCRC ( localCrc );

   if (thisIsTheLastBlock && block[last] != 42) unblockError ();
}


/*---------------------------------------------------*/
/*--- Processing of complete files and streams    ---*/
/*---------------------------------------------------*/

/*---------------------------------------------*/
void compressStream ( FILE *stream, FILE *zStream )
{
   IntNative  retVal;
   Bool       thisIsTheLastBlock;
   BitStream  *zbs;
   UInt32     crcToSend;
   Int32      blockNo = 1;

   bytesIn  = 0;
   bytesOut = 0;

   SET_BINARY_MODE(stream);
   SET_BINARY_MODE(zStream);

   zbs = bsOpenWriteStream ( zStream );

   /*--- Write `magic' bytes B and Z,
         then 0 indicating file-format revision zero,
         followed by a digit indicating blockSize100k.
   ---*/
   bsPutUChar ( zbs, 'B' );
   bsPutUChar ( zbs, 'Z' );
   bsPutUChar ( zbs, '0' );
   bsPutUChar ( zbs, '0' + blockSize100k );

   initialiseCRC ();
   initBogusModel ();
   arithCodeStartEncoding ( zbs );

   do {
      if (veryVerbose)
         fprintf ( stderr, "\nBEGIN block %d\n", blockNo );
      blockNo++;
      thisIsTheLastBlock = loadAndRLEsource ( stream );
      spotBlock ( True );
      doReversibleTransformation ();
      moveToFrontCodeAndSend ( zbs, thisIsTheLastBlock );
   }
      while ( ! thisIsTheLastBlock );

   crcToSend = getFinalCRC ();
   putUInt32 ( zbs, crcToSend );
   if (veryVerbose)
      fprintf ( stderr, "\nCRC = 0x%x\n", crcToSend );
  
   arithCodeDoneEncoding ( zbs );
   bsClose ( zbs );
   ERROR_IF_NOT_ZERO ( ferror(stream) );
   retVal = fclose ( stream );
   ERROR_IF_EOF ( retVal );

   if (veryVerbose) {
       fprintf ( stderr, "\n" );
       dumpAllModelStats ();
       fprintf ( stderr, "\n" );
   }
   
   if (bytesIn == 0) bytesIn = 1;
   if (bytesOut == 0) bytesOut = 1;

   if (verbose)
      fprintf ( stderr, "%6.3f:1, %6.3f bits/byte, "
                        "%5.2f%% saved, %d in, %d out.\n",
                (float)bytesIn / (float)bytesOut,
                (8.0 * (float)bytesOut) / (float)bytesIn,
                100.0 * (1.0 - (float)bytesOut / (float)bytesIn),
                bytesIn, 
                bytesOut
              );

   if (veryVerbose)
      fprintf ( stderr, "\n" );
}


/*---------------------------------------------*/
Bool uncompressStream ( FILE *zStream, FILE *stream )
{
   Bool       thisIsTheLastBlock;
   BitStream  *zbs;
   Int32      magic1, magic2, magic3, magic4;
   UInt32     crcStored, crcComputed;
   Int32      currBlockNo;
   IntNative  retVal;

   SET_BINARY_MODE(stream);
   SET_BINARY_MODE(zStream);

   zbs = ( bsOpenReadStream ( zStream ) );

   /*-- 
      A bad magic number is `recoverable from';
      return with False so the caller skips the file.
   --*/
   magic1 = (Int32)bsGetUChar ( zbs );
   magic2 = (Int32)bsGetUChar ( zbs );
   magic3 = (Int32)bsGetUChar ( zbs );
   magic4 = (Int32)bsGetUChar ( zbs );
   if (magic1 != 'B' ||
       magic2 != 'Z' ||
       magic3 != '0' ||
       magic4 < '1'  ||
       magic4 > '9') {
     bsClose ( zbs );
     retVal = fclose ( stream );
     ERROR_IF_EOF ( retVal );
     return False;
   }

   setDecompressStructureSizes ( magic4 - '0' );
   initialiseCRC ();
   initBogusModel ();
   arithCodeStartDecoding ( zbs );

   if (veryVerbose) fprintf ( stderr, "\n  " );
   currBlockNo = 0;
   do {
      currBlockNo++;
      if (veryVerbose)
         fprintf ( stderr, "[%d: ac+mtf ", currBlockNo );
      thisIsTheLastBlock = getAndMoveToFrontDecode ( zbs );
      if (veryVerbose) fprintf ( stderr, "rt " );
      undoReversibleTransformation ();
      spotBlock ( False );
      if (veryVerbose) fprintf ( stderr, "rld" );
      unRLEandDump ( stream, thisIsTheLastBlock );
      if (veryVerbose) fprintf ( stderr, "] " );
   }
      while ( ! thisIsTheLastBlock );    

   if (veryVerbose) fprintf ( stderr, "\n  " );

   /*-- A bad CRC is considered a fatal error. --*/
   crcStored   = getUInt32 ( zbs );
   crcComputed = getFinalCRC ();
   if (veryVerbose)
      fprintf ( stderr,
                "CRCs: stored = 0x%x, computed = 0x%x\n  ",
                crcStored, crcComputed );
   if (crcStored != crcComputed)
      crcError ( crcStored, crcComputed );

   arithCodeDoneDecoding ( zbs );
   bsClose ( zbs );
   ERROR_IF_NOT_ZERO ( ferror(stream) );
   retVal = fclose ( stream );
   ERROR_IF_EOF ( retVal );
   return True;
}



/*---------------------------------------------------*/
/*--- Error [non-] handling grunge                ---*/
/*---------------------------------------------------*/

/*---------------------------------------------*/
void showFileNames ( void )
{
   fprintf ( 
      stderr,
      "\tInput file = %s, output file = %s\n",
      (opMode == OM_STDIN_TO_STDOUT) ? "(stdin)" : inName,
      (opMode != OM_FILES_TO_FILES) ? "(stdout)" : outName
   );
}


/*---------------------------------------------*/
void cleanUpAndFail ( void )
{ 
   IntNative retVal;

   if ( opMode == OM_FILES_TO_FILES ) {
      fprintf ( stderr, "%s: Deleting output file %s, if it exists.\n",
                progName, outName );
      if (outputHandleJustInCase != NULL)
         fclose ( outputHandleJustInCase );
      retVal = remove ( outName );
      if (retVal != 0)
         fprintf ( stderr, 
                   "%s: WARNING: deletion of output file (apparently) failed.\n",
                   progName );
   }
   exit ( 1 );
}


/*---------------------------------------------*/
void panic ( Char* s )
{
   fprintf ( stderr, 
             "\n%s: PANIC -- internal consistency error:\n"
             "\t%s\n"
             "\tThis is a BUG.  Please report it to me at:\n"
             "\tsewardj@cs.man.ac.uk.\n",
             progName, s );
   showFileNames();
   cleanUpAndFail();
}


/*---------------------------------------------*/
void crcError ( UInt32 crcStored, UInt32 crcComputed )
{
   fprintf ( stderr,
             "\n%s: Data integrity error when decompressing.\n"
             "\tStored CRC = 0x%x, computed CRC = 0x%x\n"
             "\tThis could be a bug -- please report it to me at:\n"
             "\tsewardj@cs.man.ac.uk.\n",
             progName, crcStored, crcComputed );
   showFileNames();
   cleanUpAndFail();
}


/*---------------------------------------------*/
void compressedStreamEOF ( void )
{
   fprintf ( stderr,
             "\n%s: Compressed file ends unexpectedly;\n\t"
             "perhaps it is corrupted?  *Possible* reason follows.\n",
             progName );
   perror ( progName );
   showFileNames();
   cleanUpAndFail();
}


/*---------------------------------------------*/
void ioError ( )
{
   fprintf ( stderr,
             "\n%s: I/O or other error, bailing out.  Possible reason follows.\n",
             progName );
   perror ( progName );
   showFileNames();
   cleanUpAndFail();
}


/*---------------------------------------------*/
void blockOverrun ()
{
   fprintf ( stderr,
             "\n%s: block overrun during decompression,\n"
             "\twhich probably means the compressed file\n"
             "\tis corrupted.\n",
             progName );
   showFileNames();
   cleanUpAndFail();
}


/*---------------------------------------------*/
void unblockError ()
{
   fprintf ( stderr,
             "\n%s: compressed file didn't unblock correctly,\n"
             "\twhich probably means it is corrupted.\n",
             progName );
   showFileNames();
   cleanUpAndFail();
}


/*---------------------------------------------*/
void bitStreamEOF ()
{
   fprintf ( stderr,
             "\n%s: read past the end of compressed data,\n"
             "\twhich probably means it is corrupted.\n",
             progName );
   showFileNames();
   cleanUpAndFail();
}


/*---------------------------------------------*/
void mySignalCatcher ( IntNative *n )
{
   fprintf ( stderr, 
             "\n%s: Control-C (or similar) caught, quitting.\n",
             progName );
   cleanUpAndFail();
}


/*---------------------------------------------*/
void mySIGSEGVorSIGBUScatcher ( IntNative *n )
{
   if (compressing)
      fprintf ( stderr,
                "\n%s: Caught a SIGSEGV or SIGBUS whilst compressing,\n"
                "\twhich probably indicates a bug in BZIP.  Please\n"
                "\treport it to me at: sewardj@cs.man.ac.uk\n",
                progName );
      else
      fprintf ( stderr,
                "\n%s: Caught a SIGSEGV or SIGBUS whilst decompressing,\n"
                "\twhich probably indicates that the compressed data\n"
                "\tis corrupted.\n",
                progName );

   showFileNames();
   cleanUpAndFail();
}


/*---------------------------------------------*/
void uncompressOutOfMemory ( Int32 draw, Int32 blockSize )
{
   fprintf ( stderr, 
             "\n%s: Can't allocate enough memory for decompression.\n"
             "\tRequested %d bytes for a block size of %d.\n"
             "\tFind a machine with more memory, perhaps?\n",
             progName, draw, blockSize );
   showFileNames();
   cleanUpAndFail();
}


/*---------------------------------------------*/
void compressOutOfMemory ( Int32 draw, Int32 blockSize )
{
   fprintf ( stderr, 
             "\n%s: Can't allocate enough memory for compression.\n"
             "\tRequested %d bytes for a block size of %d.\n"
             "\tReduce the block size, and/or use the -e flag.\n",
             progName, draw, blockSize );
   showFileNames();
   cleanUpAndFail();
}


/*---------------------------------------------------*/
/*--- The main driver machinery                   ---*/
/*---------------------------------------------------*/

/*---------------------------------------------*/
void pad ( Char *s )
{
   Int32 i;
   if ( (Int32)strlen(s) >= longestFileName ) return;
   for (i = 1; i <= longestFileName - (Int32)strlen(s); i++) 
      fprintf ( stderr, " " );
}


/*---------------------------------------------*/
Bool fileExists ( Char* name )
{
   FILE *tmp   = fopen ( name, "rb" );
   Bool exists = (tmp != NULL);
   if (tmp != NULL) fclose ( tmp );
   return exists;
}


/*---------------------------------------------*/
/*--
  if in doubt, return True
--*/
Bool notABogStandardFile ( Char* name )
{  
   IntNative      i;
   struct MY_STAT statBuf;

   i = MY_LSTAT ( name, &statBuf );
   if (i != 0) return True;
   if (MY_S_IFREG(statBuf.st_mode)) return False;
   return True;
}


/*---------------------------------------------*/
void copyDateAndPermissions ( Char *srcName, Char *dstName )
{
   IntNative      retVal;
   struct MY_STAT statBuf;
   struct utimbuf uTimBuf;

   retVal = MY_LSTAT ( srcName, &statBuf );
   ERROR_IF_NOT_ZERO ( retVal );
   uTimBuf.actime = statBuf.st_atime;
   uTimBuf.modtime = statBuf.st_mtime;

   retVal = chmod ( dstName, statBuf.st_mode );
   ERROR_IF_NOT_ZERO ( retVal );
   retVal = utime ( dstName, &uTimBuf );
   ERROR_IF_NOT_ZERO ( retVal );
}


/*---------------------------------------------*/
Bool endsInBz ( Char* name )
{
   Int32 n = strlen ( name );
   if (n <= 3) return False;
   return
      (name[n-3] == '.' && 
       name[n-2] == 'b' && 
       name[n-1] == 'z');
}


/*---------------------------------------------*/
Bool containsDubiousChars ( Char* name )
{
   Bool cdc = False;
   for (; *name != '\0'; name++)
      if (*name == '?' || *name == '*') cdc = True;
   return cdc;
}


/*---------------------------------------------*/
void compress ( Char *name )
{
   FILE *inStr;
   FILE *outStr;

   strcpy ( inName, name );
   strcpy ( outName, name );
   strcat ( outName, ".bz" );

   if ( opMode != OM_STDIN_TO_STDOUT && containsDubiousChars ( inName ) ) {
      fprintf ( stderr, "%s: There are no files matching `%s'.\n",
      progName, inName );
      return;
   }
   if ( opMode != OM_STDIN_TO_STDOUT && !fileExists ( inName ) ) {
      fprintf ( stderr, "%s: Input file %s doesn't exist, skipping.\n",
                progName, inName );
      return;
   }
   if ( opMode != OM_STDIN_TO_STDOUT && endsInBz ( inName )) {
      fprintf ( stderr, "%s: Input file name %s ends in `.bz', skipping.\n",
                progName, inName );
      return;
   }
   if ( opMode != OM_STDIN_TO_STDOUT && notABogStandardFile ( inName )) {
      fprintf ( stderr, "%s: Input file %s is not a normal file, skipping.\n",
                progName, inName );
      return;
   }
   if ( opMode == OM_FILES_TO_FILES && fileExists ( outName ) ) {
      fprintf ( stderr, "%s: Output file %s already exists, skipping.\n",
                progName, outName );
      return;
   }
   
   switch ( opMode ) {

      case OM_STDIN_TO_STDOUT:
         inStr = stdin; 
         outStr = stdout;
         if ( isatty ( fileno ( stdout ) ) ) {
            fprintf ( stderr, 
                      "%s: I won't write compressed data to a terminal.\n",
                      progName );
            fprintf ( stderr, "%s: For help, type: `%s --help'.\n",
                              progName, progName );
            return;
         };
         break;

      case OM_FILE_TO_STDOUT:
         inStr = fopen ( inName, "rb" );
         outStr = stdout;
         if ( isatty ( fileno ( stdout ) ) ) {
            fprintf ( stderr, 
                      "%s: I won't write compressed data to a terminal.\n",
                      progName );
            fprintf ( stderr, "%s: For help, type: `%s --help'.\n",
                              progName, progName );
            return;
         };
         if ( inStr == NULL ) {
            fprintf ( stderr, "%s: Can't open input file %s, skipping.\n",
                      progName, inName );
            return;
         };
         break;

      case OM_FILES_TO_FILES:
         inStr = fopen ( inName, "rb" );
         outStr = fopen ( outName, "wb" );
         if ( outStr == NULL) {
            fprintf ( stderr, "%s: Can't create output file %s, skipping.\n",
                      progName, outName );
            return;
         }
         if ( inStr == NULL ) {
            fprintf ( stderr, "%s: Can't open input file %s, skipping.\n",
                      progName, inName );
            return;
         };
         break;

      default:
         panic ( "compress: bad opMode" );
         break;
   }

   if (verbose) {
      fprintf ( stderr, 
                "  %s: ", 
                opMode == OM_STDIN_TO_STDOUT ? "(stdin)" : inName );
      pad ( opMode == OM_STDIN_TO_STDOUT ? "(stdin)" : inName );
      fflush ( stderr );
   }
   
   /*--- Now the input and output handles are sane.  Do the Biz. ---*/
   errno = 0;
   outputHandleJustInCase = outStr;
   compressStream ( inStr, outStr );
   outputHandleJustInCase = NULL;

   /*--- If there was an I/O error, we won't get here. ---*/
   if ( opMode == OM_FILES_TO_FILES ) {
      copyDateAndPermissions ( inName, outName );
      if ( !keepInputFiles ) {
         IntNative retVal = remove ( inName );
         ERROR_IF_NOT_ZERO ( retVal );
      }
   }
}


/*---------------------------------------------*/
void uncompress ( Char *name )
{
   FILE *inStr;
   FILE *outStr;
   Bool magicNumberOK;

   strcpy ( inName, name );
   strcpy ( outName, name );
   if ( endsInBz ( inName ) )
      outName [ strlen ( outName ) - 3 ] = '\0';

   if ( opMode != OM_STDIN_TO_STDOUT && containsDubiousChars ( inName ) ) {
      fprintf ( stderr, "%s: There are no files matching `%s'.\n",
                progName, inName );
      return;
   }
   if ( opMode != OM_STDIN_TO_STDOUT && !fileExists ( inName ) ) {
      fprintf ( stderr, "%s: Input file %s doesn't exist, skipping.\n",
                progName, inName );
      return;
   }
   if ( opMode != OM_STDIN_TO_STDOUT && !endsInBz ( inName )) {
      fprintf ( stderr,
                "%s: Input file name %s doesn't end in `.bz', skipping.\n",
                progName, inName );
      return;
   }
   if ( opMode != OM_STDIN_TO_STDOUT && notABogStandardFile ( inName )) {
      fprintf ( stderr, "%s: Input file %s is not a normal file, skipping.\n",
                progName, inName );
      return;
   }
   if ( opMode == OM_FILES_TO_FILES && fileExists ( outName ) ) {
      fprintf ( stderr, "%s: Output file %s already exists, skipping.\n",
                progName, outName );
      return;
   }
   
   switch ( opMode ) {

      case OM_STDIN_TO_STDOUT:
         inStr = stdin; 
         outStr = stdout;
         if ( isatty ( fileno ( stdin ) ) ) {
            fprintf ( stderr,
                      "%s: I won't read compressed data from a terminal.\n",
                      progName );
            fprintf ( stderr, "%s: For help, type: `%s --help'.\n",
                              progName, progName );
            return;
         };
         break;

      case OM_FILE_TO_STDOUT:
         inStr = fopen ( inName, "rb" );
         outStr = stdout;
         if ( inStr == NULL ) {
            fprintf ( stderr, "%s: Can't open input file %s, skipping.\n",
                      progName, inName );
            return;
         };
         break;

      case OM_FILES_TO_FILES:
         inStr = fopen ( inName, "rb" );
         outStr = fopen ( outName, "wb" );
         if ( outStr == NULL) {
            fprintf ( stderr, "%s: Can't create output file %s, skipping.\n",
                      progName, outName );
            return;
         }
         if ( inStr == NULL ) {
            fprintf ( stderr, "%s: Can't open input file %s, skipping.\n",
                      progName, inName );
            return;
         };
         break;

      default:
         panic ( "uncompress: bad opMode" );
         break;
   }

   if (verbose) {
      fprintf ( stderr, 
                "  %s: ", 
                opMode == OM_STDIN_TO_STDOUT ? "(stdin)" : inName );
      pad ( opMode == OM_STDIN_TO_STDOUT ? "(stdin)" : inName );
      fflush ( stderr );
   }

   /*--- Now the input and output handles are sane.  Do the Biz. ---*/
   errno = 0;
   outputHandleJustInCase = outStr;
   magicNumberOK = uncompressStream ( inStr, outStr );
   outputHandleJustInCase = NULL;

   /*--- If there was an I/O error, we won't get here. ---*/
   if ( magicNumberOK ) {
      if ( opMode == OM_FILES_TO_FILES ) {
         copyDateAndPermissions ( inName, outName );
         if ( !keepInputFiles ) { 
            IntNative retVal = remove ( inName );
            ERROR_IF_NOT_ZERO ( retVal );
         }
      }
   } else {
      if ( opMode == OM_FILES_TO_FILES ) {
         IntNative retVal = remove ( outName );
         ERROR_IF_NOT_ZERO ( retVal );
      }
   }

   if ( magicNumberOK ) {
      if (verbose)
         fprintf ( stderr, "done\n" );
   } else {
      if (verbose)
         fprintf ( stderr, "not a BZIP file, skipping.\n" ); else
         fprintf ( stderr, 
                   "%s: %s is not a BZIP file, skipping.\n",
                   progName,
                   opMode == OM_STDIN_TO_STDOUT ? "(stdin)" : inName );
   }

}


/*---------------------------------------------*/
void license ( void )
{
   fprintf ( stderr,

    "  \n"
    "  Copyright (C) 1996 by Julian Seward.\n"
    "  \n"
    "  This program is free software; you can redistribute it and/or modify\n"
    "  it under the terms of the GNU General Public License as published by\n"
    "  the Free Software Foundation; either version 2 of the License, or\n"
    "  (at your option) any later version.\n"
    "  \n"
    "  This program is distributed in the hope that it will be useful,\n"
    "  but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "  GNU General Public License for more details.\n"
    "  \n"
    "  You should have received a copy of the GNU General Public License\n"
    "  along with this program; if not, write to the Free Software\n"
    "  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n"
    "  \n"
    "  The GNU General Public License is contained in the file LICENSE.\n"
    "  \n"
   );
}


/*---------------------------------------------*/
void usage ( Char *fullProgName )
{
   fprintf ( 
      stderr, 
      "\nusage: %s [flags and input files in any order]\n"
      "\n"
      "   Flags:  -d          force decompression\n"
      "           -f          force compression\n"
      "           -c          output to standard out\n"
      "           -v, -V      be verbose, or very verbose\n"
      "           -k          keep (don't delete) input files\n"
      "           -L          display software license\n"
      "           -1 .. -9    set block size of 100k .. 900k\n"
      "\n"
      "   If invoked as `bzip', the default action is to compress.\n"
      "              as `bunzip', the default action is to decompress.\n"
      "\n"
      "   If no file names are given, bzip compresses or decompresses\n"
      "   from standard input to standard output.  You can combine\n"
      "   flags, so `-v -e -4' means the same as -ve4 or -4ev, &c.\n"
      "\n"
      "   The default block size is 900k, which soaks up a lot of\n"
      "   memory for compression (7700k) and decompression (4500k).\n"
      "   You may want to select a smaller block size; see the manual\n"
      "   for details.  Smaller sizes give slightly less compression.\n"
      "   -e also saves memory during compression, at some speed cost.\n"
      "\n",

      fullProgName
   );
}


/*---------------------------------------------*/
/*--
  All the garbage from here to main() is purely to 
  implement a linked list of command-line arguments,
  into which main() copies argv[1 .. argc-1].

  The purpose of this ridiculous exercise is to 
  facilitate the expansion of wildcard characters
  * and ? in filenames for halfwitted OSs like
  MSDOS, Windows 95 and NT ... yawn.

  The actual Dirty Work is done by the platform-specific
  macro APPEND_FILESPEC.
--*/

typedef 
   struct zzzz {
      Char        *name;
      struct zzzz *link;
   }
   Cell;


/*---------------------------------------------*/
void *myMalloc ( size_t n )
{
   void* p;

   p = malloc ( n );
   if (p == NULL) {
      fprintf ( 
         stderr,
         "%s: `malloc' failed during processing of command-line args.\n",
         progName
      );
      exit ( 1 );
   }
   return p;
}


/*---------------------------------------------*/
Cell *mkCell ( void )
{
   Cell *c;

   c = (Cell*) myMalloc ( sizeof ( Cell ) );
   c->name = NULL;
   c->link = NULL;
   return c;
}


/*---------------------------------------------*/
Cell *snocString ( Cell *root, Char *name )
{
   if (root == NULL) {
      Cell *tmp = mkCell();
      tmp->name = (Char*) myMalloc ( 5 + strlen(name) );
      strcpy ( tmp->name, name );
      return tmp;
   } else {
      Cell *tmp = root;
      while (tmp->link != NULL) tmp = tmp->link;
      tmp->link = snocString ( tmp->link, name );
      return root;
   }
}



/*---------------------------------------------*/
IntNative main ( IntNative argc, Char *argv[] )
{
   Int32  numFileNames;
   Int32  i, j;
   Char   *tmp;
   Cell   *argList;
   Cell   *aa;

   outputHandleJustInCase  = NULL;
   ftab                    = NULL;
   block                   = NULL;
   ll                      = NULL;
   words                   = NULL;
   zptr                    = NULL;
   bsInUse                 = False;
   errno                   = 0;

   strcpy ( progNameReally, argv[0] );
   progName = &progNameReally[0];
   for (tmp = &progNameReally[0]; *tmp != '\0'; tmp++)
      if (*tmp == PATH_SEP) progName = tmp + 1;

   argList = NULL;
   for (i = 1; i <= argc-1; i++)
      APPEND_FILESPEC(argList, argv[i]);

   strcpy ( inName, "-" );
   strcpy ( outName, "-" );

   
   signal (SIGINT,  mySignalCatcher);
   signal (SIGTERM, mySignalCatcher);
   signal (SIGSEGV, mySIGSEGVorSIGBUScatcher);
   #if BZ_UNIX
   signal (SIGHUP,  mySignalCatcher);
   signal (SIGBUS,  mySIGSEGVorSIGBUScatcher);
   #endif

   if ( ! (argc > 1 && strcmp ( "-Q", argv[1] ) == 0) )
      fprintf ( stderr,
                "BZIP, a block-sorting file compressor.  "
                "Version 0.21, 25-August-96.\n" );

   #if DEBUG
      if ( ! (argc > 1 && strcmp ( "-Q", argv[1] ) == 0) )
         fprintf ( stderr, "BZIP: *** compiled with debugging ON ***\n" );
   #endif

   if (sizeof(Int32) != 4 || sizeof(UInt32) != 4    ||
       sizeof(Char)  != 1 || sizeof(UChar)  != 1) {
      fprintf ( stderr, 
                "BZIP: I require sizeof(Int32) == 4 bytes and\n"
                "\tsizeof(Char) == 1 byte to run properly, sorry!\n"
                "\tProbably you can fix this by defining them correctly,\n"
                "\tand recompiling.\n" );
      exit(1);
   }        

   longestFileName = 7;
   numFileNames    = 0;
   for (aa = argList; aa != NULL; aa = aa->link) 
      if (aa->name[0] != '-') {
         numFileNames++;
         if (longestFileName < (Int32)strlen(aa->name) )
            longestFileName = (Int32)strlen(aa->name);
      }

   keepInputFiles  = False;
   compressing     = True;
   verbose         = False;
   veryVerbose     = False;

   if (numFileNames == 0)
      opMode = OM_STDIN_TO_STDOUT; else
      opMode = OM_FILES_TO_FILES;

   if ( (strcmp ( "bunzip",     progName ) == 0) ||
        (strcmp ( "BUNZIP",     progName ) == 0) ||
        (strcmp ( "bunzip.exe", progName ) == 0) ||
        (strcmp ( "BUNZIP.EXE", progName ) == 0) )
      compressing = False;

   if (compressing) blockSize100k = 9;

   for (aa = argList; aa != NULL; aa = aa->link)
      if (aa->name[0] == '-')
         for (j = 1; aa->name[j] != '\0'; j++) 
            switch (aa->name[j]) {
               case 'Q': break;
               case 'c': opMode         = OM_FILE_TO_STDOUT; break;
               case 'd': compressing    = False; break;
               case 'f': compressing    = True; break;
               case 'v': verbose        = True; break;
               case 'k': keepInputFiles = True; break;
               case '1': blockSize100k  = 1; break;
               case '2': blockSize100k  = 2; break;
               case '3': blockSize100k  = 3; break;
               case '4': blockSize100k  = 4; break;
               case '5': blockSize100k  = 5; break;
               case '6': blockSize100k  = 6; break;
               case '7': blockSize100k  = 7; break;
               case '8': blockSize100k  = 8; break;
               case '9': blockSize100k  = 9; break;
               case 'L': license();          break;
               case 'V': verbose        = True; 
                         veryVerbose    = True; break;
               default:  fprintf ( stderr, "%s: Bad flag `%s'\n", 
                                   progName, aa->name );
                         usage ( progName );
                         exit ( 1 );
                         break;
         }

   if ( opMode == OM_FILE_TO_STDOUT && numFileNames != 1) {
      fprintf ( stderr, "%s: Option -c requires you to supply exactly one filename.\n",
                progName );
      exit ( 1 );
   }

   if ( !compressing ) blockSize100k = 0;

   if (compressing) {
      allocateCompressStructures();
      if (opMode == OM_STDIN_TO_STDOUT) 
         compress ( "-" ); 
         else
         for (aa = argList; aa != NULL; aa = aa->link)
            if (aa->name[0] != '-') compress ( aa->name );
   } else {
      if (opMode == OM_STDIN_TO_STDOUT) 
         uncompress ( "-" ); 
         else
         for (aa = argList; aa != NULL; aa = aa->link)
            if (aa->name[0] != '-') uncompress ( aa->name );
   }

   exit ( 0 );
}


/*-----------------------------------------------------------*/
/*--- end                                          bzip.c ---*/
/*-----------------------------------------------------------*/
