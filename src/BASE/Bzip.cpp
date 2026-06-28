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

#define BASIS         0
#define MODEL_2_3     1
#define MODEL_4_7     2
#define MODEL_8_15    3
#define MODEL_16_31   4
#define MODEL_32_63   5
#define MODEL_64_127  6
#define MODEL_128_255 7
Model models[8];

#define VAL_RUNA    1
#define VAL_RUNB    2
#define VAL_ONE     3
#define VAL_2_3     4
#define VAL_4_7     5
#define VAL_8_15    6
#define VAL_16_31   7
#define VAL_32_63   8
#define VAL_64_127  9
#define VAL_128_255 10
#define VAL_EOB     11
#define RUNA    257
#define RUNB    258
#define EOB     259
#define INVALID 260

#define NUM_FULLGT_UNROLLINGS 4
#define MAX_DENORM_OFFSET (4 * NUM_FULLGT_UNROLLINGS)
UInt32 *words;
Int32  *zptr;
Int32  *ftab;
UChar  *block;
UChar  *ll;
Int32   last;
Int32   lastPP;
Int32   origPtr;
Int32   blockSize100k;

#define IF_THEN_ELSE(c,t,e) ((c) ? (t) : (e))
#define GETFIRST(a)    ((UChar)(words[a] >> 24))
#define GETREST(a)     (words[a] & 0x00ffffff)
#define SETALL(a,w)    words[a] = (w)
#define GETFIRST16(a)  ((UInt32)(words[a] >> 16))
#define GETREST16(a)   (words[a] & 0x0000ffff)

void blockOverrun(void);

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
void initModels(void)
{
    initModel(&models[BASIS],         (char *)"basis",   11,  12,  1000);
    initModel(&models[MODEL_2_3],     (char *)"2-3",     2,   4,   1000);
    initModel(&models[MODEL_4_7],     (char *)"4-7",     4,   3,   1000);
    initModel(&models[MODEL_8_15],    (char *)"8-15",    8,   3,   1000);
    initModel(&models[MODEL_16_31],   (char *)"16-31",   16,  3,   1000);
    initModel(&models[MODEL_32_63],   (char *)"32-63",   32,  3,   1000);
    initModel(&models[MODEL_64_127],  (char *)"64-127",  64,  2,   1000);
    initModel(&models[MODEL_128_255], (char *)"128-255", 128, 1,   1000);
}

VA(0x004d4e10, 0x71)
void dumpAllModelStats(void)
{
    dumpModelStats(&bogusModel);
    dumpModelStats(&models[BASIS]);
    dumpModelStats(&models[MODEL_2_3]);
    dumpModelStats(&models[MODEL_4_7]);
    dumpModelStats(&models[MODEL_8_15]);
    dumpModelStats(&models[MODEL_16_31]);
    dumpModelStats(&models[MODEL_32_63]);
    dumpModelStats(&models[MODEL_64_127]);
    dumpModelStats(&models[MODEL_128_255]);
}

VA(0x004d4e90, 0x153)
Int32 getMTFVal(BitStream *bs)
{
    Int32 retVal;

    switch (getSymbol(&models[BASIS], bs)) {
        case VAL_EOB:
            retVal = EOB; break;
        case VAL_RUNA:
            retVal = RUNA; break;
        case VAL_RUNB:
            retVal = RUNB; break;
        case VAL_ONE:
            retVal = 1; break;
        case VAL_2_3:
            retVal = getSymbol(&models[MODEL_2_3], bs) + 2 - 1; break;
        case VAL_4_7:
            retVal = getSymbol(&models[MODEL_4_7], bs) + 4 - 1; break;
        case VAL_8_15:
            retVal = getSymbol(&models[MODEL_8_15], bs) + 8 - 1; break;
        case VAL_16_31:
            retVal = getSymbol(&models[MODEL_16_31], bs) + 16 - 1; break;
        case VAL_32_63:
            retVal = getSymbol(&models[MODEL_32_63], bs) + 32 - 1; break;
        case VAL_64_127:
            retVal = getSymbol(&models[MODEL_64_127], bs) + 64 - 1; break;
        default:
            retVal = getSymbol(&models[MODEL_128_255], bs) + 128 - 1; break;
    }
    return retVal;
}

VA(0x004d4ff0, 0x27f)
void sendMTFVal(BitStream *bs, Int32 n)
{
    if (n == RUNA) putSymbol(&models[BASIS], VAL_RUNA, bs); else
    if (n == RUNB) putSymbol(&models[BASIS], VAL_RUNB, bs); else
    if (n == EOB ) putSymbol(&models[BASIS], VAL_EOB,  bs); else

    if (n == 1) putSymbol(&models[BASIS], VAL_ONE, bs); else

    if (n >= 2 && n <= 3) {
        putSymbol(&models[BASIS], VAL_2_3, bs);
        putSymbol(&models[MODEL_2_3], n - 2 + 1, bs);
    } else

    if (n >= 4 && n <= 7) {
        putSymbol(&models[BASIS], VAL_4_7, bs);
        putSymbol(&models[MODEL_4_7], n - 4 + 1, bs);
    } else

    if (n >= 8 && n <= 15) {
        putSymbol(&models[BASIS], VAL_8_15, bs);
        putSymbol(&models[MODEL_8_15], n - 8 + 1, bs);
    } else

    if (n >= 16 && n <= 31) {
        putSymbol(&models[BASIS], VAL_16_31, bs);
        putSymbol(&models[MODEL_16_31], n - 16 + 1, bs);
    } else

    if (n >= 32 && n <= 63) {
        putSymbol(&models[BASIS], VAL_32_63, bs);
        putSymbol(&models[MODEL_32_63], n - 32 + 1, bs);
    } else

    if (n >= 64 && n <= 127) {
        putSymbol(&models[BASIS], VAL_64_127, bs);
        putSymbol(&models[MODEL_64_127], n - 64 + 1, bs);
    } else

    if (n >= 128 && n <= 255) {
        putSymbol(&models[BASIS], VAL_128_255, bs);
        putSymbol(&models[MODEL_128_255], n - 128 + 1, bs);
    } else {

        panic((char *)"sendMTFVal: bad value!");
    }
}

VA(0x004d5270, 0x94)
// void FreeCompressStructures(void);

VA(0x004d5310, 0xcf)
// void allocateCompressStructures(void);

VA(0x004d53e0, 0x94)
// void FreeDecompressStructures(void);

VA(0x004d5480, 0xe4)
// void setDecompressStructureSizes(int);

VA(0x004d5570, 0x22)
UInt32 GETALL(Int32 a)
{
    return words[a];
}

VA(0x004d55a0, 0x41)
void SETREST16(Int32 a, UInt32 w)
{
    words[a] = (words[a] & 0xffff0000) | (((UInt32)(w)) & 0x0000ffff);
}

VA(0x004d55f0, 0x3e)
void SETFIRST16(Int32 a, UInt32 w)
{
    words[a] = (words[a] & 0x0000ffff) | (((UInt32)(w)) << 16);
}

VA(0x004d5630, 0x41)
void SETREST(Int32 a, UInt32 w)
{
    words[a] = (words[a] & 0xff000000) | (((UInt32)(w)) & 0x00ffffff);
}

VA(0x004d5680, 0x40)
void SETFIRST(Int32 a, UChar c)
{
    words[a] = (words[a] & 0x00ffffff) | (((UInt32)(c)) << 24);
}

VA(0x004d56c0, 0x40)
void SETSECOND(Int32 a, UChar c)
{
    words[a] = (words[a] & 0xff00ffff) | (((UInt32)(c)) << 16);
}

VA(0x004d5700, 0x40)
void SETTHIRD(Int32 a, UChar c)
{
    words[a] = (words[a] & 0xffff00ff) | (((UInt32)(c)) << 8);
}

VA(0x004d5740, 0x3d)
void SETFOURTH(Int32 a, UChar c)
{
    words[a] = (words[a] & 0xffffff00) | (((UInt32)(c)));
}

VA(0x004d5780, 0x4d)
Int32 NORMALISE(Int32 p)
{
    return
    IF_THEN_ELSE(((p) < 0),
                 ((p) + lastPP),
                 IF_THEN_ELSE(((p) >= lastPP),
                              ((p) - lastPP),
                              (p)));
}

VA(0x004d57d0, 0x36)
Int32 NORMALISEHI(Int32 p)
{
    return
    IF_THEN_ELSE(((p) >= lastPP),
                 ((p) - lastPP),
                 (p));
}

VA(0x004d5810, 0x31)
Int32 NORMALISELO(Int32 p)
{
    return
    IF_THEN_ELSE(((p) < 0),
                 ((p) + lastPP),
                 (p));
}

VA(0x004d5850, 0x39)
Int32 STRONG_NORMALISE(Int32 p)
{
    while (p < 0) { p += lastPP; }
    return
        p % lastPP;
}

VA(0x004d5890, 0x9d)
void sendZeroes(BitStream *outStream, Int32 zeroesPending)
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
            sendMTFVal(outStream, RUNA); else
            sendMTFVal(outStream, RUNB);
        bitsToSend >>= 1;
        numBits--;
    }
}

VA(0x004d5930, 0x189)
void moveToFrontCodeAndSend(BitStream *outStream, Bool thisIsTheLastBlock)
{
    UChar  yy[256];
    Int32  i, j;
    UChar  tmp;
    UChar  tmp2;
    Int32  zeroesPending;

    zeroesPending = 0;
    if (thisIsTheLastBlock)
        putInt32(outStream, -(origPtr + 1)); else
        putInt32(outStream,  (origPtr + 1));

    initModels();

    for (i = 0; i <= 255; i++)
        yy[i] = (UChar)i;

    for (i = 0; i <= last; i++) {
        UChar ll_i;

        ll_i = GETFIRST(NORMALISELO(zptr[i] - 1));

        j = 0;
        tmp = yy[j];
        while (ll_i != tmp) {
            j++;
            tmp2 = tmp;
            tmp = yy[j];
            yy[j] = tmp2;
        }
        yy[0] = tmp;

        if (j == 0) {
            zeroesPending++;
        } else {
            sendZeroes(outStream, zeroesPending);
            zeroesPending = 0;
            sendMTFVal(outStream, j);
        }
    }
    sendZeroes(outStream, zeroesPending);
    sendMTFVal(outStream, EOB);
}

VA(0x004d5ac0, 0x2d9)
Bool getAndMoveToFrontDecode(BitStream *inStream)
{
    UChar  yy[256];
    Int32  i, j, tmpOrigPtr, nextSym, limit;

    limit = 100000 * blockSize100k;

    tmpOrigPtr = getInt32(inStream);
    if (tmpOrigPtr < 0)
        origPtr = (-tmpOrigPtr) - 1; else
        origPtr =   tmpOrigPtr  - 1;

    initModels();

    for (i = 0; i <= 255; i++)
        yy[i] = (UChar)i;

    last = -1;

    nextSym = getMTFVal(inStream);

    LOOPSTART:

    if (nextSym == EOB)
        return (tmpOrigPtr < 0);

    if (nextSym == RUNA || nextSym == RUNB) {
        Int32 n = 0;
        do {
            n <<= 1;
            if (nextSym == RUNA) n |= 1;
            n++;
            nextSym = getMTFVal(inStream);
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

        j = nextSym;
        for (; j > 3; j -= 4) {
            yy[j]   = yy[j-1];
            yy[j-1] = yy[j-2];
            yy[j-2] = yy[j-3];
            yy[j-3] = yy[j-4];
        }
        for (; j > 0; j--) yy[j] = yy[j-1];

        yy[0] = ll[last];
        nextSym = getMTFVal(inStream);
        goto LOOPSTART;
    }

    sprintf(gText, "bad MTF value %d\n", nextSym);
    LogStr(gText);
    panic((char *)"getAndMoveToFrontDecode\n");
    return True;
}

VA(0x004d5da0, 0x84)
void stripe(void)
{
    Int32 i;

    for (i = 0; i < lastPP; i++) {
        UChar c = GETFIRST(i);
        SETSECOND(NORMALISELO(i-1), c);
        SETTHIRD (NORMALISELO(i-2), c);
        SETFOURTH(NORMALISELO(i-3), c);
    }
}

VA(0x004d5e30, 0x4f)
void copyOffsetWords(void)
{
    Int32 i;

    for (i = 0; i < 4 * NUM_FULLGT_UNROLLINGS; i++)
        words[lastPP+i] = words[i];
}

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
