#include <va.h>
#include <BASE/Bzip.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


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

DATA(0x0053958c) u32 globalCrc;
DATA(0x00539588) i32 bsInUse;
DATA(0x00539598) BitStream aBitStreamBuffer;
DATA(0x0053683c) i32 bytesOut;
DATA(0x00536c4c) UInt32 bigL;
DATA(0x00536c50) UInt32 bigR;
DATA(0x00536c48) UInt32 bigD;
DATA(0x0053640c) UInt32 bitsOutstanding;
DATA(0x00536410) Model bogusModel;

#define BASIS 0
#define MODEL_2_3 1
#define MODEL_4_7 2
#define MODEL_8_15 3
#define MODEL_16_31 4
#define MODEL_32_63 5
#define MODEL_64_127 6
#define MODEL_128_255 7
DATA(0x00537460) Model models[8];

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
DATA(0x0051f5a4) UInt32 crc32Table[256] = {
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
DATA(0x005395a8) UInt32* words = NULL;
DATA(0x005395ac) Int32* zptr = NULL;
DATA(0x005395b0) Int32* ftab = NULL;
DATA(0x005395b4) UChar* block = NULL;
DATA(0x005395b8) UChar* ll = NULL;
DATA(0x00536404) Int32 last;
DATA(0x00536400) Int32 lastPP;
DATA(0x00539590) Int32 origPtr;
DATA(0x00539594) Int32 blockSize100k;
DATA(0x00539580) Int32 veryVerbose;
DATA(0x00537454) Char* progName;
DATA(0x00536834) Int32 compressing;
DATA(0x00536408) Int32 bytesIn;
DATA(0x00537458) Int32 verbose;
DATA(0x00536840) FILE* outputHandleJustInCase;

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


VA(0x004c6ea0, 0xf)
void initialiseCRC(void) {
    globalCrc = 0xffffffff;
}

VA(0x004c6eb0, 0xc)
u32 getFinalCRC(void) {
    return ~globalCrc;
}

VA(0x004c6ec0, 0xa)
u32 getGlobalCRC(void) {
    return globalCrc;
}

VA(0x004c6ed0, 0x13)
void setGlobalCRC(u32 newCrc) {
    globalCrc = newCrc;
}

VA(0x004c6ef0, 0x57)
BitStream* bsOpenReadStream(FILE* f) {
    BitStream* bs;
    if (bsInUse)
        panic(const_cast<char*>(
            "bsOpenReadStream"));
    bsInUse = 1;
    bs = &aBitStreamBuffer;
    bs->handle = f;
    bs->buffer = 0;
    bs->buffLive = 0;
    bs->mode = 'r';
    return bs;
}

VA(0x004c6f50, 0x57)
BitStream* bsOpenWriteStream(FILE* f) {
    BitStream* bs;
    if (bsInUse)
        panic(const_cast<char*>(
            "bsOpenWriteStream"));
    bsInUse = 1;
    bs = &aBitStreamBuffer;
    bs->handle = f;
    bs->buffer = 0;
    bs->buffLive = 0;
    bs->mode = 'w';
    return bs;
}

VA(0x004c6fb0, 0x8a)
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

VA(0x004c7040, 0x81)
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

VA(0x004c70d0, 0x45)
u8 bsGetUChar(BitStream* bs) {
    Int32 i;
    UInt32 c;

    c = 0;
    for (i = 0; i <= 7; i++)
        c = (c << 1) | bsGetBit(bs);

    return (UChar)c;
}

VA(0x004c7120, 0x43)
void bsPutUChar(BitStream* bs, u8 c) {
    i32 i;
    for (i = 7; i >= 0; i--)
        bsPutBit(bs, ((u32)c >> i) & 1);
}

VA(0x004c7170, 0xde)
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

VA(0x004c7250, 0x20)
u32 minUInt32(u32 a, u32 b) {
    if (a < b)
        return a;
    else
        return b;
}

VA(0x004c7270, 0x43)
void arithCodeBitPlusFollow(BitStream* bs, UInt32 bit) {
    bsPutBit(bs, bit);
    while (bitsOutstanding > 0) {
        bsPutBit(bs, 1 - bit);
        bitsOutstanding--;
    }
}

VA(0x004c72c0, 0x29)
void arithCodeStartEncoding(BitStream* H2_UNUSED(bs)) {
    bigL = 0;
    bigR = TWO_TO_THE(smallB - 1);
    bitsOutstanding = 0;
}

VA(0x004c72f0, 0x40)
void arithCodeDoneEncoding(BitStream* bs) {
    Int32 i;

    for (i = smallB; i >= 1; i--)
        arithCodeBitPlusFollow(bs, (bigL >> (i - 1)) & 0x1);
}

VA(0x004c7330, 0x5c)
void arithCodeStartDecoding(BitStream* bs) {
    Int32 i;

    bigL = 0;
    bigR = TWO_TO_THE(smallB - 1);
    bigD = 0;
    for (i = 1; i <= smallB; i++)
        bigD = (bigD << 1) + bsGetBit(bs);
}

VA(0x004c7390, 0xb)
void arithCodeDoneDecoding(BitStream* H2_UNUSED(bs)) {

}

VA(0x004c73a0, 0xa5)
void arithCodeRenormalise_Encode(BitStream* bs) {
    while (bigR <= TWO_TO_THE(smallB - 2)) {
        if ((bigL + bigR) <= TWO_TO_THE(smallB - 1)) {
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

VA(0x004c7450, 0xcb)
void arithCodeSymbol(BitStream* bs, Model* m, Int32 symbol) {
    UInt32 smallL, smallH, smallT, smallR, smallR_x_smallL;
    Int32 i;

    smallT = m->totFreq;
    smallL = 0;
    for (i = 1; i < symbol; i++)
        smallL += m->freq[i];
    smallH = smallL + m->freq[symbol];

    smallR = bigR / smallT;

    smallR_x_smallL = smallR * smallL;
    bigL = bigL + smallR_x_smallL;

    if (smallH < smallT)
        bigR = smallR * (smallH - smallL);
    else
        bigR = bigR - smallR_x_smallL;

    arithCodeRenormalise_Encode(bs);

    if (bitsOutstanding > MAX_BITS_OUTSTANDING)
        panic(const_cast<char*>("arithCodeSymbol: too many bits outstanding"));
}

VA(0x004c7520, 0xf8)
Int32 arithDecodeSymbol(BitStream* bs, Model* m) {
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
        bigR = smallR * (smallH - smallL);
    else
        bigR = bigR - smallR_x_smallL;

    while (bigR <= TWO_TO_THE(smallB - 2)) {
        bigR = 2 * bigR;
        bigD = 2 * bigD + bsGetBit(bs);
    }

    return (Int32)symbol;
}

VA(0x004c7620, 0xc8)
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

VA(0x004c76f0, 0x34)
void dumpModelStats(Model* m) {
    sprintf(
        gText,
        "model %s:\t scalings %d\n",
        m->name,
        m->numScalings
    );
    LogStr(gText);
}

VA(0x004c7730, 0xaf)
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

VA(0x004c77e0, 0x2c)
void putSymbol(Model* m, Int32 symbol, BitStream* bs) {
    arithCodeSymbol(bs, m, symbol);
    updateModel(m, symbol);
}

VA(0x004c7810, 0x2c)
Int32 getSymbol(Model* m, BitStream* bs) {
    Int32 symbol;

    symbol = arithDecodeSymbol(bs, m);
    updateModel(m, symbol);

    return symbol;
}

VA(0x004c7840, 0x20)
void initBogusModel(void) {
    initModel(
        &bogusModel,
        const_cast<char*>("bogus"),
        256,
        0,
        256
    );
}

VA(0x004c7860, 0x2a)
void putUChar(BitStream* bs, UChar c) {
    putSymbol(&bogusModel, 1 + (UInt32)c, bs);
}

VA(0x004c7890, 0x5d)
void putInt32(BitStream* bs, Int32 i) {
    putUChar(bs, (UChar)(((UInt32)i >> 24) & 0xFF));
    putUChar(bs, (UChar)(((UInt32)i >> 16) & 0xFF));
    putUChar(bs, (UChar)(((UInt32)i >> 8) & 0xFF));
    putUChar(bs, (UChar)((UInt32)i & 0xFF));
}

VA(0x004c78f0, 0x5d)
void putUInt32(BitStream* bs, UInt32 i) {
    putUChar(bs, (UChar)((i >> 24) & 0xFF));
    putUChar(bs, (UChar)((i >> 16) & 0xFF));
    putUChar(bs, (UChar)((i >> 8) & 0xFF));
    putUChar(bs, (UChar)(i & 0xFF));
}

VA(0x004c7950, 0x1b)
UChar getUChar(BitStream* bs) {
    return (UChar)(getSymbol(&bogusModel, bs) - 1);
}

VA(0x004c7970, 0x74)
Int32 getInt32(BitStream* bs) {
    UInt32 res = 0;

    res |= (getUChar(bs) << 24);
    res |= (getUChar(bs) << 16);
    res |= (getUChar(bs) << 8);
    res |= (getUChar(bs));
    return (Int32)res;
}

VA(0x004c79f0, 0x74)
UInt32 getUInt32(BitStream* bs) {
    UInt32 res = 0;

    res |= (getUChar(bs) << 24);
    res |= (getUChar(bs) << 16);
    res |= (getUChar(bs) << 8);
    res |= (getUChar(bs));
    return res;
}

VA(0x004c7a70, 0xc8)
void initModels(void) {
    initModel(
        &models[BASIS],
        const_cast<char*>("basis"),
        11,
        12,
        1000
    );
    initModel(
        &models[MODEL_2_3],
        const_cast<char*>("2-3"),
        2,
        4,
        1000
    );
    initModel(
        &models[MODEL_4_7],
        const_cast<char*>("4-7"),
        4,
        3,
        1000
    );
    initModel(
        &models[MODEL_8_15],
        const_cast<char*>("8-15"),
        8,
        3,
        1000
    );
    initModel(
        &models[MODEL_16_31],
        const_cast<char*>("16-31"),
        16,
        3,
        1000
    );
    initModel(
        &models[MODEL_32_63],
        const_cast<char*>("32-63"),
        32,
        3,
        1000
    );
    initModel(
        &models[MODEL_64_127],
        const_cast<char*>("64-127"),
        64,
        2,
        1000
    );
    initModel(
        &models[MODEL_128_255],
        const_cast<char*>("128-255"),
        128,
        1,
        1000
    );
}

VA(0x004c7b40, 0x5f)
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

VA(0x004c7ba0, 0x12a)
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

VA(0x004c7cd0, 0x220)
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

VA(0x004c7ef0, 0x81)
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

VA(0x004c7f80, 0xab)
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

VA(0x004c8030, 0x81)
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

VA(0x004c80c0, 0xc3)
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

VA(0x004c8190, 0x17)
UInt32 GETALL(Int32 a) {
    return words[a];
}

VA(0x004c81b0, 0x37)
void SETREST16(Int32 a, UInt32 w) {
    words[a] = (words[a] & 0xffff0000) | (((UInt32)(w)) & 0x0000ffff);
}

VA(0x004c81f0, 0x35)
void SETFIRST16(Int32 a, UInt32 w) {
    words[a] = (words[a] & 0x0000ffff) | (((UInt32)(w)) << 16);
}

VA(0x004c8230, 0x37)
void SETREST(Int32 a, UInt32 w) {
    words[a] = (words[a] & 0xff000000) | (((UInt32)(w)) & 0x00ffffff);
}

VA(0x004c8270, 0x3a)
void SETFIRST(Int32 a, UChar c) {
    words[a] = (words[a] & 0x00ffffff) | (((UInt32)(c)) << 24);
}

VA(0x004c82b0, 0x3a)
void SETSECOND(Int32 a, UChar c) {
    words[a] = (words[a] & 0xff00ffff) | (((UInt32)(c)) << 16);
}

VA(0x004c82f0, 0x37)
void SETTHIRD(Int32 a, UChar c) {
    words[a] = (words[a] & 0xffff00ff) | (((UInt32)(c)) << 8);
}

VA(0x004c8330, 0x34)
void SETFOURTH(Int32 a, UChar c) {
    words[a] = (words[a] & 0xffffff00) | (((UInt32)(c)));
}

VA(0x004c8370, 0x49)
Int32 NORMALISE(Int32 p) {
    return IF_THEN_ELSE(
        ((p) < 0),
        ((p) + lastPP),
        IF_THEN_ELSE(((p) >= lastPP), ((p)-lastPP), (p))
    );
}

VA(0x004c83c0, 0x2f)
Int32 NORMALISEHI(Int32 p) {
    return IF_THEN_ELSE(((p) >= lastPP), ((p)-lastPP), (p));
}

VA(0x004c83f0, 0x2a)
Int32 NORMALISELO(Int32 p) {
    return IF_THEN_ELSE(((p) < 0), ((p) + lastPP), (p));
}

VA(0x004c8420, 0x2b)
Int32 STRONG_NORMALISE(Int32 p) {
    while (p < 0) {
        p += lastPP;
    }
    return p % lastPP;
}

VA(0x004c8450, 0xa7)
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

VA(0x004c8500, 0x1c0)
void moveToFrontCodeAndSend(BitStream* outStream, Bool thisIsTheLastBlock) {
    UChar yy[256];
    Int32 i, j;
    UChar tmp;
    UChar tmp2;
    Int32 zeroesPending;

    zeroesPending = 0;
    if (thisIsTheLastBlock)
        putInt32(outStream, -(origPtr + 1));
    else
        putInt32(outStream, (origPtr + 1));

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

VA(0x004c86c0, 0x331)
Bool getAndMoveToFrontDecode(BitStream* inStream) {
    UChar yy[256];
    Int32 i, j, tmpOrigPtr, nextSym, limit;

    limit = 100000 * blockSize100k;

    tmpOrigPtr = getInt32(inStream);
    if (tmpOrigPtr < 0)
        origPtr = (-tmpOrigPtr) - 1;
    else
        origPtr = tmpOrigPtr - 1;

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
            if (nextSym == RUNA)
                n |= 1;
            n++;
            nextSym = getMTFVal(inStream);
        } while (nextSym == RUNA || nextSym == RUNB);
        while (n > 0) {
            last++;
            if (last >= limit)
                blockOverrun();
            ll[last] = yy[0];
            n--;
        }
        goto LOOPSTART;
    }

    if (nextSym >= 1 && nextSym <= 255) {
        last++;
        if (last >= limit)
            blockOverrun();
        ll[last] = yy[nextSym];

        j = nextSym;
        for (; j > 3; j -= 4) {
            yy[j] = yy[j - 1];
            yy[j - 1] = yy[j - 2];
            yy[j - 2] = yy[j - 3];
            yy[j - 3] = yy[j - 4];
        }
        for (; j > 0; j--)
            yy[j] = yy[j - 1];

        yy[0] = ll[last];
        nextSym = getMTFVal(inStream);
        goto LOOPSTART;
    }

    sprintf(
        gText,
        "bad MTF value %d\n",
        nextSym
    );
    LogStr(gText);
    panic(const_cast<char*>("getAndMoveToFrontDecode\n"));
    return True;
}

VA(0x004c8a00, 0x81)
void stripe(void) {
    Int32 i;

    for (i = 0; i < lastPP; i++) {
        UChar c = GETFIRST(i);
        SETSECOND(NORMALISELO(i - 1), c);
        SETTHIRD(NORMALISELO(i - 2), c);
        SETFOURTH(NORMALISELO(i - 3), c);
    }
}

VA(0x004c8a90, 0x42)
void copyOffsetWords(void) {
    Int32 i;

    for (i = 0; i < 4 * NUM_FULLGT_UNROLLINGS; i++)
        words[lastPP + i] = words[i];
}

VA(0x004c8ae0, 0x13f)
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
        Int32 zl = (za);                                                                           \
        Int32 zr = (zb);                                                                           \
        Int32 zt = zptr[RC(zl)];                                                                   \
        zptr[RC(zl)] = zptr[RC(zr)];                                                               \
        zptr[RC(zr)] = zt;                                                                         \
    }

VA(0x004c8c20, 0x53c)
void qsortFull(Int32 left, Int32 right) {
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
            if (fullGt(zptr[RC(wuL)], zptr[RC(wuC)]))
                SWAP(wuL, wuC);
            if (fullGt(zptr[RC(wuL)], zptr[RC(wuR)]))
                SWAP(wuL, wuR);
            if (fullGt(zptr[RC(wuC)], zptr[RC(wuR)]))
                SWAP(wuC, wuR);

            SWAP(wuC, wuR - 1);
            pivot = zptr[RC(wuR - 1)];

            i = wuL;
            j = wuR - 1;
            for (;;) {
                do
                    i++;
                while (fullGt(pivot, zptr[RC(i)]));
                do
                    j--;
                while (fullGt(zptr[RC(j)], pivot));
                if (i < j)
                    SWAP(i, j) else break;
            }
            SWAP(i, wuR - 1);

            if ((i - wuL) > (wuR - i)) {
                stackL[sp] = wuL;
                stackR[sp] = i - 1;
                sp++;
                wuL = i + 1;
            } else {
                stackL[sp] = i + 1;
                stackR[sp] = wuR;
                sp++;
                wuR = i - 1;
            }

        } else {

            for (i = wuL + 1; i <= wuR; i++) {
                v = zptr[RC(i)];
                j = i;
                while (fullGt(zptr[RC(j - 1)], v)) {
                    zptr[RC(j)] = zptr[RC(j - 1)];
                    j = j - 1;
                    if (j <= wuL)
                        goto zero;
                }
            zero:
                zptr[RC(j)] = v;
            }
            if (sp == 0)
                return;
            sp--;
            wuL = stackL[sp];
            wuR = stackR[sp];
        }
    }
}

#undef RC
#undef SWAP
#undef ISORT_BELOW

VA(0x004c9160, 0xb9)
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

VA(0x004c9220, 0xf4)
void shellTrivial(void) {
    Int32 i, j, h, bigN;
    Int32 v;

    Int32 ptrLo = 0;
    Int32 ptrHi = last;
    bigN = ptrHi - ptrLo + 1;
    h = 1;
    do {
        h = 3 * h + 1;
    } while (!(h > bigN));
    do {
        h = h / 3;
        for (i = ptrLo + h; i <= ptrHi; i++) {
            v = zptr[i];
            j = i;
            while (trivialGt(zptr[j - h], v)) {
                zptr[j] = zptr[j - h];
                j = j - h;
                if (j <= (ptrLo + h - 1))
                    goto zero;
            }
        zero:
            zptr[j] = v;
        }
    } while (h != 1);
}

VA(0x004c9320, 0x422)
void sortIt(void) {
    lastPP = last + 1;

    if (lastPP <= 1024) {

        Int32 i;

        if (veryVerbose) {
            sprintf(
                gText,
                "trivialSort ...\n"
            );
            LogStr(gText);
        }
        for (i = 0; i <= last; i++)
            zptr[i] = i;
        shellTrivial();
        if (veryVerbose) {
            sprintf(
                gText,
                "trivialSort done.\n"
            );
            LogStr(gText);
        }

    } else {
        Int32 i;
        Int32 grade;
        Int32 notDone;

        stripe();

        if (veryVerbose) {
            sprintf(
                gText,
                "bucket sorting ...\n"
            );
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

        notDone = lastPP;
        for (grade = 1; grade <= 5; grade++) {
            Int32 candNo;
            Int32 loBound;
            Int32 hiBound;

            switch (grade) {
                case 1:
                    loBound = 2;
                    hiBound = 15;
                    break;
                case 2:
                    loBound = 16;
                    hiBound = 255;
                    break;
                case 3:
                    loBound = 256;
                    hiBound = 4095;
                    break;
                case 4:
                    loBound = 4096;
                    hiBound = 65535;
                    break;
                case 5:
                    loBound = 65536;
                    hiBound = 900000;
                    break;
                default:
                    panic(const_cast<char*>(
                        "gradedSort"));
                    break;
            }
            if (loBound > lastPP)
                continue;

            candNo = 0;
            for (i = 0; i <= 65535; i++) {

                Int32 freqHere = ftab[i + 1] - ftab[i];

                if (freqHere >= loBound && freqHere <= hiBound) {
                    Int32 j, k;
                    Int32 lower = ftab[i];
                    Int32 upper = ftab[i + 1] - 1;

                    candNo++;
                    notDone -= freqHere;

                    if (veryVerbose) {
                        sprintf(
                            gText,
                            "   %d -> %d:  cand %5d,   freq = %6d,   notdone = %6d",
                            loBound,
                            hiBound,
                            candNo,
                            freqHere,
                            notDone
                        );
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
                    if (veryVerbose) {
                        sprintf(
                            gText,
                            " "
                        );
                        LogStr(gText);
                    }
                }
            }
        }
    }
}

VA(0x004c9750, 0x87)
void doReversibleTransformation(void) {
    Int32 i;

    if (veryVerbose) {
        sprintf(
            gText,
            " "
        );
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

VA(0x004c97e0, 0x1b2)
void undoReversibleTransformation(void) {
    Int32 cc[256];
    Int32 i, j, ch, sum;

    for (i = 0; i <= 255; i++)
        cc[i] = 0;

    for (i = 0; i <= last; i++) {
        UChar ll_i = ll[i];
        zptr[i] = cc[ll_i];
        cc[ll_i]++;
    }

    sum = 0;
    for (ch = 0; ch <= 255; ch++) {
        sum = sum + cc[ch];
        cc[ch] = sum - cc[ch];
    }

    i = origPtr;
    for (j = last; j >= 0; j--) {
        UChar ll_i = ll[i];
        block[j] = ll_i;
        i = zptr[i] + cc[ll_i];
    }
}

#define SPOT_BASIS_STEP 8000

VA(0x004c99a0, 0x173)
void spotBlock(Bool weAreCompressing) {
    Int32 index, delta, updatedDelta2;

    index = SPOT_BASIS_STEP;
    delta = 1;

    while (index < last) {

        Int32 n;

        if (weAreCompressing)
            n = (Int32)GETFIRST(index) + 1;
        else
            n = (Int32)block[index] - 1;

        if (n == 256)
            n = 0;
        else if (n == -1)
            n = 255;

        if (!(n >= 0 && n <= 255))
            panic(const_cast<char*>("spotBlock"));

        if (weAreCompressing)
            SETFIRST(index, (UChar)n);
        else
            block[index] = (UChar)n;

        switch (delta) {
            case 3:
                updatedDelta2 = 1;
                break;
            case 1:
                updatedDelta2 = 4;
                break;
            case 4:
                updatedDelta2 = 5;
                break;
            case 5:
                updatedDelta2 = 9;
                break;
            case 9:
                updatedDelta2 = 2;
                break;
            case 2:
                updatedDelta2 = 6;
                break;
            case 6:
                updatedDelta2 = 7;
                break;
            case 8:
                updatedDelta2 = 8;
                break;
            case 7:
                updatedDelta2 = 3;
                break;
            default:
                updatedDelta2 = 1;
                break;
        }
        delta = updatedDelta2;

        index = index + SPOT_BASIS_STEP + 17 * (updatedDelta2 - 5);
    }
}

VA(0x004c9b20, 0x134)
Int32 getRLEpair(FILE* src) {
    Int32 runLength;
    IntNative ch, chLatest;

    ch = getc(src);

    if (ch == EOF) {
        ERROR_IF_NOT_ZERO(errno);
        return (1 << 16) | MY_EOF;
    }

    runLength = 0;
    do {
        chLatest = getc(src);
        runLength++;
        bytesIn++;
    } while (ch == chLatest && runLength < 255);

    if (chLatest != EOF) {
        if (ungetc(chLatest, src) == EOF)
            panic(const_cast<char*>("getRLEpair: ungetc failed"));
    } else {
        ERROR_IF_NOT_ZERO(errno);
    }

    if (runLength == 1) {
        UPDATE_CRC(globalCrc, (UChar)ch);
        return (1 << 16) | ch;
    } else {
        Int32 i;
        for (i = 1; i <= runLength; i++)
            UPDATE_CRC(globalCrc, (UChar)ch);
        return (runLength << 16) | ch;
    }
}

VA(0x004c9c60, 0x20c)
Bool loadAndRLEsource(FILE* src) {
    Int32 ch, allowableBlockSize;

    last = -1;
    ch = 0;

    allowableBlockSize = 100000 * blockSize100k - 20;

    while (last < allowableBlockSize && ch != MY_EOF) {
        Int32 rlePair, runLen;
        rlePair = getRLEpair(src);
        ch = rlePair & 0xFFFF;
        runLen = (UInt32)rlePair >> 16;

        if (ch == MY_EOF) {
            last++;
            SETFIRST(last, ((UChar)42));
        } else
            switch (runLen) {
                case 1:
                    last++;
                    SETFIRST(last, ((UChar)ch));
                    break;
                case 2:
                    last++;
                    SETFIRST(last, ((UChar)ch));
                    last++;
                    SETFIRST(last, ((UChar)ch));
                    break;
                case 3:
                    last++;
                    SETFIRST(last, ((UChar)ch));
                    last++;
                    SETFIRST(last, ((UChar)ch));
                    last++;
                    SETFIRST(last, ((UChar)ch));
                    break;
                default:
                    last++;
                    SETFIRST(last, ((UChar)ch));
                    last++;
                    SETFIRST(last, ((UChar)ch));
                    last++;
                    SETFIRST(last, ((UChar)ch));
                    last++;
                    SETFIRST(last, ((UChar)ch));
                    last++;
                    SETFIRST(last, ((UChar)(runLen - 4)));
                    break;
            }
    }
    return (ch == MY_EOF);
}

VA(0x004c9e70, 0x176)
void unRLEandDump(FILE* dst, Bool thisIsTheLastBlock) {
    IntNative retVal;
    Int32 lastCharToSpew, i, count, chPrev, ch;
    UInt32 localCrc;

    if (thisIsTheLastBlock)
        lastCharToSpew = last - 1;
    else
        lastCharToSpew = last;

    count = 0;
    i = 0;
    ch = 256;
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
                for (j = 0; j < (Int32)block[i]; j++) {
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

    if (thisIsTheLastBlock && block[last] != 42)
        unblockError();
}

VA(0x004c9ff0, 0x257)
void compressStream(FILE* stream, FILE* zStream) {
    IntNative retVal;
    Bool thisIsTheLastBlock;
    BitStream* zbs;
    UInt32 crcToSend;
    Int32 blockNo = 1;

    bytesIn = 0;
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
        if (veryVerbose) {
            sprintf(
                gText,
                "\nBEGIN block %d\n",
                blockNo
            );
            LogStr(gText);
        }
        blockNo++;
        thisIsTheLastBlock = loadAndRLEsource(stream);
        spotBlock(True);
        doReversibleTransformation();
        moveToFrontCodeAndSend(zbs, thisIsTheLastBlock);
    } while (!thisIsTheLastBlock);

    crcToSend = getFinalCRC();
    putUInt32(zbs, crcToSend);
    if (veryVerbose) {
        sprintf(
            gText,
            "\nCRC = 0x%x\n",
            crcToSend
        );
        LogStr(gText);
    }

    arithCodeDoneEncoding(zbs);
    bsClose(zbs);
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
            (8.0 * (float)bytesOut)
                / (float)bytesIn,
            100.0
                * (1.0
                   - (float)bytesOut / (float)bytesIn),
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

VA(0x004ca250, 0x233)
Bool uncompressStream(FILE* zStream, FILE* stream) {
    Bool thisIsTheLastBlock;
    BitStream* zbs;
    Int32 magic1, magic2, magic3, magic4;
    UInt32 crcStored, crcComputed;
    Int32 currBlockNo;
    IntNative retVal;

    zbs = (bsOpenReadStream(zStream));

    magic1 = (Int32)bsGetUChar(zbs);
    magic2 = (Int32)bsGetUChar(zbs);
    magic3 = (Int32)bsGetUChar(zbs);
    magic4 = (Int32)bsGetUChar(zbs);
    if (magic1 != 'B' || magic2 != 'Z' || magic3 != '0' || magic4 < '1' || magic4 > '9') {
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

    if (veryVerbose) {
        sprintf(gText, "  ");
        LogStr(gText);
    }
    currBlockNo = 0;
    do {
        currBlockNo++;
        if (veryVerbose) {
            sprintf(
                gText,
                "[%d: ac+mtf ",
                currBlockNo
            );
            LogStr(gText);
        }
        thisIsTheLastBlock = getAndMoveToFrontDecode(zbs);
        if (veryVerbose)
            LogStr("rt ");
        undoReversibleTransformation();
        spotBlock(False);
        if (veryVerbose)
            LogStr("rld");
        unRLEandDump(stream, thisIsTheLastBlock);
        if (veryVerbose)
            LogStr("] ");
    } while (!thisIsTheLastBlock);

    if (veryVerbose)
        LogStr(" ");

    crcStored = getUInt32(zbs);
    crcComputed = getFinalCRC();
    if (veryVerbose) {
        sprintf(
            gText,
            "CRCs: stored = 0x%x, computed = 0x%x\n  ",
            crcStored,
            crcComputed
        );
        LogStr(gText);
    }
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

VA(0x004ca490, 0x5)
void showFileNames(void) {}

VA(0x004ca4a0, 0x5)
void cleanUpAndFail(void) {}

VA(0x004ca4b0, 0xb)
void panic(char* H2_UNUSED(s)) {}

VA(0x004ca4c0, 0x45)
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

VA(0x004ca510, 0x40)
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

VA(0x004ca550, 0x40)
void ioError(void) {
    sprintf(
        gText,
        "\n%s: I/O or other error, bailing out.  Possible reason follows.\n",
        progName
    );
    LogStr(gText);
    perror(progName);
    showFileNames();
    cleanUpAndFail();
}

VA(0x004ca590, 0x31)
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

VA(0x004ca5d0, 0x31)
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

VA(0x004ca610, 0x31)
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

VA(0x004ca650, 0x2c)
void __cdecl mySignalCatcher(IntNative* H2_UNUSED(n)) {
    sprintf(
        gText,
        "\n%s: Control-C (or similar) caught, quitting.\n",
        progName
    );
    LogStr(gText);
    cleanUpAndFail();
}

VA(0x004ca680, 0x65)
void mySIGSEGVorSIGBUScatcher(IntNative* H2_UNUSED(n)) {
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

VA(0x004ca6f0, 0x45)
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

VA(0x004ca740, 0x45)
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

VA(0x004ca790, 0x66)
Bool endsInBz(Char* name) {
    Int32 n = strlen(name);
    if (n <= 3)
        return False;
    return (name[n - 3] == '.' && name[n - 2] == 'n' && name[n - 1] == 'w');
}

VA(0x004ca800, 0xa4)
void compress(Char* name) {
    FILE* inStr;
    FILE* outStr;
    IntNative H2_UNUSED(retVal);

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

VA(0x004ca8b0, 0xc2)
void uncompress(Char* name) {
    FILE* inStr;
    FILE* outStr;
    Bool H2_UNUSED(magicNumberOK);
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

VA(0x004ca980, 0x259)
i32l EncodeData(char* dst, char* src, u32l srcLen) {
    char fname[450] = H2_ZERO_INIT;
    i32 fd;
    i32 H2_UNUSED(result);
    FILE* fp;
    i32l flen;

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

    fd = _open(fname, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fd == -1)
        FileError(fname);
    _write(fd, src, srcLen);
    _close(fd);
    compress(fname);

    strcat(fname, ".nw");
    fp = fopen(fname, "rb");
    result = fseek(fp, 0, 2);
    flen = ftell(fp);
    result = fseek(fp, 0, 0);
    result = fread(dst, flen, 1, fp);
    result = fclose(fp);
    result = remove(fname);
    FreeCompressStructures();

    return flen;
}

VA(0x004cabe0, 0x266)
i32l DecodeData(char* dst, char* src, u32l srcLen) {
    char fname[450] = H2_ZERO_INIT;
    i32 fd;
    i32 H2_UNUSED(result);
    FILE* fp;
    i32l flen;

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

    fd = _open(fname, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (fd == -1)
        FileError(fname);
    _write(fd, src, srcLen);
    _close(fd);
    uncompress(fname);

    fname[strlen(fname) - 3] = '\0';
    fp = fopen(fname, "rb");
    result = fseek(fp, 0, 2);
    flen = ftell(fp);
    result = fseek(fp, 0, 0);
    result = fread(dst, flen, 1, fp);
    result = fclose(fp);
    result = remove(fname);
    FreeCompressStructures();

    return flen;
}

DATA(0x00536c44) i32 longestFileName;
DATA(0x00539584) i32 opMode;
DATA(0x00537054) char inName[1024];
DATA(0x00536c54) char outName[1024];
DATA(0x00536844) char progNameReally[0x400];
DATA(0x00536838) i32 keepInputFiles;
