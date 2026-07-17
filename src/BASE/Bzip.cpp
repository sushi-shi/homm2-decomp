// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Bzip.obj   from: .\basewin.lib
// functions: 88   data: 32
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Bzip.h>       // bzip types/records + this TU's free-function decls
#include <BASE/Misc.h>       // LogStr, BaseAlloc, BaseFree, Random
#include <SOURCE/KB.h>       // FileError
#include <io.h>              // _open, _write, _close
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// Bzip has the string ops inlined (retail endsInBz/compress use repne scasb +
// rep movs, not calls), so intrinsics are on for this TU.
#pragma intrinsic(strcpy, strcat, strlen, memcpy)

// bzip-0.21 types/records live in BASE/Bzip.h (included above). Body-only macros stay here.
#define True  1
#define False 0
#define ERROR_IF_EOF(i)       { if ((i) == EOF)  ioError(); }
#define ERROR_IF_NOT_ZERO(i)  { if ((i) != 0)    ioError(); }
#define ERROR_IF_MINUS_ONE(i) { if ((i) == (-1)) ioError(); }

#define TWO_TO_THE(n) (1 << (n))
#define MAX_BITS_OUTSTANDING 500000000
#define smallB 26
#define smallF 18

DATA(0x00538070) unsigned int globalCrc;
DATA(0x0053806c) int bsInUse;
DATA(0x00534ed0) BitStream aBitStreamBuffer;
DATA(0x00537428) int bytesOut;
DATA(0x0053701c) UInt32 bigL;
DATA(0x00537020) UInt32 bigR;
DATA(0x00537018) UInt32 bigD;
DATA(0x00537010) UInt32 bitsOutstanding;
DATA(0x00537830) Model bogusModel;

#define BASIS         0
#define MODEL_2_3     1
#define MODEL_4_7     2
#define MODEL_8_15    3
#define MODEL_16_31   4
#define MODEL_32_63   5
#define MODEL_64_127  6
#define MODEL_128_255 7
DATA(0x00534ef0) Model models[8];

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
DATA(0x0051ff50) UInt32 crc32Table[256] = {
    0x00000000U, 0x04c11db7U, 0x09823b6eU, 0x0d4326d9U,
    0x130476dcU, 0x17c56b6bU, 0x1a864db2U, 0x1e475005U,
    0x2608edb8U, 0x22c9f00fU, 0x2f8ad6d6U, 0x2b4bcb61U,
    0x350c9b64U, 0x31cd86d3U, 0x3c8ea00aU, 0x384fbdbdU,
    0x4c11db70U, 0x48d0c6c7U, 0x4593e01eU, 0x4152fda9U,
    0x5f15adacU, 0x5bd4b01bU, 0x569796c2U, 0x52568b75U,
    0x6a1936c8U, 0x6ed82b7fU, 0x639b0da6U, 0x675a1011U,
    0x791d4014U, 0x7ddc5da3U, 0x709f7b7aU, 0x745e66cdU,
    0x9823b6e0U, 0x9ce2ab57U, 0x91a18d8eU, 0x95609039U,
    0x8b27c03cU, 0x8fe6dd8bU, 0x82a5fb52U, 0x8664e6e5U,
    0xbe2b5b58U, 0xbaea46efU, 0xb7a96036U, 0xb3687d81U,
    0xad2f2d84U, 0xa9ee3033U, 0xa4ad16eaU, 0xa06c0b5dU,
    0xd4326d90U, 0xd0f37027U, 0xddb056feU, 0xd9714b49U,
    0xc7361b4cU, 0xc3f706fbU, 0xceb42022U, 0xca753d95U,
    0xf23a8028U, 0xf6fb9d9fU, 0xfbb8bb46U, 0xff79a6f1U,
    0xe13ef6f4U, 0xe5ffeb43U, 0xe8bccd9aU, 0xec7dd02dU,
    0x34867077U, 0x30476dc0U, 0x3d044b19U, 0x39c556aeU,
    0x278206abU, 0x23431b1cU, 0x2e003dc5U, 0x2ac12072U,
    0x128e9dcfU, 0x164f8078U, 0x1b0ca6a1U, 0x1fcdbb16U,
    0x018aeb13U, 0x054bf6a4U, 0x0808d07dU, 0x0cc9cdcaU,
    0x7897ab07U, 0x7c56b6b0U, 0x71159069U, 0x75d48ddeU,
    0x6b93dddbU, 0x6f52c06cU, 0x6211e6b5U, 0x66d0fb02U,
    0x5e9f46bfU, 0x5a5e5b08U, 0x571d7dd1U, 0x53dc6066U,
    0x4d9b3063U, 0x495a2dd4U, 0x44190b0dU, 0x40d816baU,
    0xaca5c697U, 0xa864db20U, 0xa527fdf9U, 0xa1e6e04eU,
    0xbfa1b04bU, 0xbb60adfcU, 0xb6238b25U, 0xb2e29692U,
    0x8aad2b2fU, 0x8e6c3698U, 0x832f1041U, 0x87ee0df6U,
    0x99a95df3U, 0x9d684044U, 0x902b669dU, 0x94ea7b2aU,
    0xe0b41de7U, 0xe4750050U, 0xe9362689U, 0xedf73b3eU,
    0xf3b06b3bU, 0xf771768cU, 0xfa325055U, 0xfef34de2U,
    0xc6bcf05fU, 0xc27dede8U, 0xcf3ecb31U, 0xcbffd686U,
    0xd5b88683U, 0xd1799b34U, 0xdc3abdedU, 0xd8fba05aU,
    0x690ce0eeU, 0x6dcdfd59U, 0x608edb80U, 0x644fc637U,
    0x7a089632U, 0x7ec98b85U, 0x738aad5cU, 0x774bb0ebU,
    0x4f040d56U, 0x4bc510e1U, 0x46863638U, 0x42472b8fU,
    0x5c007b8aU, 0x58c1663dU, 0x558240e4U, 0x51435d53U,
    0x251d3b9eU, 0x21dc2629U, 0x2c9f00f0U, 0x285e1d47U,
    0x36194d42U, 0x32d850f5U, 0x3f9b762cU, 0x3b5a6b9bU,
    0x0315d626U, 0x07d4cb91U, 0x0a97ed48U, 0x0e56f0ffU,
    0x1011a0faU, 0x14d0bd4dU, 0x19939b94U, 0x1d528623U,
    0xf12f560eU, 0xf5ee4bb9U, 0xf8ad6d60U, 0xfc6c70d7U,
    0xe22b20d2U, 0xe6ea3d65U, 0xeba91bbcU, 0xef68060bU,
    0xd727bbb6U, 0xd3e6a601U, 0xdea580d8U, 0xda649d6fU,
    0xc423cd6aU, 0xc0e2d0ddU, 0xcda1f604U, 0xc960ebb3U,
    0xbd3e8d7eU, 0xb9ff90c9U, 0xb4bcb610U, 0xb07daba7U,
    0xae3afba2U, 0xaafbe615U, 0xa7b8c0ccU, 0xa379dd7bU,
    0x9b3660c6U, 0x9ff77d71U, 0x92b45ba8U, 0x9675461fU,
    0x8832161aU, 0x8cf30badU, 0x81b02d74U, 0x857130c3U,
    0x5d8a9099U, 0x594b8d2eU, 0x5408abf7U, 0x50c9b640U,
    0x4e8ee645U, 0x4a4ffbf2U, 0x470cdd2bU, 0x43cdc09cU,
    0x7b827d21U, 0x7f436096U, 0x7200464fU, 0x76c15bf8U,
    0x68860bfdU, 0x6c47164aU, 0x61043093U, 0x65c52d24U,
    0x119b4be9U, 0x155a565eU, 0x18197087U, 0x1cd86d30U,
    0x029f3d35U, 0x065e2082U, 0x0b1d065bU, 0x0fdc1becU,
    0x3793a651U, 0x3352bbe6U, 0x3e119d3fU, 0x3ad08088U,
    0x2497d08dU, 0x2056cd3aU, 0x2d15ebe3U, 0x29d4f654U,
    0xc5a92679U, 0xc1683bceU, 0xcc2b1d17U, 0xc8ea00a0U,
    0xd6ad50a5U, 0xd26c4d12U, 0xdf2f6bcbU, 0xdbee767cU,
    0xe3a1cbc1U, 0xe760d676U, 0xea23f0afU, 0xeee2ed18U,
    0xf0a5bd1dU, 0xf464a0aaU, 0xf9278673U, 0xfde69bc4U,
    0x89b8fd09U, 0x8d79e0beU, 0x803ac667U, 0x84fbdbd0U,
    0x9abc8bd5U, 0x9e7d9662U, 0x933eb0bbU, 0x97ffad0cU,
    0xafb010b1U, 0xab710d06U, 0xa6322bdfU, 0xa2f33668U,
    0xbcb4666dU, 0xb8757bdaU, 0xb5365d03U, 0xb1f740b4U
};
DATA(0x00520350) UInt32 *words = 0;
DATA(0x00520354) Int32  *zptr = 0;
DATA(0x00520358) Int32  *ftab = 0;
DATA(0x0052035c) UChar  *block = 0;
DATA(0x00520360) UChar  *ll = 0;
DATA(0x00538068) Int32   last;
DATA(0x0053805c) Int32   lastPP;
DATA(0x00534ee0) Int32   origPtr;
DATA(0x00538058) Int32   blockSize100k;
DATA(0x00537014) Int32   veryVerbose;
DATA(0x00538074) Char   *progName;
DATA(0x00538064) Int32   compressing;
DATA(0x00538060) Int32   bytesIn;
DATA(0x00537c54) Int32   verbose;
DATA(0x00537024) FILE   *outputHandleJustInCase;

#define MY_EOF 257
#define UPDATE_CRC(crcVar,cha)              \
{                                           \
   crcVar = (crcVar << 8) ^                 \
            crc32Table[(crcVar >> 24) ^     \
                       ((UChar)cha)];       \
}

#define IF_THEN_ELSE(c,t,e) ((c) ? (t) : (e))
#define GETFIRST(a)    ((UChar)(words[a] >> 24))
#define GETREST(a)     (words[a] & 0x00ffffff)
#define SETALL(a,w)    words[a] = (w)
#define GETFIRST16(a)  ((UInt32)(words[a] >> 16))
#define GETREST16(a)   (words[a] & 0x0000ffff)

// (Bzip's own free-function declarations are in BASE/Bzip.h; the externs they used —
// gText, LogStr, BaseAlloc, BaseFree, FileError, Random — come from Misc.h/KB.h/_globals.h;
// _open/_write/_close from <io.h>.)
// NWC wraps malloc/free in BaseAlloc/BaseFree(ptr, __FILE__, __LINE__). __FILE__ is
// the original build path (reloc-masked); __LINE__ immediates are hardcoded from the
// retail disasm since our line layout differs.
#define BZFILE const_cast<char *>("I:\\Projects\\Heroes\\Prog\\BASE\\Bzip.cpp")

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
    if (bsInUse) panic(const_cast<char *>("bsOpenReadStream"));
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
    if (bsInUse) panic(const_cast<char *>("bsOpenWriteStream"));
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
    if (!bsInUse) panic(const_cast<char *>("bsClose"));
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
        if ((0[&bigR] + bigL) <= TWO_TO_THE(smallB - 1)) {
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

// @early-stop
// Relocation-masked instructions are byte-identical and all eight effective targets agree;
// the sole residual is the delinked panic-string symbol identity ($SG3428 vs $SG3417).
VA(0x004d46c0, 0xd7)
void arithCodeSymbol(BitStream *bs, Model *m, Int32 symbol)
{
    UInt32 cumulativeLow8, cumulativeHigh2, totalFrequency14, rangeWidth29, rangeProduct8;
    Int32 symbolIndex9;

    totalFrequency14 = m->totFreq;
    cumulativeLow8 = 0;
    for (symbolIndex9 = 1; symbolIndex9 < 0[&symbol]; symbolIndex9++)
        cumulativeLow8 += m->freq[symbolIndex9];
    cumulativeHigh2 = cumulativeLow8 + m->freq[symbol];

    rangeWidth29 = bigR / totalFrequency14;

    rangeProduct8 = cumulativeLow8 * rangeWidth29;
    bigL = bigL + rangeProduct8;

    if (cumulativeHigh2 < totalFrequency14)
        bigR = rangeWidth29 * (cumulativeHigh2 - cumulativeLow8); else
        bigR = bigR - rangeProduct8;

    arithCodeRenormalise_Encode(bs);

    if (bitsOutstanding > MAX_BITS_OUTSTANDING)
        panic(const_cast<char *>("arithCodeSymbol: too many bits outstanding"));
}

// @early-stop
// All 85 instructions agree and both objects contain the same 11 relocation targets;
// the remaining report residual is delinked local-label/padding identity metadata.
VA(0x004d47a0, 0xfe)
Int32 arithDecodeSymbol(BitStream *bs, Model *m)
{
    UInt32 cumulativeLow7, scaledTarget1, cumulativeHigh1, totalFrequency13,
           rangeWidth28, rangeProduct26, decodedSymbol5;

    totalFrequency13 = m->totFreq;

    rangeWidth28 = bigR / totalFrequency13;
    scaledTarget1 = minUInt32(totalFrequency13 - 1, bigD / rangeWidth28);

    decodedSymbol5 = 0;
    cumulativeHigh1 = 0;
    while (cumulativeHigh1 <= scaledTarget1) {
        decodedSymbol5++;
        cumulativeHigh1 += m->freq[decodedSymbol5];
    }
    cumulativeLow7 = cumulativeHigh1 - m->freq[decodedSymbol5];

    rangeProduct26 = rangeWidth28 * cumulativeLow7;
    bigD = bigD - rangeProduct26;

    if (cumulativeHigh1 < totalFrequency13)
        bigR = rangeWidth28 * (cumulativeHigh1 - cumulativeLow7); else
        bigR = bigR - rangeProduct26;

    while (bigR <= TWO_TO_THE(smallB - 2)) {
        bigR = 2 * bigR;
        bigD = 2 * bigD + bsGetBit(bs);
    }

    return (Int32)decodedSymbol5;
}

VA(0x004d48a0, 0xe0)
void initModel(Model *m, Char *initName, Int32 initNumSymbols, Int32 initIncValue, Int32 initNoExceed)
{
    Int32 i;

    if (initIncValue == 0) {
        m->totFreq = initNumSymbols;
        for (i = 1; i <= 0[&initNumSymbols]; i++)
            m->freq[i] = 1;
    } else {
        m->totFreq = initNumSymbols * initIncValue;
        for (i = 1; i <= 0[&initNumSymbols]; i++)
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
    initModel(&bogusModel, const_cast<char *>("bogus"), 256, 0, 256);
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
    initModel(&models[BASIS],         const_cast<char *>("basis"),   11,  12,  1000);
    initModel(&models[MODEL_2_3],     const_cast<char *>("2-3"),     2,   4,   1000);
    initModel(&models[MODEL_4_7],     const_cast<char *>("4-7"),     4,   3,   1000);
    initModel(&models[MODEL_8_15],    const_cast<char *>("8-15"),    8,   3,   1000);
    initModel(&models[MODEL_16_31],   const_cast<char *>("16-31"),   16,  3,   1000);
    initModel(&models[MODEL_32_63],   const_cast<char *>("32-63"),   32,  3,   1000);
    initModel(&models[MODEL_64_127],  const_cast<char *>("64-127"),  64,  2,   1000);
    initModel(&models[MODEL_128_255], const_cast<char *>("128-255"), 128, 1,   1000);
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

// @early-stop
// The explicit 0x153-byte range is raw-byte exact after masking the union of the 28
// aligned relocation fields.  The only identity differences are models interior aliases
// and the embedded 0x2c-byte jump table's local labels; effective targets/addends agree.
// Retail's next-public row has one alignment NOP at +0x153, outside this function range.
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

        panic(const_cast<char *>("sendMTFVal: bad value!"));
    }
}

VA(0x004d5270, 0x94)
void FreeCompressStructures(void)
{
    if (words != NULL) H2_FREE(words, BZFILE, 0x461);
    if (ftab  != NULL) H2_FREE(ftab,  BZFILE, 0x462);
    if (zptr  != NULL) H2_FREE(zptr,  BZFILE, 0x463);
    words = NULL;
    ftab  = NULL;
    zptr  = NULL;
}

VA(0x004d5310, 0xcf)
void allocateCompressStructures(void)
{
    Int32 n = 100000 * blockSize100k;
    FreeCompressStructures();
    words = (UInt32 *)H2_ALLOC((n + MAX_DENORM_OFFSET) * sizeof(Int32) + 1, BZFILE, 0x475);
    zptr  = (Int32 *)H2_ALLOC(n                        * sizeof(Int32) + 1, BZFILE, 0x476);
    ftab  = (Int32 *)H2_ALLOC(65537                    * sizeof(Int32) + 1, BZFILE, 0x477);

    if (words == NULL || zptr == NULL || ftab == NULL) {
        Int32 totalDraw = (n + MAX_DENORM_OFFSET) * sizeof(Int32) +
                          n * sizeof(Int32) +
                          65537 * sizeof(Int32);

        compressOutOfMemory(totalDraw, n);
    }
}

VA(0x004d53e0, 0x94)
void FreeDecompressStructures(void)
{
    if (block != NULL) H2_FREE(block, BZFILE, 0x489);
    if (ll    != NULL) H2_FREE(ll,    BZFILE, 0x48a);
    if (zptr  != NULL) H2_FREE(zptr,  BZFILE, 0x48b);
    block = NULL;
    ll    = NULL;
    zptr  = NULL;
}

VA(0x004d5480, 0xe4)
void setDecompressStructureSizes(Int32 newSize100k)
{
    if (0[&newSize100k] == blockSize100k)
        return;

    blockSize100k = newSize100k;
    FreeDecompressStructures();

    if (newSize100k != 0) {
        Int32 n = 100000 * newSize100k;
        block = (UChar *)H2_ALLOC(n * sizeof(UChar) + 1, BZFILE, 0x4a1);
        ll    = (UChar *)H2_ALLOC(n * sizeof(UChar) + 1, BZFILE, 0x4a2);
        zptr  = (Int32 *)H2_ALLOC(n * sizeof(Int32) + 1, BZFILE, 0x4a3);

        if (block == NULL || ll == NULL || zptr == NULL) {
            Int32 totalDraw = 6 * n * sizeof(UChar);
            uncompressOutOfMemory(totalDraw, n);
        }
    }
}

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
                 (lastPP + (p)),
                 IF_THEN_ELSE(((lastPP > (p)) == False),
                              ((p) - lastPP),
                              (p)));
}

// @early-stop
// All 22 instructions and both lastPP+0 relocations agree; the retained residual is
// local-label/padding identity outside the executable instruction stream.
VA(0x004d57d0, 0x36)
Int32 NORMALISEHI(Int32 p)
{
    return
    IF_THEN_ELSE(((p) >= 0[&lastPP]),
                 ((p) - lastPP),
                 (p));
}

// @early-stop
// All 20 instructions and the lastPP+0 relocation agree; the retained residual is
// local-label/padding identity outside the executable instruction stream.
VA(0x004d5810, 0x31)
Int32 NORMALISELO(Int32 p)
{
    return
    IF_THEN_ELSE(((p) < 0),
                 (((p) | 0) + lastPP),
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

// @early-stop
// All 106 instructions, the 0x120 frame, and all 13 effective relocation targets/addends
// agree; the retained residual is delinked local-label identity metadata.
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
    UChar  symbols[256];
    Int32  i, j, encodedOrigin, nextSym, blockLimit;

    blockLimit = 100000 * blockSize100k;

    encodedOrigin = getInt32(inStream);
    if (encodedOrigin < 0)
        origPtr = (-encodedOrigin) - 1; else
        origPtr =   encodedOrigin  - 1;

    initModels();

    for (i = 0; i <= 255; i++)
        symbols[i] = (UChar)i;

    last = -1;

    nextSym = getMTFVal(inStream);

    LOOPSTART:

    if (nextSym == EOB)
        return (encodedOrigin < 0);

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
            last++; if (last >= 0[&blockLimit]) blockOverrun();
            ll[last] = symbols[0];
            n--;
        }
        goto LOOPSTART;
    }

    if (nextSym >= 1 && nextSym <= 255) {
        last++; if (last >= 0[&blockLimit]) blockOverrun();
        ll[last] = symbols[nextSym];

        j = nextSym;
        for (; j > 3; j -= 4) {
            symbols[j]   = symbols[j-1];
            symbols[j-1] = symbols[j-2];
            symbols[j-2] = symbols[j-3];
            symbols[j-3] = symbols[j-4];
        }
        for (; j > 0; j--) symbols[j] = symbols[j-1];

        symbols[0] = ll[last];
        nextSym = getMTFVal(inStream);
        goto LOOPSTART;
    }

    sprintf(gText, "bad MTF value %d\n", nextSym);
    LogStr(gText);
    panic(const_cast<char *>("getAndMoveToFrontDecode\n"));
    return True;
}

VA(0x004d5da0, 0x84)
void stripe(void)
{
    Int32 i;

    for (i = 0; 0[&i] < lastPP; i++) {
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

// @early-stop
// All 113 instructions and all ten effective relocation targets/addends agree; the
// retained residual is delinked local-label identity metadata.
VA(0x004d5e80, 0x172)
Bool fullGt(Int32 i1, Int32 i2)
{
    Int32 i1orig = i1;

    if (i1 == i2) return False;

    do {
        UInt32 w1;
        UInt32 w2;

        w1 = GETALL(i1);
        w2 = GETALL(i2);
        if (w1 != w2) return (w1 > w2);
        i1 += 4;
        i2 += 4;

        w1 = GETALL(i1);
        w2 = GETALL(i2);
        if (w1 != w2) return (w1 > w2);
        i1 += 4;
        i2 += 4;

        w1 = GETALL(i1);
        w2 = GETALL(i2);
        if (w1 != w2) return (w1 > w2);
        i1 += 4;
        i2 += 4;

        w1 = GETALL(i1);
        w2 = GETALL(i2);
        if (w1 != w2) return (w1 > w2);
        i1 += 4;
        i2 += 4;

        i1 = NORMALISEHI(i1);
        i2 = NORMALISEHI(i2);

    }
        while (i1 != i1orig);
    return False;
}

#define ISORT_BELOW 10
#define RC(x) (x)
#define SWAP(za,zb)                                           \
   { Int32 zl = (za); Int32 zr = (zb);                        \
     Int32 zt = zptr[RC(zl)]; zptr[RC(zl)] = zptr[RC(zr)];    \
     zptr[RC(zr)] = zt;                                       \
   }

// @semantic
// The recovered 0x1b0 frame, sorting CFG, and all 44 effective relocations agree.  The
// first structural residual is +0x18: candidate initializes [ebp-8], retail [ebp-0xc0];
// the resulting local/macro-temporary slots remain a compiler-shape wall. No speculative
// slot renaming was retained; revisit only with a complete retail role-to-slot map.
VA(0x004d6000, 0x548)
void qsortFull(Int32 left, Int32 right)
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

        if (wuR - wuL > ISORT_BELOW) {

            wuC = (wuL + wuR) >> 1;
            if (fullGt(zptr[RC(wuL)], zptr[RC(wuC)])) SWAP(wuL, wuC);
            if (fullGt(zptr[RC(wuL)], zptr[RC(wuR)])) SWAP(wuL, wuR);
            if (fullGt(zptr[RC(wuC)], zptr[RC(wuR)])) SWAP(wuC, wuR);

            SWAP(wuC, wuR-1);
            pivot = zptr[RC(wuR-1)];

            i = wuL;
            j = wuR - 1;
            for (;;) {
                do i++; while (fullGt(pivot, zptr[RC(i)]));
                do j--; while (fullGt(zptr[RC(j)], pivot));
                if (i < j) SWAP(i, j) else break;
            }
            SWAP(i, wuR-1);

            if ((i - wuL) > (wuR - i)) {
                stackL[sp] = wuL; stackR[sp] = i-1; sp++; wuL = i+1;
            } else {
                stackL[sp] = i+1; stackR[sp] = wuR; sp++; wuR = i-1;
            }

        } else {

            for (i = wuL + 1; i <= wuR; i++) {
                v = zptr[RC(i)];
                j = i;
                while (fullGt(zptr[RC(j-1)], v)) {
                    zptr[RC(j)] = zptr[RC(j-1)];
                    j = j - 1;
                    if (j <= wuL) goto zero;
                }
                zero:
                zptr[RC(j)] = v;
            }
            if (sp == 0) return;
            sp--; wuL = stackL[sp]; wuR = stackR[sp];

        }
    }
}

#undef RC
#undef SWAP
#undef ISORT_BELOW

VA(0x004d6550, 0xbc)
Bool trivialGt(Int32 i1, Int32 i2)
{
    Int32 k;

    for (k = 0; 0[&k] <= last; k++) {
        UChar c1 = GETFIRST(i1);
        UChar c2 = GETFIRST(i2);
        if (c1 == c2) {
            i1++; i1 = NORMALISEHI(i1);
            i2++; i2 = NORMALISEHI(i2);
        } else
        if (c1 > c2) return True; else return False;
    }
    return False;
}

// @early-stop
// All 88 instructions, the 0x1c frame, and all seven effective relocation targets/addends
// agree; the retained residual is delinked local-label identity metadata.
VA(0x004d6610, 0x10f)
void shellTrivial(void)
{
    Int32 i, j, h, bigN;
    Int32 v;

    Int32 ptrLo = 0;
    Int32 ptrHi = last;
    bigN = ptrHi - ptrLo + 1;
    h = 1;
    do { h = 3 * h + 1; } while (!(h > bigN));
    do {
        h = h / 3;
        for (i = ptrLo + h; i <= ptrHi; i++) {
            v = zptr[i];
            j = i;
            while (trivialGt(zptr[j-h], v)) {
                zptr[j] = zptr[j-h];
                j = j - h;
                if (j <= (ptrLo + h - 1)) goto zero;
            }
            zero:
            zptr[j] = v;
        }
    } while (h != 1);
}

// @semantic
// The trivial/graded BWT paths, 0x3c frame, five grade ranges, CFG, and all 68 effective
// relocations agree. Three global/local loop comparisons were steered exactly with 0[&i].
// The first remaining raw-code divergence is +0xdf: candidate i is [ebp-8], retail
// [ebp-0xc]; nested-scope slots and the 0x14-byte grade jump table remain compiler shape.
VA(0x004d6720, 0x434)
void sortIt(void)
{
    lastPP = last + 1;

    if (lastPP <= 1024) {

        Int32 i;

        if (veryVerbose) { sprintf(gText, "trivialSort ...\n"); LogStr(gText); }
        for (i = 0; 0[&i] <= last; i++) zptr[i] = i;
        shellTrivial();
        if (veryVerbose) { sprintf(gText, "trivialSort done.\n"); LogStr(gText); }

    } else {
        Int32 i;
        Int32 grade;
        Int32 notDone;

        stripe();

        if (veryVerbose) { sprintf(gText, "bucket sorting ...\n"); LogStr(gText); }

        for (i = 0; i <= 65536; i++)
            ftab[i] = 0;
        for (i = 0; 0[&i] <= last; i++)
            ftab[GETFIRST16(i)]++;
        for (i = 1; i <= 65536; i++)
            ftab[i] += ftab[i-1];

        for (i = 0; 0[&i] <= last; i++) {
            UInt32 j = GETFIRST16(i);
            ftab[j]--;
            zptr[ftab[j]] = i;
        }

        copyOffsetWords();

        notDone = lastPP;
        for (grade = 1; grade <= 5; grade++) {
            Int32 candNo;
            Int32 loBound = 0;
            Int32 hiBound = 0;

            switch (grade) {
                case 1:  loBound = 2;     hiBound = 15;     break;
                case 2:  loBound = 16;    hiBound = 255;    break;
                case 3:  loBound = 256;   hiBound = 4095;   break;
                case 4:  loBound = 4096;  hiBound = 65535;  break;
                case 5:  loBound = 65536; hiBound = 900000; break;
                default: panic(const_cast<char *>("gradedSort")); break;
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
                        sprintf(gText,
                                "   %d -> %d:  cand %5d,   freq = %6d,   notdone = %6d",
                                loBound, hiBound, candNo, freqHere, notDone);
                        LogStr(gText);
                    }

                    qsortFull(lower, upper);

                    if (freqHere < 65535) {
                        for (j = lower, k = 0; j <= upper; j++, k++) {
                            Int32 a2update = zptr[j];
                            SETREST16(a2update, k);
                            if (a2update < (4 * NUM_FULLGT_UNROLLINGS))
                                SETREST16(a2update + lastPP, k);
                        }
                    }
                    if (veryVerbose) { sprintf(gText, " "); LogStr(gText); }

                }
            }
        }
    }
}

VA(0x004d6b60, 0xa4)
void doReversibleTransformation(void)
{
    Int32 i;

    if (veryVerbose) { sprintf(gText, " "); LogStr(gText); }

    sortIt();

    origPtr = -1;
    for (i = 0; i <= last; i++)
        if (zptr[i] == 0)
            { origPtr = i; break; }

    if (origPtr == -1) panic(const_cast<char *>("doReversibleTransformation"));
}

VA(0x004d6c10, 0x158)
void undoReversibleTransformation(void)
{
    Int32  frequencyByChar[256];
    Int32  i, j, currentChar, total;

    for (i = 0; i <= 255; i++) frequencyByChar[i] = 0;

    for (i = 0; 0[&i] <= last; i++) {
        UChar ll_i = ll[i];
        zptr[i] = frequencyByChar[ll_i];
        frequencyByChar[ll_i]++;
    }

    total = 0;
    for (currentChar = 0; currentChar <= 255; currentChar++) {
        total = total + frequencyByChar[currentChar];
        frequencyByChar[currentChar] = total - frequencyByChar[currentChar];
    }

    i = origPtr;
    for (j = last; j >= 0; j--) {
        UChar ll_i = ll[i];
        block[j] = ll_i;
        i = zptr[i] + frequencyByChar[ll_i];
    }
}

#define SPOT_BASIS_STEP 8000

VA(0x004d6d70, 0x1c1)
void spotBlock(Bool weAreCompressing)
{
    Int32 spotPos, delta, updatedDelta;

    spotPos = SPOT_BASIS_STEP;
    delta = 1;

    while (0[&spotPos] < last) {

        Int32 n;

        if (weAreCompressing)
            n = (Int32)GETFIRST(spotPos) + 1; else
            n = (Int32)block[spotPos]    - 1;

        if (n == 256) n = 0; else if (n == -1)  n = 255;

        if (!(n >= 0 && n <= 255)) panic(const_cast<char *>("spotBlock"));

        if (weAreCompressing)
            SETFIRST(spotPos, (UChar)n); else
            block[spotPos] = (UChar)n;

        switch (delta) {
            case 3:  updatedDelta = 1; break;
            case 1:  updatedDelta = 4; break;
            case 4:  updatedDelta = 5; break;
            case 5:  updatedDelta = 9; break;
            case 9:  updatedDelta = 2; break;
            case 2:  updatedDelta = 6; break;
            case 6:  updatedDelta = 7; break;
            case 8:  updatedDelta = 8; break;
            case 7:  updatedDelta = 3; break;
            default: updatedDelta = 1; break;
        }
        delta = updatedDelta;

        spotPos = spotPos + SPOT_BASIS_STEP + 17 * (updatedDelta - 5);
    }
}

VA(0x004d6f40, 0x15c)
Int32 getRLEpair(FILE *src)
{
    Int32     runLen;
    IntNative ch, latestCh;

    ch = getc(src);

    if (ch == EOF) {
        ERROR_IF_NOT_ZERO(errno);
        return (1 << 16) | MY_EOF;
    }

    runLen = 0;
    do {
        latestCh = getc(src);
        runLen++;
        bytesIn++;
    }
        while (ch == latestCh && runLen < 255);

    if (latestCh != EOF) {
        if (ungetc(latestCh, src) == EOF)
            panic(const_cast<char *>("getRLEpair: ungetc failed"));
    } else {
        ERROR_IF_NOT_ZERO(errno);
    }

    if (runLen == 1) {
        UPDATE_CRC(globalCrc, (UChar)ch);
        return (1 << 16) | ch;
    } else {
        Int32 i;
        for (i = 1; 0[&i] <= runLen; i++)
            UPDATE_CRC(globalCrc, (UChar)ch);
        return (runLen << 16) | ch;
    }
}

VA(0x004d70a0, 0x1eb)
Bool loadAndRLEsource(FILE *src)
{
    Int32 currentChar, allowableBlockSize;

    last = -1;
    currentChar = 0;

    allowableBlockSize = 100000 * blockSize100k - 20;

    while (last < 0[&allowableBlockSize] && currentChar != MY_EOF) {
        Int32 rlePair, runLength;
        rlePair = getRLEpair(src);
        currentChar = rlePair & 0xFFFF;
        runLength   = (UInt32)rlePair >> 16;

        if (currentChar == MY_EOF)
            { last++; SETFIRST(last, ((UChar)42)); }
            else
            switch (runLength) {
                case 1:
                    last++; SETFIRST(last, ((UChar)currentChar)); break;
                case 2:
                    last++; SETFIRST(last, ((UChar)currentChar));
                    last++; SETFIRST(last, ((UChar)currentChar)); break;
                case 3:
                    last++; SETFIRST(last, ((UChar)currentChar));
                    last++; SETFIRST(last, ((UChar)currentChar));
                    last++; SETFIRST(last, ((UChar)currentChar)); break;
                default:
                    last++; SETFIRST(last, ((UChar)currentChar));
                    last++; SETFIRST(last, ((UChar)currentChar));
                    last++; SETFIRST(last, ((UChar)currentChar));
                    last++; SETFIRST(last, ((UChar)currentChar));
                    last++; SETFIRST(last, ((UChar)(runLength-4))); break;
            }
    }
    return (currentChar == MY_EOF);
}

// @early-stop
// All 125 instructions, the 0x28 frame, and all 15 effective relocation targets/addends
// agree; the retained residual is delinked local-label identity metadata.
VA(0x004d7290, 0x18d)
void unRLEandDump(FILE *dst, Bool thisIsTheLastBlock)
{
    IntNative retVal;
    Int32     lastCharToSpew, i, count, chPrev, ch;
    UInt32    localCrc;

    if (thisIsTheLastBlock)
        lastCharToSpew = last - 1; else
        lastCharToSpew = last;

    count    = 0;
    i        = 0;
    ch       = 256;
    localCrc = getGlobalCRC();

    while (i <= lastCharToSpew) {
        chPrev = ch;
        ch = block[i];
        i++;

        retVal = putc(ch, dst);
        ERROR_IF_EOF(retVal);
        UPDATE_CRC(localCrc, (UChar)ch);

        if (ch != chPrev) {
            count = 1;
        } else {
            count++;
            if (count >= 4) {
                Int32 j;
                for (j = 0;  j < (Int32)block[i];  j++) {
                    retVal = putc(ch, dst);
                    ERROR_IF_EOF(retVal);
                    UPDATE_CRC(localCrc, (UChar)ch);
                }
                i++;
                count = 0;
            }
        }
    }

    setGlobalCRC(localCrc);

    if (thisIsTheLastBlock && block[last] != 42) unblockError();
}

// @early-stop
// Relocation-masked instructions, the 0x34 frame, CFG, and all 79 effective targets agree.
// Residuals are local string/constant identities and the __adjust_fdiv alias for retail's
// ?iLeftRightSave+0x10; the resolved addresses and owner-relative values are identical.
VA(0x004d7420, 0x2e6)
void compressStream(FILE *stream, FILE *zStream)
{
    IntNative  retVal;
    Bool       thisIsTheLastBlock;
    BitStream  *zbs;
    UInt32     crcToSend;
    Int32      blockNo = 1;

    bytesIn  = 0;
    bytesOut = 0;

    zbs = bsOpenWriteStream(zStream);

    bsPutUChar(zbs, 'B');
    bsPutUChar(zbs, 'Z');
    bsPutUChar(zbs, '0');
    bsPutUChar(zbs, '0' + blockSize100k);

    initialiseCRC();
    initBogusModel();
    arithCodeStartEncoding(zbs);

    do {
        if (veryVerbose) { sprintf(gText, "\nBEGIN block %d\n", blockNo); LogStr(gText); }
        blockNo++;
        thisIsTheLastBlock = loadAndRLEsource(stream);
        spotBlock(True);
        doReversibleTransformation();
        moveToFrontCodeAndSend(zbs, thisIsTheLastBlock);
    }
        while (!thisIsTheLastBlock);

    crcToSend = getFinalCRC();
    putUInt32(zbs, crcToSend);
    if (veryVerbose) { sprintf(gText, "\nCRC = 0x%x\n", crcToSend); LogStr(gText); }

    arithCodeDoneEncoding(zbs);
    bsClose(zbs);
    ERROR_IF_NOT_ZERO(ferror(stream));
    retVal = fclose(stream);
    ERROR_IF_EOF(retVal);

    if (veryVerbose) {
        sprintf(gText, " "); LogStr(gText);
        dumpAllModelStats();
        sprintf(gText, " "); LogStr(gText);
    }

    if (bytesIn == 0) bytesIn = 1;
    if (bytesOut == 0) bytesOut = 1;

    if (verbose) {
        sprintf(gText, "%6.3f:1, %6.3f bits/byte, "
                       "%5.2f%% saved, %d in, %d out.\n",
                (float)bytesIn / (float)bytesOut,
                (8.0 * (float)bytesOut) / (float)bytesIn,
                100.0 * (1.0 - (float)bytesOut / (float)bytesIn),
                bytesIn,
                bytesOut);
        LogStr(gText);
    }

    if (veryVerbose) { sprintf(gText, "\n"); LogStr(gText); }
}

// @early-stop
// Relocation-masked instructions, the 0x30 frame, CFG, and all 56 effective targets/addends
// agree; the retained residual is seven delinked local-string identities.
VA(0x004d7710, 0x26e)
Bool uncompressStream(FILE *zStream, FILE *stream)
{
    Bool       thisIsTheLastBlock;
    BitStream  *zbs;
    Int32      magic1, magic2, magic3, magic4;
    UInt32     crcStored, crcComputed;
    Int32      currBlockNo;
    IntNative  retVal;

    zbs = (bsOpenReadStream(zStream));

    magic1 = (Int32)bsGetUChar(zbs);
    magic2 = (Int32)bsGetUChar(zbs);
    magic3 = (Int32)bsGetUChar(zbs);
    magic4 = (Int32)bsGetUChar(zbs);
    if (magic1 != 'B' ||
        magic2 != 'Z' ||
        magic3 != '0' ||
        magic4 < '1'  ||
        magic4 > '9') {
        bsClose(zbs);
        retVal = fclose(stream);
        ERROR_IF_EOF(retVal);
        FreeDecompressStructures();
        return False;
    }

    setDecompressStructureSizes(magic4 - '0');
    initialiseCRC();
    initBogusModel();
    arithCodeStartDecoding(zbs);

    if (veryVerbose) { sprintf(gText, "  "); LogStr(gText); }
    currBlockNo = 0;
    do {
        currBlockNo++;
        if (veryVerbose) { sprintf(gText, "[%d: ac+mtf ", currBlockNo); LogStr(gText); }
        thisIsTheLastBlock = getAndMoveToFrontDecode(zbs);
        if (veryVerbose) LogStr("rt ");
        undoReversibleTransformation();
        spotBlock(False);
        if (veryVerbose) LogStr("rld");
        unRLEandDump(stream, thisIsTheLastBlock);
        if (veryVerbose) LogStr("] ");
    }
        while (!thisIsTheLastBlock);

    if (veryVerbose) LogStr(" ");

    crcStored   = getUInt32(zbs);
    crcComputed = getFinalCRC();
    if (veryVerbose) { sprintf(gText, "CRCs: stored = 0x%x, computed = 0x%x\n  ", crcStored, crcComputed); LogStr(gText); }
    if (crcStored != crcComputed)
        crcError(crcStored, crcComputed);

    arithCodeDoneDecoding(zbs);
    bsClose(zbs);
    ERROR_IF_NOT_ZERO(ferror(stream));
    retVal = fclose(stream);
    ERROR_IF_EOF(retVal);
    FreeDecompressStructures();
    return True;
}

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
void crcError(UInt32 crcStored, UInt32 crcComputed)
{
    sprintf(gText,
            "\n%s: Data integrity error when decompressing.\n"
            "\tStored CRC = 0x%x, computed CRC = 0x%x\n"
            "\tThis could be a bug -- please report it to me at:\n"
            "\tsewardj@cs.man.ac.uk.\n",
            progName, crcStored, crcComputed);
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

VA(0x004d7a10, 0x4a)
void compressedStreamEOF(void)
{
    sprintf(gText,
            "\n%s: Compressed file ends unexpectedly;\n\t"
            "perhaps it is corrupted?  *Possible* reason follows.\n",
            progName);
    LogStr(gText);
    perror(progName);
    showFileNames();
    cleanUpAndFail();
}

VA(0x004d7a60, 0x4a)
void ioError(void)
{
    sprintf(gText,
            "\n%s: I/O or other error, bailing out.  Possible reason follows.\n",
            progName);
    LogStr(gText);
    perror(progName);
    showFileNames();
    cleanUpAndFail();
}

VA(0x004d7ab0, 0x3c)
void blockOverrun(void)
{
    sprintf(gText,
            "\n%s: block overrun during decompression,\n"
            "\twhich probably means the compressed file\n"
            "\tis corrupted.\n",
            progName);
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

VA(0x004d7af0, 0x3c)
void unblockError(void)
{
    sprintf(gText,
            "\n%s: compressed file didn't unblock correctly,\n"
            "\twhich probably means it is corrupted.\n",
            progName);
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

VA(0x004d7b30, 0x3c)
void bitStreamEOF(void)
{
    sprintf(gText,
            "\n%s: read past the end of compressed data,\n"
            "\twhich probably means it is corrupted.\n",
            progName);
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

VA(0x004d7b70, 0x37)
void __cdecl mySignalCatcher(IntNative *n)
{
    sprintf(gText,
            "\n%s: Control-C (or similar) caught, quitting.\n",
            progName);
    LogStr(gText);
    cleanUpAndFail();
}

VA(0x004d7bb0, 0x76)
void mySIGSEGVorSIGBUScatcher(IntNative *n)
{
    if (compressing) {
        sprintf(gText,
                "\n%s: Caught a SIGSEGV or SIGBUS whilst compressing,\n"
                "\twhich probably indicates a bug in BZIP.  Please\n"
                "\treport it to me at: sewardj@cs.man.ac.uk\n",
                progName);
        LogStr(gText);
    } else {
        sprintf(gText,
                "\n%s: Caught a SIGSEGV or SIGBUS whilst decompressing,\n"
                "\twhich probably indicates that the compressed data\n"
                "\tis corrupted.\n",
                progName);
        LogStr(gText);
    }

    showFileNames();
    cleanUpAndFail();
}

VA(0x004d7c30, 0x4d)
void uncompressOutOfMemory(Int32 draw, Int32 blockSize)
{
    sprintf(gText,
            "\n%s: Can't allocate enough memory for decompression.\n"
            "\tRequested %d bytes for a block size of %d.\n"
            "\tFind a machine with more memory, perhaps?\n",
            progName, draw, blockSize);
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

VA(0x004d7c80, 0x4d)
void compressOutOfMemory(Int32 draw, Int32 blockSize)
{
    sprintf(gText,
            "\n%s: Can't allocate enough memory for compression.\n"
            "\tRequested %d bytes for a block size of %d.\n"
            "\tReduce the block size, and/or use the -e flag.\n",
            progName, draw, blockSize);
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

// @early-stop
// The explicit 0x83-byte range is raw-byte exact and has no relocations.  Retail's
// next-public row contributes one alignment NOP at +0x83, outside this function range.
VA(0x004d7cd0, 0x83)
Bool endsInBz(Char *name)
{
    Int32 n = strlen(name);
    if (n <= 3) return False;
    return
        (name[n-3] == '.' &&
         name[n-2] == 'n' &&
         name[n-1] == 'w');
}

// @early-stop
// Relocation-masked instructions, the 0x14 frame, file CFG, and all 16 effective targets
// agree; the retained residual is the delinked .nw/rb/wb literal identities.
VA(0x004d7d60, 0xe2)
void compress(Char *name)
{
    FILE *inStr;
    FILE *outStr;
    IntNative retVal;

    strcpy(inName, name);
    strcpy(outName, name);
    strcat(outName, ".nw");

    inStr = fopen(inName, "rb");
    outStr = fopen(outName, "wb");

    errno = 0;
    outputHandleJustInCase = outStr;
    compressStream(inStr, outStr);
    outputHandleJustInCase = NULL;

    retVal = remove(inName);
}

// @early-stop
// Relocation-masked instructions, the 0x1c frame, file CFG, and all 19 effective targets
// agree; only local literal identities and the equivalent outName-3 alias remain.
VA(0x004d7e50, 0x110)
void uncompress(Char *name)
{
    FILE *inStr;
    FILE *outStr;
    Bool magicNumberOK;
    IntNative retVal;

    strcpy(inName, name);
    strcpy(outName, name);
    if (endsInBz(inName))
        outName[strlen(outName) - 3] = '\0';

    inStr = fopen(inName, "rb");
    outStr = fopen(outName, "wb");

    errno = 0;
    outputHandleJustInCase = outStr;
    magicNumberOK = uncompressStream(inStr, outStr);
    outputHandleJustInCase = NULL;

    retVal = remove(inName);
    ERROR_IF_NOT_ZERO(retVal);
}

// @early-stop
// Relocation-masked instructions, the 0x1f0 frame, file CFG, and all 28 effective targets
// agree. Residuals are local literals and _open/_write/_close import-name aliases that
// resolve to the same retail addresses with zero addends.
VA(0x004d7f60, 0x2d5)
long EncodeData(char *dst, char *src, unsigned long srcLen)
{
    char  fname[450] = { 0 };
    int   fd;
    int   retVal;
    FILE *fp;
    long  flen;

    outputHandleJustInCase = NULL;
    bsInUse = 0;
    errno = 0;
    blockSize100k = 3;
    LogStr("Encode 1");
    allocateCompressStructures();

    strcpy(fname, ".\\DATA\\");
    strcat(fname, "H2C");
    fname[strlen(fname)] = (char)Random(0x41, 0x5a);
    fname[strlen(fname)] = (char)Random(0x41, 0x5a);
    fname[strlen(fname)] = (char)Random(0x41, 0x5a);
    fname[strlen(fname)] = (char)Random(0x41, 0x5a);

    fd = _open(fname, 0x8301, 0x80);
    if (fd == -1) FileError(fname);
    _write(fd, src, srcLen);
    _close(fd);
    compress(fname);

    strcat(fname, ".nw");
    fp = fopen(fname, "rb");
    retVal = fseek(fp, 0, 2);
    flen = ftell(fp);
    retVal = fseek(fp, 0, 0);
    retVal = fread(dst, flen, 1, fp);
    retVal = fclose(fp);
    retVal = remove(fname);
    FreeCompressStructures();

    return flen;
}

// @early-stop
// Relocation-masked instructions, the 0x1f4 frame, file CFG, and all 26 effective targets
// agree. Residuals are local literals and _open/_write/_close import-name aliases that
// resolve to the same retail addresses with zero addends.
VA(0x004d8240, 0x2f3)
long DecodeData(char *dst, char *src, unsigned long srcLen)
{
    char  fname[450] = { 0 };
    int   fd;
    int   retVal;
    FILE *fp;
    long  flen;

    outputHandleJustInCase = NULL;
    bsInUse = 0;
    errno = 0;
    blockSize100k = 0;
    allocateCompressStructures();

    strcpy(fname, ".\\DATA\\");
    strcat(fname, "H2C");
    fname[strlen(fname)] = (char)Random(0x41, 0x5a);
    fname[strlen(fname)] = (char)Random(0x41, 0x5a);
    fname[strlen(fname)] = (char)Random(0x41, 0x5a);
    fname[strlen(fname)] = (char)Random(0x41, 0x5a);
    strcat(fname, ".nw");

    fd = _open(fname, 0x8301, 0x80);
    if (fd == -1) FileError(fname);
    _write(fd, src, srcLen);
    _close(fd);
    uncompress(fname);

    fname[strlen(fname) - 3] = '\0';
    fp = fopen(fname, "rb");
    retVal = fseek(fp, 0, 2);
    flen = ftell(fp);
    retVal = fseek(fp, 0, 0);
    retVal = fread(dst, flen, 1, fp);
    retVal = fclose(fp);
    retVal = remove(fname);
    FreeCompressStructures();

    return flen;
}

// @data-layout-note
// Retail `.rdata` and candidate ordinal 5 are byte-exact at 0xeba88+0x18.
// Retail initialized storage is 0x11ff50+0xd70; candidate ordinal 2 is a
// byte-exact 0xd6f-byte prefix followed by one retail zero padding byte. Its
// six DATA definitions and 77 private allocations therefore have exact
// owner RVAs, extents, payloads, and section offsets.
//
// Retail loader-zero storage is 0x134ed0+0x31b0, while the hash-ordered
// candidate COMMON contribution is 0x2da8. All 26 real definitions have
// exact public/source anchors. The retail-only gaps are 0x134eec+0x4,
// 0x13742c+0x4, 0x137c5c+0x3fc, and the terminal 0x13807c+0x4. The only
// HIGHLOW target inside a gap is uncompress+0x87: candidate and retail both
// encode outName-3, yielding 0x13742d. It is not a separate allocation. The
// unreferenced 0x3fc interval remains unresolved; do not add padding, aliases,
// guessed buffers, or section pragmas for any of these gaps.
//
// Ordered Bzip-owned relocation targets are exact in 67 of 69 referencing
// functions (414 references). getMTFVal has 1 candidate versus 8 retail model
// references and sortIt has 23 versus 35; those residuals belong to their
// incomplete function structure, not to data ownership or layout.
// ---- globals (definitions, RVA order) ----
DATA(0x00534ee4) int longestFileName;
DATA(0x00534ee8) int opMode;
DATA(0x00537028) char inName[1024];
DATA(0x00537430) char outName[1024];
DATA(0x00537c58) char *progNameReally;
DATA(0x00538078) int keepInputFiles;
