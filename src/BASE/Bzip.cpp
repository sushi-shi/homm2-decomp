#include <Ints.h>
#include <BASE/Bzip.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <PLATFORM/File.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#pragma intrinsic(strcpy, strcat, strlen, memcpy)

#define True 1
#define False 0
#define ERROR_IF_EOF(i)                                                                            \
    {                                                                                              \
        if ((i) == EOF)                                                                            \
            ioError();                                                                             \
    }
#define ERROR_IF_NOT_ZERO(i)                                                                       \
    {                                                                                              \
        if ((i) != 0)                                                                              \
            ioError();                                                                             \
    }
#define ERROR_IF_MINUS_ONE(i)                                                                      \
    {                                                                                              \
        if ((i) == (-1))                                                                           \
            ioError();                                                                             \
    }

#define TWO_TO_THE(n) (1 << (n))
#define MAX_BITS_OUTSTANDING 500000000
#define smallB 26
#define smallF 18

u32 globalCrc;
i32 bsInUse;
BitStream aBitStreamBuffer;
i32 bytesOut;
UInt32 bigL;
UInt32 bigR;
UInt32 bigD;
UInt32 bitsOutstanding;
Model bogusModel;

#define BASIS 0
#define MODEL_2_3 1
#define MODEL_4_7 2
#define MODEL_8_15 3
#define MODEL_16_31 4
#define MODEL_32_63 5
#define MODEL_64_127 6
#define MODEL_128_255 7
Model models[8];

#define VAL_RUNA 1
#define VAL_RUNB 2
#define VAL_ONE 3
#define VAL_2_3 4
#define VAL_4_7 5
#define VAL_8_15 6
#define VAL_16_31 7
#define VAL_32_63 8
#define VAL_64_127 9
#define VAL_128_255 10
#define VAL_EOB 11
#define RUNA 257
#define RUNB 258
#define EOB 259
#define INVALID 260

#define NUM_FULLGT_UNROLLINGS 4
#define MAX_DENORM_OFFSET (4 * NUM_FULLGT_UNROLLINGS)
UInt32 crc32Table[256] = {
    0x00000000U, 0x04c11db7U, 0x09823b6eU, 0x0d4326d9U, 0x130476dcU, 0x17c56b6bU, 0x1a864db2U,
    0x1e475005U, 0x2608edb8U, 0x22c9f00fU, 0x2f8ad6d6U, 0x2b4bcb61U, 0x350c9b64U, 0x31cd86d3U,
    0x3c8ea00aU, 0x384fbdbdU, 0x4c11db70U, 0x48d0c6c7U, 0x4593e01eU, 0x4152fda9U, 0x5f15adacU,
    0x5bd4b01bU, 0x569796c2U, 0x52568b75U, 0x6a1936c8U, 0x6ed82b7fU, 0x639b0da6U, 0x675a1011U,
    0x791d4014U, 0x7ddc5da3U, 0x709f7b7aU, 0x745e66cdU, 0x9823b6e0U, 0x9ce2ab57U, 0x91a18d8eU,
    0x95609039U, 0x8b27c03cU, 0x8fe6dd8bU, 0x82a5fb52U, 0x8664e6e5U, 0xbe2b5b58U, 0xbaea46efU,
    0xb7a96036U, 0xb3687d81U, 0xad2f2d84U, 0xa9ee3033U, 0xa4ad16eaU, 0xa06c0b5dU, 0xd4326d90U,
    0xd0f37027U, 0xddb056feU, 0xd9714b49U, 0xc7361b4cU, 0xc3f706fbU, 0xceb42022U, 0xca753d95U,
    0xf23a8028U, 0xf6fb9d9fU, 0xfbb8bb46U, 0xff79a6f1U, 0xe13ef6f4U, 0xe5ffeb43U, 0xe8bccd9aU,
    0xec7dd02dU, 0x34867077U, 0x30476dc0U, 0x3d044b19U, 0x39c556aeU, 0x278206abU, 0x23431b1cU,
    0x2e003dc5U, 0x2ac12072U, 0x128e9dcfU, 0x164f8078U, 0x1b0ca6a1U, 0x1fcdbb16U, 0x018aeb13U,
    0x054bf6a4U, 0x0808d07dU, 0x0cc9cdcaU, 0x7897ab07U, 0x7c56b6b0U, 0x71159069U, 0x75d48ddeU,
    0x6b93dddbU, 0x6f52c06cU, 0x6211e6b5U, 0x66d0fb02U, 0x5e9f46bfU, 0x5a5e5b08U, 0x571d7dd1U,
    0x53dc6066U, 0x4d9b3063U, 0x495a2dd4U, 0x44190b0dU, 0x40d816baU, 0xaca5c697U, 0xa864db20U,
    0xa527fdf9U, 0xa1e6e04eU, 0xbfa1b04bU, 0xbb60adfcU, 0xb6238b25U, 0xb2e29692U, 0x8aad2b2fU,
    0x8e6c3698U, 0x832f1041U, 0x87ee0df6U, 0x99a95df3U, 0x9d684044U, 0x902b669dU, 0x94ea7b2aU,
    0xe0b41de7U, 0xe4750050U, 0xe9362689U, 0xedf73b3eU, 0xf3b06b3bU, 0xf771768cU, 0xfa325055U,
    0xfef34de2U, 0xc6bcf05fU, 0xc27dede8U, 0xcf3ecb31U, 0xcbffd686U, 0xd5b88683U, 0xd1799b34U,
    0xdc3abdedU, 0xd8fba05aU, 0x690ce0eeU, 0x6dcdfd59U, 0x608edb80U, 0x644fc637U, 0x7a089632U,
    0x7ec98b85U, 0x738aad5cU, 0x774bb0ebU, 0x4f040d56U, 0x4bc510e1U, 0x46863638U, 0x42472b8fU,
    0x5c007b8aU, 0x58c1663dU, 0x558240e4U, 0x51435d53U, 0x251d3b9eU, 0x21dc2629U, 0x2c9f00f0U,
    0x285e1d47U, 0x36194d42U, 0x32d850f5U, 0x3f9b762cU, 0x3b5a6b9bU, 0x0315d626U, 0x07d4cb91U,
    0x0a97ed48U, 0x0e56f0ffU, 0x1011a0faU, 0x14d0bd4dU, 0x19939b94U, 0x1d528623U, 0xf12f560eU,
    0xf5ee4bb9U, 0xf8ad6d60U, 0xfc6c70d7U, 0xe22b20d2U, 0xe6ea3d65U, 0xeba91bbcU, 0xef68060bU,
    0xd727bbb6U, 0xd3e6a601U, 0xdea580d8U, 0xda649d6fU, 0xc423cd6aU, 0xc0e2d0ddU, 0xcda1f604U,
    0xc960ebb3U, 0xbd3e8d7eU, 0xb9ff90c9U, 0xb4bcb610U, 0xb07daba7U, 0xae3afba2U, 0xaafbe615U,
    0xa7b8c0ccU, 0xa379dd7bU, 0x9b3660c6U, 0x9ff77d71U, 0x92b45ba8U, 0x9675461fU, 0x8832161aU,
    0x8cf30badU, 0x81b02d74U, 0x857130c3U, 0x5d8a9099U, 0x594b8d2eU, 0x5408abf7U, 0x50c9b640U,
    0x4e8ee645U, 0x4a4ffbf2U, 0x470cdd2bU, 0x43cdc09cU, 0x7b827d21U, 0x7f436096U, 0x7200464fU,
    0x76c15bf8U, 0x68860bfdU, 0x6c47164aU, 0x61043093U, 0x65c52d24U, 0x119b4be9U, 0x155a565eU,
    0x18197087U, 0x1cd86d30U, 0x029f3d35U, 0x065e2082U, 0x0b1d065bU, 0x0fdc1becU, 0x3793a651U,
    0x3352bbe6U, 0x3e119d3fU, 0x3ad08088U, 0x2497d08dU, 0x2056cd3aU, 0x2d15ebe3U, 0x29d4f654U,
    0xc5a92679U, 0xc1683bceU, 0xcc2b1d17U, 0xc8ea00a0U, 0xd6ad50a5U, 0xd26c4d12U, 0xdf2f6bcbU,
    0xdbee767cU, 0xe3a1cbc1U, 0xe760d676U, 0xea23f0afU, 0xeee2ed18U, 0xf0a5bd1dU, 0xf464a0aaU,
    0xf9278673U, 0xfde69bc4U, 0x89b8fd09U, 0x8d79e0beU, 0x803ac667U, 0x84fbdbd0U, 0x9abc8bd5U,
    0x9e7d9662U, 0x933eb0bbU, 0x97ffad0cU, 0xafb010b1U, 0xab710d06U, 0xa6322bdfU, 0xa2f33668U,
    0xbcb4666dU, 0xb8757bdaU, 0xb5365d03U, 0xb1f740b4U
};
UInt32* words = NULL;
Int32* zptr = NULL;
Int32* ftab = NULL;
UChar* block = NULL;
UChar* ll = NULL;
Int32 last;
Int32 lastPP;
Int32 origPtr;
Int32 blockSize100k;
Int32 veryVerbose;
Char* progName;
Int32 compressing;
Int32 bytesIn;
Int32 verbose;
FILE* outputHandleJustInCase;

#define MY_EOF 257
#define UPDATE_CRC(crcVar, cha)                                                                    \
    {                                                                                              \
        crcVar = (crcVar << 8) ^ crc32Table[(crcVar >> 24) ^ ((UChar)cha)];                        \
    }

#define IF_THEN_ELSE(c, t, e) ((c) ? (t) : (e))
#define GETFIRST(a) ((UChar)(words[a] >> 24))
#define GETREST(a) (words[a] & 0x00ffffff)
#define SETALL(a, w) words[a] = (w)
#define GETFIRST16(a) ((UInt32)(words[a] >> 16))
#define GETREST16(a) (words[a] & 0x0000ffff)


void initialiseCRC(void) {
    globalCrc = 0xffffffff;
}

u32 getFinalCRC(void) {
    return ~globalCrc;
}

u32 getGlobalCRC(void) {
    return globalCrc;
}

void setGlobalCRC(u32 newCrc) {
    globalCrc = newCrc;
}

BitStream* bsOpenReadStream(FILE* f) {
    BitStream* bs;
    if (bsInUse)
        panic(const_cast<char*>("bsOpenReadStream"));
    bsInUse = 1;
    bs = &aBitStreamBuffer;
    bs->handle = f;
    bs->buffer = 0;
    bs->buffLive = 0;
    bs->mode = 'r';
    return bs;
}

BitStream* bsOpenWriteStream(FILE* f) {
    BitStream* bs;
    if (bsInUse)
        panic(const_cast<char*>("bsOpenWriteStream"));
    bsInUse = 1;
    bs = &aBitStreamBuffer;
    bs->handle = f;
    bs->buffer = 0;
    bs->buffLive = 0;
    bs->mode = 'w';
    return bs;
}

void bsPutBit(BitStream* bs, i32 bit) {
    i32 retVal;
    if (bs->buffLive == 8) {
        retVal = putc((u8)bs->buffer, bs->handle);
        if (retVal == EOF)
            ioError();
        bytesOut++;
        bs->buffLive = 1;
        bs->buffer = bit & 1;
    } else {
        bs->buffer = (bs->buffer << 1) | (bit & 1);
        bs->buffLive++;
    }
}

i32 bsGetBit(BitStream* bs) {
    i32 retVal;
    if (bs->buffLive > 0) {
        bs->buffLive--;
        return (bs->buffer >> bs->buffLive) & 1;
    } else {
        retVal = getc(bs->handle);
        if (retVal == EOF)
            compressedStreamEOF();
        bs->buffLive = 7;
        bs->buffer = retVal;
        if (bs->buffer == -1)
            bitStreamEOF();
        return (bs->buffer >> 7) & 1;
    }
}

u8 bsGetUChar(BitStream* bs) {
    Int32 i;
    UInt32 c;

    c = 0;
    for (i = 0; i <= 7; i++)
        c = (c << 1) | bsGetBit(bs);

    return (UChar)c;
}

void bsPutUChar(BitStream* bs, u8 c) {
    i32 i;
    for (i = 7; i >= 0; i--)
        bsPutBit(bs, ((u32)c >> i) & 1);
}

void bsClose(BitStream* bs) {
    IntNative retVal;
    if (!bsInUse)
        panic(const_cast<char*>("bsClose"));
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

u32 minUInt32(u32 a, u32 b) {
    if (a < b)
        return a;
    else
        return b;
}

void arithCodeBitPlusFollow(BitStream* bs, UInt32 bit) {
    bsPutBit(bs, bit);
    while (bitsOutstanding > 0) {
        bsPutBit(bs, 1 - bit);
        bitsOutstanding--;
    }
}

void arithCodeStartEncoding(BitStream* bs) {
    bigL = 0;
    bigR = TWO_TO_THE(smallB - 1);
    bitsOutstanding = 0;
}

void arithCodeDoneEncoding(BitStream* bs) {
    Int32 i;

    for (i = smallB; i >= 1; i--)
        arithCodeBitPlusFollow(bs, (bigL >> (i - 1)) & 0x1);
}

void arithCodeStartDecoding(BitStream* bs) {
    Int32 i;

    bigL = 0;
    bigR = TWO_TO_THE(smallB - 1);
    bigD = 0;
    for (i = 1; i <= smallB; i++)
        bigD = (bigD << 1) + bsGetBit(bs);
}

void arithCodeDoneDecoding(BitStream* bs) {

}

void arithCodeRenormalise_Encode(BitStream* bs) {
    while (bigR <= TWO_TO_THE(smallB - 2)) {
        if ((bigR + bigL) <= TWO_TO_THE(smallB - 1)) {
            arithCodeBitPlusFollow(bs, 0);
        } else if (TWO_TO_THE(smallB - 1) <= bigL) {
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

void arithCodeSymbol(BitStream* bs, Model* m, Int32 symbol) {
    UInt32 cumulativeLow8, cumulativeHigh2, totalFrequency14, rangeWidth29, rangeProduct8;
    Int32 symbolIndex9;

    totalFrequency14 = m->totFreq;
    cumulativeLow8 = 0;
    for (symbolIndex9 = 1; symbolIndex9 < symbol; symbolIndex9++)
        cumulativeLow8 += m->freq[symbolIndex9];
    cumulativeHigh2 = cumulativeLow8 + m->freq[symbol];

    rangeWidth29 = bigR / totalFrequency14;

    rangeProduct8 = cumulativeLow8 * rangeWidth29;
    bigL = bigL + rangeProduct8;

    if (cumulativeHigh2 < totalFrequency14)
        bigR = rangeWidth29 * (cumulativeHigh2 - cumulativeLow8);
    else
        bigR = bigR - rangeProduct8;

    arithCodeRenormalise_Encode(bs);

    if (bitsOutstanding > MAX_BITS_OUTSTANDING)
        panic(const_cast<char*>("arithCodeSymbol: too many bits outstanding"));
}

Int32 arithDecodeSymbol(BitStream* bs, Model* m) {
    UInt32 cumulativeLow7, scaledTarget1, cumulativeHigh1, totalFrequency13, rangeWidth28,
        rangeProduct26, decodedSymbol5;

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
        bigR = rangeWidth28 * (cumulativeHigh1 - cumulativeLow7);
    else
        bigR = bigR - rangeProduct26;

    while (bigR <= TWO_TO_THE(smallB - 2)) {
        bigR = 2 * bigR;
        bigD = 2 * bigD + bsGetBit(bs);
    }

    return (Int32)decodedSymbol5;
}

void initModel(
    Model* m,
    Char* initName,
    Int32 initNumSymbols,
    Int32 initIncValue,
    Int32 initNoExceed
) {
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

    m->numSymbols = initNumSymbols;
    m->incValue = initIncValue;
    m->noExceed = initNoExceed;
    m->name = initName;
    m->freq[0] = 0;
    m->freq[initNumSymbols + 1] = 0;
    m->numScalings = 0;
}

void dumpModelStats(Model* m) {
    sprintf(gText, "model %s:\t scalings %d\n", m->name, m->numScalings);
    LogStr(gText);
}

void updateModel(Model* m, Int32 symbol) {
    UInt32 i;

    m->totFreq += m->incValue;
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

void putSymbol(Model* m, Int32 symbol, BitStream* bs) {
    arithCodeSymbol(bs, m, symbol);
    updateModel(m, symbol);
}

Int32 getSymbol(Model* m, BitStream* bs) {
    Int32 symbol;

    symbol = arithDecodeSymbol(bs, m);
    updateModel(m, symbol);

    return symbol;
}

void initBogusModel(void) {
    initModel(&bogusModel, const_cast<char*>("bogus"), 256, 0, 256);
}

void putUChar(BitStream* bs, UChar c) {
    putSymbol(&bogusModel, 1 + (UInt32)c, bs);
}

void putInt32(BitStream* bs, Int32 i) {
    putUChar(bs, (UChar)(((UInt32)i >> 24) & 0xFF));
    putUChar(bs, (UChar)(((UInt32)i >> 16) & 0xFF));
    putUChar(bs, (UChar)(((UInt32)i >> 8) & 0xFF));
    putUChar(bs, (UChar)((UInt32)i & 0xFF));
}

void putUInt32(BitStream* bs, UInt32 i) {
    putUChar(bs, (UChar)((i >> 24) & 0xFF));
    putUChar(bs, (UChar)((i >> 16) & 0xFF));
    putUChar(bs, (UChar)((i >> 8) & 0xFF));
    putUChar(bs, (UChar)(i & 0xFF));
}

UChar getUChar(BitStream* bs) {
    return (UChar)(getSymbol(&bogusModel, bs) - 1);
}

Int32 getInt32(BitStream* bs) {
    UInt32 res = 0;

    res |= (getUChar(bs) << 24);
    res |= (getUChar(bs) << 16);
    res |= (getUChar(bs) << 8);
    res |= (getUChar(bs));
    return (Int32)res;
}

UInt32 getUInt32(BitStream* bs) {
    UInt32 res = 0;

    res |= (getUChar(bs) << 24);
    res |= (getUChar(bs) << 16);
    res |= (getUChar(bs) << 8);
    res |= (getUChar(bs));
    return res;
}

void initModels(void) {
    initModel(&models[BASIS], const_cast<char*>("basis"), 11, 12, 1000);
    initModel(&models[MODEL_2_3], const_cast<char*>("2-3"), 2, 4, 1000);
    initModel(&models[MODEL_4_7], const_cast<char*>("4-7"), 4, 3, 1000);
    initModel(&models[MODEL_8_15], const_cast<char*>("8-15"), 8, 3, 1000);
    initModel(&models[MODEL_16_31], const_cast<char*>("16-31"), 16, 3, 1000);
    initModel(&models[MODEL_32_63], const_cast<char*>("32-63"), 32, 3, 1000);
    initModel(&models[MODEL_64_127], const_cast<char*>("64-127"), 64, 2, 1000);
    initModel(&models[MODEL_128_255], const_cast<char*>("128-255"), 128, 1, 1000);
}

void dumpAllModelStats(void) {
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

Int32 getMTFVal(BitStream* bs) {
    Int32 retVal;

    switch (getSymbol(&models[BASIS], bs)) {
        case VAL_EOB:
            retVal = EOB;
            break;
        case VAL_RUNA:
            retVal = RUNA;
            break;
        case VAL_RUNB:
            retVal = RUNB;
            break;
        case VAL_ONE:
            retVal = 1;
            break;
        case VAL_2_3:
            retVal = getSymbol(&models[MODEL_2_3], bs) + 2 - 1;
            break;
        case VAL_4_7:
            retVal = getSymbol(&models[MODEL_4_7], bs) + 4 - 1;
            break;
        case VAL_8_15:
            retVal = getSymbol(&models[MODEL_8_15], bs) + 8 - 1;
            break;
        case VAL_16_31:
            retVal = getSymbol(&models[MODEL_16_31], bs) + 16 - 1;
            break;
        case VAL_32_63:
            retVal = getSymbol(&models[MODEL_32_63], bs) + 32 - 1;
            break;
        case VAL_64_127:
            retVal = getSymbol(&models[MODEL_64_127], bs) + 64 - 1;
            break;
        default:
            retVal = getSymbol(&models[MODEL_128_255], bs) + 128 - 1;
            break;
    }
    return retVal;
}

void sendMTFVal(BitStream* bs, Int32 n) {
    if (n == RUNA)
        putSymbol(&models[BASIS], VAL_RUNA, bs);
    else if (n == RUNB)
        putSymbol(&models[BASIS], VAL_RUNB, bs);
    else if (n == EOB)
        putSymbol(&models[BASIS], VAL_EOB, bs);
    else

        if (n == 1)
        putSymbol(&models[BASIS], VAL_ONE, bs);
    else

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

        panic(const_cast<char*>("sendMTFVal: bad value!"));
    }
}

void FreeCompressStructures(void) {
    if (words != NULL)
        H2_FREE(words);
    if (ftab != NULL)
        H2_FREE(ftab);
    if (zptr != NULL)
        H2_FREE(zptr);
    words = NULL;
    ftab = NULL;
    zptr = NULL;
}

void allocateCompressStructures(void) {
    Int32 n = 100000 * blockSize100k;
    FreeCompressStructures();
    words = (UInt32*)H2_ALLOC((n + MAX_DENORM_OFFSET) * sizeof(Int32) + 1);
    zptr = (Int32*)H2_ALLOC(n * sizeof(Int32) + 1);
    ftab = (Int32*)H2_ALLOC(65537 * sizeof(Int32) + 1);

    if (words == NULL || zptr == NULL || ftab == NULL) {
        Int32 totalDraw =
            (n + MAX_DENORM_OFFSET) * sizeof(Int32) + n * sizeof(Int32) + 65537 * sizeof(Int32);

        compressOutOfMemory(totalDraw, n);
    }
}

void FreeDecompressStructures(void) {
    if (block != NULL)
        H2_FREE(block);
    if (ll != NULL)
        H2_FREE(ll);
    if (zptr != NULL)
        H2_FREE(zptr);
    block = NULL;
    ll = NULL;
    zptr = NULL;
}

void setDecompressStructureSizes(Int32 newSize100k) {
    if (newSize100k == blockSize100k)
        return;

    blockSize100k = newSize100k;
    FreeDecompressStructures();

    if (newSize100k != 0) {
        Int32 n = 100000 * newSize100k;
        block = (UChar*)H2_ALLOC(n * sizeof(UChar) + 1);
        ll = (UChar*)H2_ALLOC(n * sizeof(UChar) + 1);
        zptr = (Int32*)H2_ALLOC(n * sizeof(Int32) + 1);

        if (block == NULL || ll == NULL || zptr == NULL) {
            Int32 totalDraw = 6 * n * sizeof(UChar);
            uncompressOutOfMemory(totalDraw, n);
        }
    }
}

UInt32 GETALL(Int32 a) {
    return words[a];
}

void SETREST16(Int32 a, UInt32 w) {
    words[a] = (words[a] & 0xffff0000) | (((UInt32)(w)) & 0x0000ffff);
}

void SETFIRST16(Int32 a, UInt32 w) {
    words[a] = (words[a] & 0x0000ffff) | (((UInt32)(w)) << 16);
}

void SETREST(Int32 a, UInt32 w) {
    words[a] = (words[a] & 0xff000000) | (((UInt32)(w)) & 0x00ffffff);
}

void SETFIRST(Int32 a, UChar c) {
    words[a] = (words[a] & 0x00ffffff) | (((UInt32)(c)) << 24);
}

void SETSECOND(Int32 a, UChar c) {
    words[a] = (words[a] & 0xff00ffff) | (((UInt32)(c)) << 16);
}

void SETTHIRD(Int32 a, UChar c) {
    words[a] = (words[a] & 0xffff00ff) | (((UInt32)(c)) << 8);
}

void SETFOURTH(Int32 a, UChar c) {
    words[a] = (words[a] & 0xffffff00) | (((UInt32)(c)));
}

Int32 NORMALISE(Int32 p) {
    return IF_THEN_ELSE(
        ((p) < 0),
        (lastPP + (p)),
        IF_THEN_ELSE(((lastPP > (p)) == False), ((p)-lastPP), (p))
    );
}

Int32 NORMALISEHI(Int32 p) {
    return IF_THEN_ELSE(((p) >= lastPP), ((p)-lastPP), (p));
}

Int32 NORMALISELO(Int32 p) {
    return IF_THEN_ELSE(((p) < 0), ((p) + lastPP), (p));
}

Int32 STRONG_NORMALISE(Int32 p) {
    while (p < 0) {
        p += lastPP;
    }
    return p % lastPP;
}

void sendZeroes(BitStream* outStream, Int32 zeroesPending) {
    UInt32 bitsToSend;
    Int32 numBits;

    if (zeroesPending == 0)
        return;

    bitsToSend = 0;
    numBits = 0;
    while (zeroesPending != 0) {
        numBits++;
        bitsToSend <<= 1;
        zeroesPending--;
        if ((zeroesPending & 0x1) == 1)
            bitsToSend |= 1;
        zeroesPending >>= 1;
    }
    while (numBits > 0) {
        if ((bitsToSend & 0x1) == 1)
            sendMTFVal(outStream, RUNA);
        else
            sendMTFVal(outStream, RUNB);
        bitsToSend >>= 1;
        numBits--;
    }
}

void moveToFrontCodeAndSend(BitStream* outStream, Bool thisIsTheLastBlock) {
    UChar yy0[256];
    Int32 i9, j6;
    UChar tmp8;
    UChar tmp2a6;
    Int32 zeroesPending9;

    zeroesPending9 = 0;
    if (thisIsTheLastBlock)
        putInt32(outStream, -(origPtr + 1));
    else
        putInt32(outStream, (origPtr + 1));

    initModels();

    for (i9 = 0; i9 <= 255; i9++)
        yy0[i9] = (UChar)i9;

    for (i9 = 0; i9 <= last; i9++) {
        UChar ll_i;

        ll_i = GETFIRST(NORMALISELO(zptr[i9] - 1));

        j6 = 0;
        tmp8 = yy0[j6];
        while (ll_i != tmp8) {
            j6++;
            tmp2a6 = tmp8;
            tmp8 = yy0[j6];
            yy0[j6] = tmp2a6;
        }
        yy0[0] = tmp8;

        if (j6 == 0) {
            zeroesPending9++;
        } else {
            sendZeroes(outStream, zeroesPending9);
            zeroesPending9 = 0;
            sendMTFVal(outStream, j6);
        }
    }
    sendZeroes(outStream, zeroesPending9);
    sendMTFVal(outStream, EOB);
}

Bool getAndMoveToFrontDecode(BitStream* inStream) {
    UChar symbols[256];
    Int32 i, j, encodedOrigin, nextSym, blockLimit;

    blockLimit = 100000 * blockSize100k;

    encodedOrigin = getInt32(inStream);
    if (encodedOrigin < 0)
        origPtr = (-encodedOrigin) - 1;
    else
        origPtr = encodedOrigin - 1;

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
            if (nextSym == RUNA)
                n |= 1;
            n++;
            nextSym = getMTFVal(inStream);
        } while (nextSym == RUNA || nextSym == RUNB);
        while (n > 0) {
            last++;
            if (last >= blockLimit)
                blockOverrun();
            ll[last] = symbols[0];
            n--;
        }
        goto LOOPSTART;
    }

    if (nextSym >= 1 && nextSym <= 255) {
        last++;
        if (last >= blockLimit)
            blockOverrun();
        ll[last] = symbols[nextSym];

        j = nextSym;
        for (; j > 3; j -= 4) {
            symbols[j] = symbols[j - 1];
            symbols[j - 1] = symbols[j - 2];
            symbols[j - 2] = symbols[j - 3];
            symbols[j - 3] = symbols[j - 4];
        }
        for (; j > 0; j--)
            symbols[j] = symbols[j - 1];

        symbols[0] = ll[last];
        nextSym = getMTFVal(inStream);
        goto LOOPSTART;
    }

    sprintf(gText, "bad MTF value %d\n", nextSym);
    LogStr(gText);
    panic(const_cast<char*>("getAndMoveToFrontDecode\n"));
    return True;
}

void stripe(void) {
    Int32 i;

    for (i = 0; i < lastPP; i++) {
        UChar c = GETFIRST(i);
        SETSECOND(NORMALISELO(i - 1), c);
        SETTHIRD(NORMALISELO(i - 2), c);
        SETFOURTH(NORMALISELO(i - 3), c);
    }
}

void copyOffsetWords(void) {
    Int32 i;

    for (i = 0; i < 4 * NUM_FULLGT_UNROLLINGS; i++)
        words[lastPP + i] = words[i];
}

Bool fullGt(Int32 i1, Int32 i2) {
    Int32 i1orig = i1;

    if (i1 == i2)
        return False;

    do {
        UInt32 w1;
        UInt32 w2;

        w1 = GETALL(i1);
        w2 = GETALL(i2);
        if (w1 != w2)
            return (w1 > w2);
        i1 += 4;
        i2 += 4;

        w1 = GETALL(i1);
        w2 = GETALL(i2);
        if (w1 != w2)
            return (w1 > w2);
        i1 += 4;
        i2 += 4;

        w1 = GETALL(i1);
        w2 = GETALL(i2);
        if (w1 != w2)
            return (w1 > w2);
        i1 += 4;
        i2 += 4;

        w1 = GETALL(i1);
        w2 = GETALL(i2);
        if (w1 != w2)
            return (w1 > w2);
        i1 += 4;
        i2 += 4;

        i1 = NORMALISEHI(i1);
        i2 = NORMALISEHI(i2);

    } while (i1 != i1orig);
    return False;
}

#define ISORT_BELOW 10
#define RC(x) (x)
#define SWAP(za, zb)                                                                               \
    {                                                                                              \
        Int32 zl0 = (za);                                                                          \
        Int32 zr8 = (zb);                                                                          \
        Int32 zt = zptr[RC(zl0)];                                                                  \
        zptr[RC(zl0)] = zptr[RC(zr8)];                                                             \
        zptr[RC(zr8)] = zt;                                                                        \
    }

void qsortFull(Int32 left, Int32 right) {
    Int32 pivot3, v3;
    Int32 i, j12;
    Int32 wuC0;

    Int32 stackL[40];
    Int32 stackR[40];
    Int32 sp13 = 0;

    Int32 wuL = left;
    Int32 wuR1 = right;

    while (True) {

        if (wuR1 - wuL > ISORT_BELOW) {

            wuC0 = (wuL + wuR1) >> 1;
            if (fullGt(zptr[RC(wuL)], zptr[RC(wuC0)]))
                SWAP(wuL, wuC0);
            if (fullGt(zptr[RC(wuL)], zptr[RC(wuR1)]))
                SWAP(wuL, wuR1);
            if (fullGt(zptr[RC(wuC0)], zptr[RC(wuR1)]))
                SWAP(wuC0, wuR1);

            SWAP(wuC0, wuR1 - 1);
            pivot3 = zptr[RC(wuR1 - 1)];

            i = wuL;
            j12 = wuR1 - 1;
            for (;;) {
                do
                    i++;
                while (fullGt(pivot3, zptr[RC(i)]));
                do
                    j12--;
                while (fullGt(zptr[RC(j12)], pivot3));
                if (i < j12)
                    SWAP(i, j12) else break;
            }
            SWAP(i, wuR1 - 1);

            if ((i - wuL) > (wuR1 - i)) {
                stackL[sp13] = wuL;
                stackR[sp13] = i - 1;
                sp13++;
                wuL = i + 1;
            } else {
                stackL[sp13] = i + 1;
                stackR[sp13] = wuR1;
                sp13++;
                wuR1 = i - 1;
            }

        } else {

            for (i = wuL + 1; i <= wuR1; i++) {
                v3 = zptr[RC(i)];
                j12 = i;
                while (fullGt(zptr[RC(j12 - 1)], v3)) {
                    zptr[RC(j12)] = zptr[RC(j12 - 1)];
                    j12 = j12 - 1;
                    if (j12 <= wuL)
                        goto zero;
                }
            zero:
                zptr[RC(j12)] = v3;
            }
            if (sp13 == 0)
                return;
            sp13--;
            wuL = stackL[sp13];
            wuR1 = stackR[sp13];
        }
    }
}

#undef RC
#undef SWAP
#undef ISORT_BELOW

Bool trivialGt(Int32 i1, Int32 i2) {
    Int32 k;

    for (k = 0; k <= last; k++) {
        UChar c1 = GETFIRST(i1);
        UChar c2 = GETFIRST(i2);
        if (c1 == c2) {
            i1++;
            i1 = NORMALISEHI(i1);
            i2++;
            i2 = NORMALISEHI(i2);
        } else if (c1 > c2)
            return True;
        else
            return False;
    }
    return False;
}

void shellTrivial(void) {
    Int32 i, j, h, bigN2;
    Int32 v;

    Int32 ptrLo5 = 0;
    Int32 ptrHi7 = last;
    bigN2 = ptrHi7 - ptrLo5 + 1;
    h = 1;
    do {
        h = 3 * h + 1;
    } while (!(h > bigN2));
    do {
        h = h / 3;
        for (i = ptrLo5 + h; i <= ptrHi7; i++) {
            v = zptr[i];
            j = i;
            while (trivialGt(zptr[j - h], v)) {
                zptr[j] = zptr[j - h];
                j = j - h;
                if (j <= (ptrLo5 + h - 1))
                    goto zero;
            }
        zero:
            zptr[j] = v;
        }
    } while (h != 1);
}

void sortIt(void) {
    lastPP = last + 1;

    if (lastPP <= 1024) {

        Int32 i;

        if (veryVerbose) {
            sprintf(gText, "trivialSort ...\n");
            LogStr(gText);
        }
        for (i = 0; i <= last; i++)
            zptr[i] = i;
        shellTrivial();
        if (veryVerbose) {
            sprintf(gText, "trivialSort done.\n");
            LogStr(gText);
        }

    } else {
        Int32 i;
        Int32 grade;
        Int32 notDone0;

        stripe();

        if (veryVerbose) {
            sprintf(gText, "bucket sorting ...\n");
            LogStr(gText);
        }

        for (i = 0; i <= 65536; i++)
            ftab[i] = 0;
        for (i = 0; i <= last; i++)
            ftab[GETFIRST16(i)]++;
        for (i = 1; i <= 65536; i++)
            ftab[i] += ftab[i - 1];

        for (i = 0; i <= last; i++) {
            UInt32 j = GETFIRST16(i);
            ftab[j]--;
            zptr[ftab[j]] = i;
        }

        copyOffsetWords();

        notDone0 = lastPP;
        for (grade = 1; grade <= 5; grade++) {
            Int32 candNo0;
            Int32 loBound;
            Int32 hiBound0;

            switch (grade) {
                case 1:
                    loBound = 2;
                    hiBound0 = 15;
                    break;
                case 2:
                    loBound = 16;
                    hiBound0 = 255;
                    break;
                case 3:
                    loBound = 256;
                    hiBound0 = 4095;
                    break;
                case 4:
                    loBound = 4096;
                    hiBound0 = 65535;
                    break;
                case 5:
                    loBound = 65536;
                    hiBound0 = 900000;
                    break;
                default:
                    panic(const_cast<char*>("gradedSort"));
                    break;
            }
            if (loBound > lastPP)
                continue;

            candNo0 = 0;
            for (i = 0; i <= 65535; i++) {

                Int32 freqHere = ftab[i + 1] - ftab[i];

                if (freqHere >= loBound && hiBound0 >= freqHere) {
                    Int32 j, k;
                    Int32 lower = ftab[i];
                    Int32 upper5 = ftab[i + 1] - 1;

                    candNo0++;
                    notDone0 -= freqHere;

                    if (veryVerbose) {
                        sprintf(
                            gText,
                            "   %d -> %d:  cand %5d,   freq = %6d,   notdone = %6d",
                            loBound,
                            hiBound0,
                            candNo0,
                            freqHere,
                            notDone0
                        );
                        LogStr(gText);
                    }

                    qsortFull(lower, upper5);

                    if (freqHere < 65535) {
                        for (j = lower, k = 0; j <= upper5; j++, k++) {
                            Int32 a2update = zptr[j];
                            SETREST16(a2update, k);
                            if (a2update < (4 * NUM_FULLGT_UNROLLINGS))
                                SETREST16(a2update + lastPP, k);
                        }
                    }
                    if (veryVerbose) {
                        sprintf(gText, " ");
                        LogStr(gText);
                    }
                }
            }
        }
    }
}

void doReversibleTransformation(void) {
    Int32 i;

    if (veryVerbose) {
        sprintf(gText, " ");
        LogStr(gText);
    }

    sortIt();

    origPtr = -1;
    for (i = 0; i <= last; i++)
        if (zptr[i] == 0) {
            origPtr = i;
            break;
        }

    if (origPtr == -1)
        panic(const_cast<char*>("doReversibleTransformation"));
}

void undoReversibleTransformation(void) {
    Int32 frequencyByChar[256];
    Int32 i, j, currentChar, total;

    for (i = 0; i <= 255; i++)
        frequencyByChar[i] = 0;

    for (i = 0; i <= last; i++) {
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

void spotBlock(Bool weAreCompressing) {
    Int32 spotPos, delta, updatedDelta;

    spotPos = SPOT_BASIS_STEP;
    delta = 1;

    while (spotPos < last) {

        Int32 n;

        if (weAreCompressing)
            n = (Int32)GETFIRST(spotPos) + 1;
        else
            n = (Int32)block[spotPos] - 1;

        if (n == 256)
            n = 0;
        else if (n == -1)
            n = 255;

        if (!(n >= 0 && n <= 255))
            panic(const_cast<char*>("spotBlock"));

        if (weAreCompressing)
            SETFIRST(spotPos, (UChar)n);
        else
            block[spotPos] = (UChar)n;

        switch (delta) {
            case 3:
                updatedDelta = 1;
                break;
            case 1:
                updatedDelta = 4;
                break;
            case 4:
                updatedDelta = 5;
                break;
            case 5:
                updatedDelta = 9;
                break;
            case 9:
                updatedDelta = 2;
                break;
            case 2:
                updatedDelta = 6;
                break;
            case 6:
                updatedDelta = 7;
                break;
            case 8:
                updatedDelta = 8;
                break;
            case 7:
                updatedDelta = 3;
                break;
            default:
                updatedDelta = 1;
                break;
        }
        delta = updatedDelta;

        spotPos = spotPos + SPOT_BASIS_STEP + 17 * (updatedDelta - 5);
    }
}

Int32 getRLEpair(FILE* src) {
    Int32 runLen;
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
    } while (ch == latestCh && runLen < 255);

    if (latestCh != EOF) {
        if (ungetc(latestCh, src) == EOF)
            panic(const_cast<char*>("getRLEpair: ungetc failed"));
    } else {
        ERROR_IF_NOT_ZERO(errno);
    }

    if (runLen == 1) {
        UPDATE_CRC(globalCrc, (UChar)ch);
        return (1 << 16) | ch;
    } else {
        Int32 i;
        for (i = 1; i <= runLen; i++)
            UPDATE_CRC(globalCrc, (UChar)ch);
        return (runLen << 16) | ch;
    }
}

Bool loadAndRLEsource(FILE* src) {
    Int32 currentChar, allowableBlockSize;

    last = -1;
    currentChar = 0;

    allowableBlockSize = 100000 * blockSize100k - 20;

    while (last < allowableBlockSize && currentChar != MY_EOF) {
        Int32 rlePair, runLength;
        rlePair = getRLEpair(src);
        currentChar = rlePair & 0xFFFF;
        runLength = (UInt32)rlePair >> 16;

        if (currentChar == MY_EOF) {
            last++;
            SETFIRST(last, ((UChar)42));
        } else
            switch (runLength) {
                case 1:
                    last++;
                    SETFIRST(last, ((UChar)currentChar));
                    break;
                case 2:
                    last++;
                    SETFIRST(last, ((UChar)currentChar));
                    last++;
                    SETFIRST(last, ((UChar)currentChar));
                    break;
                case 3:
                    last++;
                    SETFIRST(last, ((UChar)currentChar));
                    last++;
                    SETFIRST(last, ((UChar)currentChar));
                    last++;
                    SETFIRST(last, ((UChar)currentChar));
                    break;
                default:
                    last++;
                    SETFIRST(last, ((UChar)currentChar));
                    last++;
                    SETFIRST(last, ((UChar)currentChar));
                    last++;
                    SETFIRST(last, ((UChar)currentChar));
                    last++;
                    SETFIRST(last, ((UChar)currentChar));
                    last++;
                    SETFIRST(last, ((UChar)(runLength - 4)));
                    break;
            }
    }
    return (currentChar == MY_EOF);
}

void unRLEandDump(FILE* dst, Bool thisIsTheLastBlock) {
    IntNative retVal6;
    Int32 lastCharToSpew0, i01, count19, chPrev, ch04;
    UInt32 localCrc02;

    if (thisIsTheLastBlock)
        lastCharToSpew0 = last - 1;
    else
        lastCharToSpew0 = last;

    count19 = 0;
    i01 = 0;
    ch04 = 256;
    localCrc02 = getGlobalCRC();

    while (i01 <= lastCharToSpew0) {
        chPrev = ch04;
        ch04 = block[i01];
        i01++;

        retVal6 = putc(ch04, dst);
        ERROR_IF_EOF(retVal6);
        UPDATE_CRC(localCrc02, (UChar)ch04);

        if (ch04 != chPrev) {
            count19 = 1;
        } else {
            count19++;
            if (count19 >= 4) {
                Int32 j;
                for (j = 0; j < (Int32)block[i01]; j++) {
                    retVal6 = putc(ch04, dst);
                    ERROR_IF_EOF(retVal6);
                    UPDATE_CRC(localCrc02, (UChar)ch04);
                }
                i01++;
                count19 = 0;
            }
        }
    }

    setGlobalCRC(localCrc02);

    if (thisIsTheLastBlock && block[last] != 42)
        unblockError();
}

void compressStream(FILE* stream, FILE* zStream) {
    IntNative retVal;
    Bool thisIsTheLastBlock9;
    BitStream* zbs2;
    UInt32 crcToSend;
    Int32 blockNo = 1;

    bytesIn = 0;
    bytesOut = 0;

    zbs2 = bsOpenWriteStream(zStream);

    bsPutUChar(zbs2, 'B');
    bsPutUChar(zbs2, 'Z');
    bsPutUChar(zbs2, '0');
    bsPutUChar(zbs2, '0' + blockSize100k);

    initialiseCRC();
    initBogusModel();
    arithCodeStartEncoding(zbs2);

    do {
        if (veryVerbose) {
            sprintf(gText, "\nBEGIN block %d\n", blockNo);
            LogStr(gText);
        }
        blockNo++;
        thisIsTheLastBlock9 = loadAndRLEsource(stream);
        spotBlock(True);
        doReversibleTransformation();
        moveToFrontCodeAndSend(zbs2, thisIsTheLastBlock9);
    } while (!thisIsTheLastBlock9);

    crcToSend = getFinalCRC();
    putUInt32(zbs2, crcToSend);
    if (veryVerbose) {
        sprintf(gText, "\nCRC = 0x%x\n", crcToSend);
        LogStr(gText);
    }

    arithCodeDoneEncoding(zbs2);
    bsClose(zbs2);
    ERROR_IF_NOT_ZERO(ferror(stream));
    retVal = fclose(stream);
    ERROR_IF_EOF(retVal);

    if (veryVerbose) {
        sprintf(gText, " ");
        LogStr(gText);
        dumpAllModelStats();
        sprintf(gText, " ");
        LogStr(gText);
    }

    if (bytesIn == 0)
        bytesIn = 1;
    if (bytesOut == 0)
        bytesOut = 1;

    if (verbose) {
        sprintf(
            gText,
            "%6.3f:1, %6.3f bits/byte, "
            "%5.2f%% saved, %d in, %d out.\n",
            (float)bytesIn / (float)bytesOut,
            (8.0 * (float)bytesOut) / (float)bytesIn,
            100.0 * (1.0 - (float)bytesOut / (float)bytesIn),
            bytesIn,
            bytesOut
        );
        LogStr(gText);
    }

    if (veryVerbose) {
        sprintf(gText, "\n");
        LogStr(gText);
    }
}

Bool uncompressStream(FILE* zStream, FILE* stream) {
    Bool thisIsTheLastBlock2;
    BitStream* zbs02;
    Int32 magic1h, magic2g, magic3b, magic4a;
    UInt32 crcStored, crcComputed6;
    Int32 currBlockNo9;
    IntNative retVal5;

    zbs02 = (bsOpenReadStream(zStream));

    magic1h = (Int32)bsGetUChar(zbs02);
    magic2g = (Int32)bsGetUChar(zbs02);
    magic3b = (Int32)bsGetUChar(zbs02);
    magic4a = (Int32)bsGetUChar(zbs02);
    if (magic1h != 'B' || magic2g != 'Z' || magic3b != '0' || magic4a < '1' || magic4a > '9') {
        bsClose(zbs02);
        retVal5 = fclose(stream);
        ERROR_IF_EOF(retVal5);
        FreeDecompressStructures();
        return False;
    }

    setDecompressStructureSizes(magic4a - '0');
    initialiseCRC();
    initBogusModel();
    arithCodeStartDecoding(zbs02);

    if (veryVerbose) {
        sprintf(gText, "  ");
        LogStr(gText);
    }
    currBlockNo9 = 0;
    do {
        currBlockNo9++;
        if (veryVerbose) {
            sprintf(gText, "[%d: ac+mtf ", currBlockNo9);
            LogStr(gText);
        }
        thisIsTheLastBlock2 = getAndMoveToFrontDecode(zbs02);
        if (veryVerbose)
            LogStr("rt ");
        undoReversibleTransformation();
        spotBlock(False);
        if (veryVerbose)
            LogStr("rld");
        unRLEandDump(stream, thisIsTheLastBlock2);
        if (veryVerbose)
            LogStr("] ");
    } while (!thisIsTheLastBlock2);

    if (veryVerbose)
        LogStr(" ");

    crcStored = getUInt32(zbs02);
    crcComputed6 = getFinalCRC();
    if (veryVerbose) {
        sprintf(gText, "CRCs: stored = 0x%x, computed = 0x%x\n  ", crcStored, crcComputed6);
        LogStr(gText);
    }
    if (crcStored != crcComputed6)
        crcError(crcStored, crcComputed6);

    arithCodeDoneDecoding(zbs02);
    bsClose(zbs02);
    ERROR_IF_NOT_ZERO(ferror(stream));
    retVal5 = fclose(stream);
    ERROR_IF_EOF(retVal5);
    FreeDecompressStructures();
    return True;
}

void showFileNames(void) {}

void cleanUpAndFail(void) {}

void panic(char* s) {}

void crcError(UInt32 crcStored, UInt32 crcComputed) {
    sprintf(
        gText,
        "\n%s: Data integrity error when decompressing.\n"
        "\tStored CRC = 0x%x, computed CRC = 0x%x\n"
        "\tThis could be a bug -- please report it to me at:\n"
        "\tsewardj@cs.man.ac.uk.\n",
        progName,
        crcStored,
        crcComputed
    );
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

void compressedStreamEOF(void) {
    sprintf(
        gText,
        "\n%s: Compressed file ends unexpectedly;\n\t"
        "perhaps it is corrupted?  *Possible* reason follows.\n",
        progName
    );
    LogStr(gText);
    perror(progName);
    showFileNames();
    cleanUpAndFail();
}

void ioError(void) {
    sprintf(gText, "\n%s: I/O or other error, bailing out.  Possible reason follows.\n", progName);
    LogStr(gText);
    perror(progName);
    showFileNames();
    cleanUpAndFail();
}

void blockOverrun(void) {
    sprintf(
        gText,
        "\n%s: block overrun during decompression,\n"
        "\twhich probably means the compressed file\n"
        "\tis corrupted.\n",
        progName
    );
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

void unblockError(void) {
    sprintf(
        gText,
        "\n%s: compressed file didn't unblock correctly,\n"
        "\twhich probably means it is corrupted.\n",
        progName
    );
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

void bitStreamEOF(void) {
    sprintf(
        gText,
        "\n%s: read past the end of compressed data,\n"
        "\twhich probably means it is corrupted.\n",
        progName
    );
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

void __cdecl mySignalCatcher(IntNative* n) {
    sprintf(gText, "\n%s: Control-C (or similar) caught, quitting.\n", progName);
    LogStr(gText);
    cleanUpAndFail();
}

void mySIGSEGVorSIGBUScatcher(IntNative* n) {
    if (compressing) {
        sprintf(
            gText,
            "\n%s: Caught a SIGSEGV or SIGBUS whilst compressing,\n"
            "\twhich probably indicates a bug in BZIP.  Please\n"
            "\treport it to me at: sewardj@cs.man.ac.uk\n",
            progName
        );
        LogStr(gText);
    } else {
        sprintf(
            gText,
            "\n%s: Caught a SIGSEGV or SIGBUS whilst decompressing,\n"
            "\twhich probably indicates that the compressed data\n"
            "\tis corrupted.\n",
            progName
        );
        LogStr(gText);
    }

    showFileNames();
    cleanUpAndFail();
}

void uncompressOutOfMemory(Int32 draw, Int32 blockSize) {
    sprintf(
        gText,
        "\n%s: Can't allocate enough memory for decompression.\n"
        "\tRequested %d bytes for a block size of %d.\n"
        "\tFind a machine with more memory, perhaps?\n",
        progName,
        draw,
        blockSize
    );
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

void compressOutOfMemory(Int32 draw, Int32 blockSize) {
    sprintf(
        gText,
        "\n%s: Can't allocate enough memory for compression.\n"
        "\tRequested %d bytes for a block size of %d.\n"
        "\tReduce the block size, and/or use the -e flag.\n",
        progName,
        draw,
        blockSize
    );
    LogStr(gText);
    showFileNames();
    cleanUpAndFail();
}

Bool endsInBz(Char* name) {
    Int32 n = strlen(name);
    if (n <= 3)
        return False;
    return (name[n - 3] == '.' && name[n - 2] == 'n' && name[n - 1] == 'w');
}

void compress(Char* name) {
    FILE* inStr;
    FILE* outStr;
    IntNative retVal3;

    strcpy(inName, name);
    strcpy(outName, name);
    strcat(outName, ".nw");

    inStr = fopen(inName, "rb");
    outStr = fopen(outName, "wb");

    errno = 0;
    outputHandleJustInCase = outStr;
    compressStream(inStr, outStr);
    outputHandleJustInCase = NULL;

    retVal3 = remove(inName);
}

void uncompress(Char* name) {
    FILE* inStr;
    FILE* outStr2;
    Bool magicNumberOK;
    IntNative retVal3;

    strcpy(inName, name);
    strcpy(outName, name);
    if (endsInBz(inName))
        outName[strlen(outName) - 3] = '\0';

    inStr = fopen(inName, "rb");
    outStr2 = fopen(outName, "wb");

    errno = 0;
    outputHandleJustInCase = outStr2;
    magicNumberOK = uncompressStream(inStr, outStr2);
    outputHandleJustInCase = NULL;

    retVal3 = remove(inName);
    ERROR_IF_NOT_ZERO(retVal3);
}

i32l EncodeData(char* dst, char* src, u32l srcLen) {
    char fname[450] = {0};
    i32 fd;
    i32 retVal;
    FILE* fp9;
    i32l flen3;

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

    fd = platform::FileOpen(fname, platform::FileMode::Write);
    if (fd == -1)
        FileError(fname);
    platform::FileWrite(fd, src, srcLen);
    platform::FileClose(fd);

    // The compressor works the file with the C runtime, so hand it the real
    // path rather than the retail one.
    platform::FileResolve(fname, platform::FileMode::Write, fname, sizeof(fname));
    compress(fname);

    strcat(fname, ".nw");
    fp9 = fopen(fname, "rb");
    retVal = fseek(fp9, 0, 2);
    flen3 = ftell(fp9);
    retVal = fseek(fp9, 0, 0);
    retVal = fread(dst, flen3, 1, fp9);
    retVal = fclose(fp9);
    retVal = remove(fname);
    FreeCompressStructures();

    return flen3;
}

i32l DecodeData(char* dst, char* src, u32l srcLen) {
    char fname[450] = {0};
    i32 fd;
    i32 retVal;
    FILE* fp9;
    i32l flen3;

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

    fd = platform::FileOpen(fname, platform::FileMode::Write);
    if (fd == -1)
        FileError(fname);
    platform::FileWrite(fd, src, srcLen);
    platform::FileClose(fd);

    // As in EncodeData: the decompressor uses the C runtime directly.
    platform::FileResolve(fname, platform::FileMode::Write, fname, sizeof(fname));
    uncompress(fname);

    fname[strlen(fname) - 3] = '\0';
    fp9 = fopen(fname, "rb");
    retVal = fseek(fp9, 0, 2);
    flen3 = ftell(fp9);
    retVal = fseek(fp9, 0, 0);
    retVal = fread(dst, flen3, 1, fp9);
    retVal = fclose(fp9);
    retVal = remove(fname);
    FreeCompressStructures();

    return flen3;
}

i32 longestFileName;
i32 opMode;
char inName[1024];
char outName[1024];
char progNameReally[0x400];
i32 keepInputFiles;
