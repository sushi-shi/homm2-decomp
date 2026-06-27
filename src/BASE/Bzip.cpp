// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Bzip.obj   from: .\basewin.lib
// functions: 88   data: 32
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x004d4050, 0x1a)
// void initialiseCRC(void);

VA(0x004d4070, 0x17)
// unsigned int getFinalCRC(void);

VA(0x004d4090, 0x15)
// unsigned int getGlobalCRC(void);

VA(0x004d40b0, 0x1e)
// void setGlobalCRC(unsigned int);

VA(0x004d40d0, 0x64)
// struct BitStream * bsOpenReadStream(struct _iobuf *);

VA(0x004d4140, 0x64)
// struct BitStream * bsOpenWriteStream(struct _iobuf *);

VA(0x004d41b0, 0x8c)
// void bsPutBit(struct BitStream *, int);

VA(0x004d4240, 0x95)
// int bsGetBit(struct BitStream *);

VA(0x004d42e0, 0x50)
// unsigned char bsGetUChar(struct BitStream *);

VA(0x004d4330, 0x4e)
// void bsPutUChar(struct BitStream *, unsigned char);

VA(0x004d4380, 0xe8)
// void bsClose(struct BitStream *);

VA(0x004d4470, 0x35)
// unsigned int minUInt32(unsigned int, unsigned int);

VA(0x004d44b0, 0x4e)
// void arithCodeBitPlusFollow(struct BitStream *, unsigned int);

VA(0x004d4500, 0x34)
// void arithCodeStartEncoding(struct BitStream *);

VA(0x004d4540, 0x4c)
// void arithCodeDoneEncoding(struct BitStream *);

VA(0x004d4590, 0x68)
// void arithCodeStartDecoding(struct BitStream *);

VA(0x004d4600, 0x16)
// void arithCodeDoneDecoding(struct BitStream *);

VA(0x004d4620, 0x98)
// void arithCodeRenormalise_Encode(struct BitStream *);

VA(0x004d46c0, 0xd7)
// void arithCodeSymbol(struct BitStream *, struct Model *, int);

VA(0x004d47a0, 0xfe)
// int arithDecodeSymbol(struct BitStream *, struct Model *);

VA(0x004d48a0, 0xe0)
// void initModel(struct Model *, char *, int, int, int);

VA(0x004d4980, 0x3f)
// void dumpModelStats(struct Model *);

VA(0x004d49c0, 0xa0)
// void updateModel(struct Model *, int);

VA(0x004d4a60, 0x35)
// void putSymbol(struct Model *, int, struct BitStream *);

VA(0x004d4aa0, 0x35)
// int getSymbol(struct Model *, struct BitStream *);

VA(0x004d4ae0, 0x2b)
// void initBogusModel(void);

VA(0x004d4b10, 0x2f)
// void putUChar(struct BitStream *, unsigned char);

VA(0x004d4b40, 0x6a)
// void putInt32(struct BitStream *, int);

VA(0x004d4bb0, 0x6a)
// void putUInt32(struct BitStream *, unsigned int);

VA(0x004d4c20, 0x28)
// unsigned char getUChar(struct BitStream *);

VA(0x004d4c50, 0x65)
// int getInt32(struct BitStream *);

VA(0x004d4cc0, 0x65)
// unsigned int getUInt32(struct BitStream *);

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
// void showFileNames(void);

VA(0x004d7990, 0x10)
// void cleanUpAndFail(void);

VA(0x004d79a0, 0x16)
// void panic(char *);

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
