// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Bzip.obj   from: .\basewin.lib
// functions: 88   data: 32
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>

// bzip-0.21 (Julian Seward, 1996) — the original arithmetic-coding bzip the
// retail Bzip.obj is built from. Types/macros mirror the reference so bodies
// transcribe near-verbatim (same names -> matching /Od stack slots).
typedef int            Int32;
typedef unsigned int   UInt32;
typedef short          Int16;
typedef unsigned short UInt16;
typedef char           Char;
typedef unsigned char  UChar;
typedef int            Bool;
typedef int            IntNative;
#define True  1
#define False 0
#define ERROR_IF_EOF(i)       { if ((i) == EOF)  ioError(); }
#define ERROR_IF_NOT_ZERO(i)  { if ((i) != 0)    ioError(); }
#define ERROR_IF_MINUS_ONE(i) { if ((i) == (-1)) ioError(); }

#define MAX_SYMBOLS 256
#define TWO_TO_THE(n) (1 << (n))
#define MAX_BITS_OUTSTANDING 500000000
#define smallB 26
#define smallF 18

struct BitStream {
    FILE  *handle;
    Int32  buffer;
    Int32  buffLive;
    Char   mode;
};

struct Model {
    UInt32  numScalings;
    UInt32  numTraffic;
    UInt32  totFreq;
    UInt32  numSymbols;
    UInt32  incValue;
    UInt32  noExceed;
    Char   *name;
    UInt32  freq[MAX_SYMBOLS + 2];
};

unsigned int globalCrc;
int bsInUse;
BitStream aBitStreamBuffer;
int bytesOut;
UInt32 bigL;
UInt32 bigR;
UInt32 bigD;
UInt32 bitsOutstanding;
Model bogusModel;

void panic(char *s);
void ioError(void);
void compressedStreamEOF(void);
void bitStreamEOF(void);
extern char gText[];
extern void LogStr(char *);

VA(0x004d4050, 0x1a)
void initialiseCRC(void)
{
    globalCrc = 0xffffffff;
}

VA(0x004d4070, 0x17)
unsigned int getFinalCRC(void)
{
    return ~globalCrc;
}

VA(0x004d4090, 0x15)
unsigned int getGlobalCRC(void)
{
    return globalCrc;
}

VA(0x004d40b0, 0x1e)
void setGlobalCRC(unsigned int newCrc)
{
    globalCrc = newCrc;
}

VA(0x004d40d0, 0x64)
BitStream * bsOpenReadStream(FILE *f)
{
    BitStream *bs;
    if (bsInUse) panic((char *)"bsOpenReadStream");
    bsInUse = 1;
    bs = &aBitStreamBuffer;
    bs->handle = f;
    bs->buffer = 0;
    bs->buffLive = 0;
    bs->mode = 'r';
    return bs;
}

VA(0x004d4140, 0x64)
BitStream * bsOpenWriteStream(FILE *f)
{
    BitStream *bs;
    if (bsInUse) panic((char *)"bsOpenWriteStream");
    bsInUse = 1;
    bs = &aBitStreamBuffer;
    bs->handle = f;
    bs->buffer = 0;
    bs->buffLive = 0;
    bs->mode = 'w';
    return bs;
}

VA(0x004d41b0, 0x8c)
void bsPutBit(BitStream *bs, int bit)
{
    int retVal;
    if (bs->buffLive == 8) {
        retVal = putc((unsigned char)bs->buffer, bs->handle);
        if (retVal == EOF) ioError();
        bytesOut++;
        bs->buffLive = 1;
        bs->buffer = bit & 1;
    } else {
        bs->buffer = (bs->buffer << 1) | (bit & 1);
        bs->buffLive++;
    }
}

VA(0x004d4240, 0x95)
int bsGetBit(BitStream *bs)
{
    int retVal;
    if (bs->buffLive > 0) {
        bs->buffLive--;
        return (bs->buffer >> bs->buffLive) & 1;
    } else {
        retVal = getc(bs->handle);
        if (retVal == EOF) compressedStreamEOF();
        bs->buffLive = 7;
        bs->buffer = retVal;
        if (bs->buffer == -1) bitStreamEOF();
        return (bs->buffer >> 7) & 1;
    }
}

VA(0x004d42e0, 0x50)
unsigned char bsGetUChar(BitStream *bs)
{
    Int32  i;
    UInt32 c;

    c = 0;
    for (i = 0; i <= 7; i++)
        c = (c << 1) | bsGetBit(bs);

    return (UChar)c;
}

VA(0x004d4330, 0x4e)
void bsPutUChar(BitStream *bs, unsigned char c)
{
    int i;
    for (i = 7; i >= 0; i--)
        bsPutBit(bs, ((unsigned int)c >> i) & 1);
}

VA(0x004d4380, 0xe8)
void bsClose(BitStream *bs)
{
    IntNative retVal;
    if (!bsInUse) panic((char *)"bsClose");
    bsInUse = False;

    if (bs->mode == 'w') {
        while (bs->buffLive < 8) {
            bs->buffLive++;
            bs->buffer <<= 1;
        }
        retVal = putc((UChar)(bs->buffer), bs->handle);
        ERROR_IF_EOF(retVal);
        bytesOut++;
        retVal = fflush(bs->handle);
        ERROR_IF_EOF(retVal);
    }
    ERROR_IF_NOT_ZERO(ferror(bs->handle));
    retVal = fclose(bs->handle);
    ERROR_IF_EOF(retVal);
}

VA(0x004d4470, 0x35)
unsigned int minUInt32(unsigned int a, unsigned int b)
{
    if (a < b) return a; else return b;
}

VA(0x004d44b0, 0x4e)
void arithCodeBitPlusFollow(BitStream *bs, UInt32 bit)
{
    bsPutBit(bs, bit);
    while (bitsOutstanding > 0) {
        bsPutBit(bs, 1 - bit);
        bitsOutstanding--;
    }
}

VA(0x004d4500, 0x34)
void arithCodeStartEncoding(BitStream *bs)
{
    bigL = 0;
    bigR = TWO_TO_THE(smallB - 1);
    bitsOutstanding = 0;
}

VA(0x004d4540, 0x4c)
void arithCodeDoneEncoding(BitStream *bs)
{
    Int32 i;

    for (i = smallB; i >= 1; i--)
        arithCodeBitPlusFollow(bs, (bigL >> (i - 1)) & 0x1);
}

VA(0x004d4590, 0x68)
void arithCodeStartDecoding(BitStream *bs)
{
    Int32 i;

    bigL = 0;
    bigR = TWO_TO_THE(smallB - 1);
    bigD = 0;
    for (i = 1; i <= smallB; i++)
        bigD = (bigD << 1) + bsGetBit(bs);
}

VA(0x004d4600, 0x16)
void arithCodeDoneDecoding(BitStream *bs)
{
    /*--- No action necessary. ---*/
}

VA(0x004d4620, 0x98)
void arithCodeRenormalise_Encode(BitStream *bs)
{
    while (bigR <= TWO_TO_THE(smallB - 2)) {
        if ((bigL + bigR) <= TWO_TO_THE(smallB - 1)) {
            arithCodeBitPlusFollow(bs, 0);
        } else
        if (TWO_TO_THE(smallB - 1) <= bigL) {
            arithCodeBitPlusFollow(bs, 1);
            bigL = bigL - TWO_TO_THE(smallB - 1);
        } else {
            bitsOutstanding++;
            bigL = bigL - TWO_TO_THE(smallB - 2);
        }
        bigL = 2 * bigL;
        bigR = 2 * bigR;
    }
}

VA(0x004d46c0, 0xd7)
void arithCodeSymbol(BitStream *bs, Model *m, Int32 symbol)
{
    UInt32 smallL, smallH, smallT, smallR, smallR_x_smallL;
    Int32  i;

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

    arithCodeRenormalise_Encode(bs);

    if (bitsOutstanding > MAX_BITS_OUTSTANDING)
        panic((char *)"arithCodeSymbol: too many bits outstanding");
}

VA(0x004d47a0, 0xfe)
Int32 arithDecodeSymbol(BitStream *bs, Model *m)
{
    UInt32 smallL, smallH, smallT, smallR;
    UInt32 smallR_x_smallL, target, symbol;

    smallT = m->totFreq;

    smallR = bigR / smallT;
    target = minUInt32(smallT - 1, bigD / smallR);

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

    while (bigR <= TWO_TO_THE(smallB - 2)) {
        bigR = 2 * bigR;
        bigD = 2 * bigD + bsGetBit(bs);
    }

    return (Int32)symbol;
}

VA(0x004d48a0, 0xe0)
void initModel(Model *m, Char *initName, Int32 initNumSymbols, Int32 initIncValue, Int32 initNoExceed)
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
    }

    m->numSymbols               = initNumSymbols;
    m->incValue                 = initIncValue;
    m->noExceed                 = initNoExceed;
    m->name                     = initName;
    m->freq[0]                  = 0;
    m->freq[initNumSymbols + 1] = 0;
    m->numScalings              = 0;
}

VA(0x004d4980, 0x3f)
void dumpModelStats(Model *m)
{
    sprintf(gText, "model %s:\t scalings %d\n", m->name, m->numScalings);
    LogStr(gText);
}

VA(0x004d49c0, 0xa0)
void updateModel(Model *m, Int32 symbol)
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

VA(0x004d4a60, 0x35)
void putSymbol(Model *m, Int32 symbol, BitStream *bs)
{
    arithCodeSymbol(bs, m, symbol);
    updateModel(m, symbol);
}

VA(0x004d4aa0, 0x35)
Int32 getSymbol(Model *m, BitStream *bs)
{
    Int32 symbol;

    symbol = arithDecodeSymbol(bs, m);
    updateModel(m, symbol);

    return symbol;
}

VA(0x004d4ae0, 0x2b)
void initBogusModel(void)
{
    initModel(&bogusModel, (char *)"bogus", 256, 0, 256);
}

VA(0x004d4b10, 0x2f)
void putUChar(BitStream *bs, UChar c)
{
    putSymbol(&bogusModel, 1 + (UInt32)c, bs);
}

VA(0x004d4b40, 0x6a)
void putInt32(BitStream *bs, Int32 i)
{
    putUChar(bs, (UChar)(((UInt32)i >> 24) & 0xFF));
    putUChar(bs, (UChar)(((UInt32)i >> 16) & 0xFF));
    putUChar(bs, (UChar)(((UInt32)i >>  8) & 0xFF));
    putUChar(bs, (UChar)( (UInt32)i        & 0xFF));
}

VA(0x004d4bb0, 0x6a)
void putUInt32(BitStream *bs, UInt32 i)
{
    putUChar(bs, (UChar)((i >> 24) & 0xFF));
    putUChar(bs, (UChar)((i >> 16) & 0xFF));
    putUChar(bs, (UChar)((i >>  8) & 0xFF));
    putUChar(bs, (UChar)( i        & 0xFF));
}

VA(0x004d4c20, 0x28)
UChar getUChar(BitStream *bs)
{
    return (UChar)(getSymbol(&bogusModel, bs) - 1);
}

VA(0x004d4c50, 0x65)
Int32 getInt32(BitStream *bs)
{
    UInt32 res = 0;

    res |= (getUChar(bs) << 24);
    res |= (getUChar(bs) << 16);
    res |= (getUChar(bs) <<  8);
    res |= (getUChar(bs)      );
    return (Int32)res;
}

VA(0x004d4cc0, 0x65)
UInt32 getUInt32(BitStream *bs)
{
    UInt32 res = 0;

    res |= (getUChar(bs) << 24);
    res |= (getUChar(bs) << 16);
    res |= (getUChar(bs) <<  8);
    res |= (getUChar(bs)      );
    return res;
}

VA(0x004d4d30, 0xda)
// void initModels(void);

VA(0x004d4e10, 0x71)
// void dumpAllModelStats(void);

VA(0x004d4e90, 0x153)
// int getMTFVal(struct BitStream *);

VA(0x004d4ff0, 0x27f)
// void sendMTFVal(struct BitStream *, int);

VA(0x004d5270, 0x94)
// void FreeCompressStructures(void);

VA(0x004d5310, 0xcf)
// void allocateCompressStructures(void);

VA(0x004d53e0, 0x94)
// void FreeDecompressStructures(void);

VA(0x004d5480, 0xe4)
// void setDecompressStructureSizes(int);

VA(0x004d5570, 0x22)
// unsigned int GETALL(int);

VA(0x004d55a0, 0x41)
// void SETREST16(int, unsigned int);

VA(0x004d55f0, 0x3e)
// void SETFIRST16(int, unsigned int);

VA(0x004d5630, 0x41)
// void SETREST(int, unsigned int);

VA(0x004d5680, 0x40)
// void SETFIRST(int, unsigned char);

VA(0x004d56c0, 0x40)
// void SETSECOND(int, unsigned char);

VA(0x004d5700, 0x40)
// void SETTHIRD(int, unsigned char);

VA(0x004d5740, 0x3d)
// void SETFOURTH(int, unsigned char);

VA(0x004d5780, 0x4d)
// int NORMALISE(int);

VA(0x004d57d0, 0x36)
// int NORMALISEHI(int);

VA(0x004d5810, 0x31)
// int NORMALISELO(int);

VA(0x004d5850, 0x39)
// int STRONG_NORMALISE(int);

VA(0x004d5890, 0x9d)
// void sendZeroes(struct BitStream *, int);

VA(0x004d5930, 0x189)
// void moveToFrontCodeAndSend(struct BitStream *, int);

VA(0x004d5ac0, 0x2d9)
// int getAndMoveToFrontDecode(struct BitStream *);

VA(0x004d5da0, 0x84)
// void stripe(void);

VA(0x004d5e30, 0x4f)
// void copyOffsetWords(void);

VA(0x004d5e80, 0x172)
// int fullGt(int, int);

VA(0x004d6000, 0x548)
// void qsortFull(int, int);

VA(0x004d6550, 0xbc)
// int trivialGt(int, int);

VA(0x004d6610, 0x10f)
// void shellTrivial(void);

VA(0x004d6720, 0x434)
// void sortIt(void);

VA(0x004d6b60, 0xa4)
// void doReversibleTransformation(void);

VA(0x004d6c10, 0x158)
// void undoReversibleTransformation(void);

VA(0x004d6d70, 0x1c1)
// void spotBlock(int);

VA(0x004d6f40, 0x15c)
// int getRLEpair(struct _iobuf *);

VA(0x004d70a0, 0x1eb)
// int loadAndRLEsource(struct _iobuf *);

VA(0x004d7290, 0x18d)
// void unRLEandDump(struct _iobuf *, int);

VA(0x004d7420, 0x2e6)
// void compressStream(struct _iobuf *, struct _iobuf *);

VA(0x004d7710, 0x26e)
// int uncompressStream(struct _iobuf *, struct _iobuf *);

VA(0x004d7980, 0x10)
void showFileNames(void)
{
}

VA(0x004d7990, 0x10)
void cleanUpAndFail(void)
{
}

VA(0x004d79a0, 0x16)
void panic(char *s)
{
}

VA(0x004d79c0, 0x4d)
// void crcError(unsigned int, unsigned int);

VA(0x004d7a10, 0x4a)
// void compressedStreamEOF(void);

VA(0x004d7a60, 0x4a)
// void ioError(void);

VA(0x004d7ab0, 0x3c)
// void blockOverrun(void);

VA(0x004d7af0, 0x3c)
// void unblockError(void);

VA(0x004d7b30, 0x3c)
// void bitStreamEOF(void);

VA(0x004d7b70, 0x37)
// void mySignalCatcher(int *);

VA(0x004d7bb0, 0x76)
// void mySIGSEGVorSIGBUScatcher(int *);

VA(0x004d7c30, 0x4d)
// void uncompressOutOfMemory(int, int);

VA(0x004d7c80, 0x4d)
// void compressOutOfMemory(int, int);

VA(0x004d7cd0, 0x83)
// int endsInBz(char *);

VA(0x004d7d60, 0xe2)
// void compress(char *);

VA(0x004d7e50, 0x110)
// void uncompress(char *);

VA(0x004d7f60, 0x2d5)
// long int EncodeData(char *, char *, unsigned long int);

VA(0x004d8240, 0x2f3)
// long int DecodeData(char *, char *, unsigned long int);

// ---- data / globals / vtables ----
DATA(0x0051ff50)  // unsigned int * crc32Table
DATA(0x00520350)  // unsigned int * words
DATA(0x00520354)  // int * zptr
DATA(0x00520358)  // int * ftab
DATA(0x0052035c)  // unsigned char * block
DATA(0x00520360)  // unsigned char * ll
DATA(0x00534ed0)  // struct BitStream aBitStreamBuffer
DATA(0x00534ee0)  // int origPtr
DATA(0x00534ee4)  // int longestFileName
DATA(0x00534ee8)  // int opMode
DATA(0x00534ef0)  // struct Model * models
DATA(0x00537010)  // unsigned int bitsOutstanding
DATA(0x00537014)  // int veryVerbose
DATA(0x00537018)  // unsigned int bigD
DATA(0x0053701c)  // unsigned int bigL
DATA(0x00537020)  // unsigned int bigR
DATA(0x00537024)  // struct _iobuf * outputHandleJustInCase
DATA(0x00537028)  // char * inName
DATA(0x00537428)  // int bytesOut
DATA(0x00537430)  // char * outName
DATA(0x00537830)  // struct Model bogusModel
DATA(0x00537c54)  // int verbose
DATA(0x00537c58)  // char * progNameReally
DATA(0x00538058)  // int blockSize100k
DATA(0x0053805c)  // int lastPP
DATA(0x00538060)  // int bytesIn
DATA(0x00538064)  // int compressing
DATA(0x00538068)  // int last
DATA(0x0053806c)  // int bsInUse
DATA(0x00538070)  // unsigned int globalCrc
DATA(0x00538074)  // char * progName
DATA(0x00538078)  // int keepInputFiles
