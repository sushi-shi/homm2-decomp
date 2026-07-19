#ifndef HOMM2_BASE_BZIP_H
#define HOMM2_BASE_BZIP_H

#include <va.h>
#include <stdio.h>

typedef i32 Int32;
typedef u32 UInt32;
typedef i16 Int16;
typedef u16 UInt16;
typedef char Char;
typedef u8 UChar;
typedef i32 Bool;
typedef i32 IntNative;

H2_ENUM_BEGIN(BzipStorageConstant)
    MAX_SYMBOLS                = 256,
    MODEL_FREQUENCY_PADDING    = 2,
    BZIP_CRC_TABLE_SIZE        = 256,
    BZIP_PROGRAM_NAME_CAPACITY = 0x400
H2_ENUM_END(BzipStorageConstant)

struct BitStream {
    FILE* handle;
    Int32 buffer;
    Int32 buffLive;
    Char mode;
};
struct Model {
    UInt32 numScalings;
    UInt32 numTraffic;
    UInt32 totFreq;
    UInt32 numSymbols;
    UInt32 incValue;
    UInt32 noExceed;
    Char* name;
    UInt32 freq[MAX_SYMBOLS + MODEL_FREQUENCY_PADDING];
};

void initialiseCRC(void);
u32 getFinalCRC(void);
u32 getGlobalCRC(void);
void setGlobalCRC(u32 newCrc);
BitStream* bsOpenReadStream(FILE* f);
BitStream* bsOpenWriteStream(FILE* f);
void bsPutBit(BitStream* bs, i32 bit);
i32 bsGetBit(BitStream* bs);
u8 bsGetUChar(BitStream* bs);
void bsPutUChar(BitStream* bs, u8 c);
void bsClose(BitStream* bs);
u32 minUInt32(u32 a, u32 b);
void arithCodeBitPlusFollow(BitStream* bs, UInt32 bit);
void arithCodeStartEncoding(BitStream* bs);
void arithCodeDoneEncoding(BitStream* bs);
void arithCodeStartDecoding(BitStream* bs);
void arithCodeDoneDecoding(BitStream* bs);
void arithCodeRenormalise_Encode(BitStream* bs);
void arithCodeSymbol(BitStream* bs, Model* m, Int32 symbol);
Int32 arithDecodeSymbol(BitStream* bs, Model* m);
void initModel(
    Model* m,
    Char* initName,
    Int32 initNumSymbols,
    Int32 initIncValue,
    Int32 initNoExceed
);
void dumpModelStats(Model* m);
void updateModel(Model* m, Int32 symbol);
void putSymbol(Model* m, Int32 symbol, BitStream* bs);
Int32 getSymbol(Model* m, BitStream* bs);
void initBogusModel(void);
void putUChar(BitStream* bs, UChar c);
void putInt32(BitStream* bs, Int32 i);
void putUInt32(BitStream* bs, UInt32 i);
UChar getUChar(BitStream* bs);
Int32 getInt32(BitStream* bs);
UInt32 getUInt32(BitStream* bs);
void initModels(void);
void dumpAllModelStats(void);
Int32 getMTFVal(BitStream* bs);
void sendMTFVal(BitStream* bs, Int32 n);
void FreeCompressStructures(void);
void allocateCompressStructures(void);
void FreeDecompressStructures(void);
void setDecompressStructureSizes(Int32 newSize100k);
UInt32 GETALL(Int32 a);
void SETREST16(Int32 a, UInt32 w);
void SETFIRST16(Int32 a, UInt32 w);
void SETREST(Int32 a, UInt32 w);
void SETFIRST(Int32 a, UChar c);
void SETSECOND(Int32 a, UChar c);
void SETTHIRD(Int32 a, UChar c);
void SETFOURTH(Int32 a, UChar c);
Int32 NORMALISE(Int32 p);
Int32 NORMALISEHI(Int32 p);
Int32 NORMALISELO(Int32 p);
Int32 STRONG_NORMALISE(Int32 p);
void sendZeroes(BitStream* outStream, Int32 zeroesPending);
void moveToFrontCodeAndSend(BitStream* outStream, Bool thisIsTheLastBlock);
Bool getAndMoveToFrontDecode(BitStream* inStream);
void stripe(void);
void copyOffsetWords(void);
Bool fullGt(Int32 i1, Int32 i2);
void qsortFull(Int32 left, Int32 right);
Bool trivialGt(Int32 i1, Int32 i2);
void shellTrivial(void);
void sortIt(void);
void doReversibleTransformation(void);
void undoReversibleTransformation(void);
void spotBlock(Bool weAreCompressing);
Int32 getRLEpair(FILE* src);
Bool loadAndRLEsource(FILE* src);
void unRLEandDump(FILE* dst, Bool thisIsTheLastBlock);
void compressStream(FILE* stream, FILE* zStream);
Bool uncompressStream(FILE* zStream, FILE* stream);
void showFileNames(void);
void cleanUpAndFail(void);
void panic(char* s);
void crcError(UInt32 crcStored, UInt32 crcComputed);
void compressedStreamEOF(void);
void ioError(void);
void blockOverrun(void);
void unblockError(void);
void bitStreamEOF(void);
void __cdecl mySignalCatcher(IntNative* n);
void mySIGSEGVorSIGBUScatcher(IntNative* n);
void uncompressOutOfMemory(Int32 draw, Int32 blockSize);
void compressOutOfMemory(Int32 draw, Int32 blockSize);
Bool endsInBz(Char* name);
void compress(Char* name);
void uncompress(Char* name);
i32l EncodeData(char* dst, char* src, u32l srcLen);
i32l DecodeData(char* dst, char* src, u32l srcLen);

extern UInt32 crc32Table[BZIP_CRC_TABLE_SIZE];
extern char inName[];
extern char outName[];

extern i32 longestFileName;
extern i32 opMode;
extern char progNameReally[BZIP_PROGRAM_NAME_CAPACITY];
extern i32 keepInputFiles;

#endif
