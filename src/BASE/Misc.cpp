// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Misc.obj   from: .\basewin.lib
// functions: 46   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/kbwin.h>
#include <BASE/heroWindow.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <BASE/font.h>
#include <BASE/Misc.h>
#include <BASE/miscwin.h>        // this TU's own free functions + indexArray/IconEntry
#include <SOURCE/KB.h>        // EventWindowHandler, FileError, ShutDown
#include <SOURCE/wingraph.h>  // SetFullScreenStatus
#include <_carcass_types.h>   // tag_message (member access)
#include <windows.h>      // MessageBoxA
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int giFindMid;

VA(0x004c3d10, 0x58)
void InitMemEntry(void)
{
    LogInt((char *)"IME", iMemEntries, -999, -999, -999, -999, -999, -999);
    gpMemEntry = (MemEntry *)malloc(0x24220);
    int i = 0;
    do {
        i += 0x4a;
        ((char *)gpMemEntry)[i - 0x4a] = 0;
    } while (i < 0x24220);
}

VA(0x004c3d70, 0x20f)
void * BaseAlloc(unsigned int, char *, int) { return 0; }

VA(0x004c3f80, 0x386)
void BaseFree(void *, char *, int) {}

VA(0x004c4310, 0x134)
void PrintMemoryLeaks(void) {}

VA(0x004c4450, 0x91)
void ShowMemoryStatus(void) {}

VA(0x004c44f0, 0x48)
unsigned long int MAKEFILEID(char *text)
{
    unsigned int hash = 0;
    int sum = 0;
    for (int i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] &= ~0x20;
        }
        hash = (hash << 5) + (hash >> 25);
        sum += text[i];
        hash += text[i] + sum;
    }
    return hash;
}

VA(0x004c4540, 0x95)
int FindIndex(struct indexArray *entries, int low, int high, int key)
{
    giFindMid = (low + high) >> 1;
    while (high - low > 1) {
        if (entries[giFindMid].field0 <= key) {
            low = giFindMid;
            if (entries[giFindMid].field0 == key) {
                return entries[low].field2;
            }
        } else {
            high = giFindMid;
        }
        giFindMid = (low + high) >> 1;
    }
    if (entries[low].field0 == key) {
        return entries[low].field2;
    }
    if (entries[high].field0 == key) {
        return entries[high].field2;
    }
    return 0xFFFF;
}

VA(0x004c45e0, 0xea)
void FadeIn(int) {}

VA(0x004c46d0, 0xe6)
void FadeOut(int) {}

VA(0x004c47c0, 0x28)
int Random(int low, int high)
{
    if (low == high) {
        return high;
    }
    if (high < low) {
        return low;
    }
    return low + rand() % (high - low + 1);
}

// @early-stop
// byte-exact except the MessageBoxA call: retail is a bare `ff 15 [0x53a650]` (the
// delinker left the IAT import unnamed); ours is the same `ff 15` with an
// __imp__MessageBoxA@16 reloc. Code bytes identical; only the masked IAT operand differs.
VA(0x004c47f0, 0x5d)
void ProcessAssert(int condition, char *file, int line)
{
    if (condition == 0) {
        gpMouseManager->SetColorMice(0);
        SetFullScreenStatus(0);
        sprintf(gText, "Assert statement failed in module %s, line %d.  Do you wish to abort the program?", file, line);
        if (MessageBoxA(hwndApp, gText, "Assert Failure", 0x14) != 7) {
            ShutDown(0);
        }
    }
}

VA(0x004c4850, 0x66)
char * FindStringInString(char *text, char *pattern)
{
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);
    int count = text_len - pattern_len + 1;
    for (int i = 0; i < count; ++i) {
        if (strncmp(text + i, pattern, pattern_len) == 0) {
            return text + i;
        }
    }
    return 0;
}

VA(0x004c48c0, 0x31)
char * FindToken(char *text, char token)
{
    int len = strlen(text);
    for (int i = 0; len > i; ++i) {
        if (text[i] == token) {
            return text + i;
        }
    }
    return 0;
}

VA(0x004c4900, 0x2b)
char * FindLastToken(char *text, char token)
{
    for (int i = strlen(text) - 1; i >= 0; --i) {
        if (text[i] == token) {
            return text + i;
        }
    }
    return 0;
}

VA(0x004c4930, 0x6c)
void SetInstallDefaults(void) {}

VA(0x004c49a0, 0x1b5)
void SetGameDefaults(void) {}

VA(0x004c4b60, 0x13f)
void ReadPrefsFromFile(void) {}

VA(0x004c4ca0, 0x7ab)
void ReadPrefsFromRegistry(void) {}

VA(0x004c5450, 0xa1)
void ReadPrefs(void) {}

VA(0x004c5500, 0x6a)
void WritePrefsToFile(void) {}

VA(0x004c5570, 0x491)
void WritePrefsToRegistry(void) {}

VA(0x004c5a10, 0xa)
void WritePrefs(void)
{
    UpdateSystemOptionsMenu();
    WritePrefsToRegistry();
}

VA(0x004c5a20, 0x3c)
int IsCDDrive(int param_1)
{
    sprintf(gText, "A:\\");
    gText[0] = gText[0] + param_1;
    return GetDriveTypeA(gText) == DRIVE_CDROM;
}

VA(0x004c5a60, 0x3ed)
int SetupCDDrive(void) { return 0; }

VA(0x004c5e50, 0x18)
void BitmapToScreen(class bitmap *bmp)
{
    BlitBitmapToScreen(bmp, 0, 0, bmp->field_0x12, bmp->field_0x14, 0, 0);
}

VA(0x004c5e70, 0x3d)
void SetPalette(signed char *, int) {}

VA(0x004c5eb0, 0x25)
void BlitBitmapToScreenNoMouseCheck(class bitmap *bmp, int p2, int p3, int p4, int p5, int p6, int p7)
{
    BlitBitmapToScreenVesa(reinterpret_cast<int>(bmp), p2, p3, p4, p5, p6, p7);
}

VA(0x004c5ee0, 0x18b)
void BlitBitmapToScreen(class bitmap *, int, int, int, int, int, int) {}

VA(0x004c6070, 0xa6)
void LogTruncate(void) {}

VA(0x004c6120, 0x9e)
void LogStr(char *param_1)
{
    char local_1f4[500];
    if (giDebugLevel > 1) {
        FILE *f = fopen("KB.LOG", "a");
        if (f != 0) {
            strcpy(local_1f4, param_1);
            strcat(local_1f4, "\n");
            fputs(local_1f4, f);
            fclose(f);
        }
        OutputDebugStringA(local_1f4);
    }
}

VA(0x004c61c0, 0x224)
void LogInt(char *, int, int, int, int, int, int, int) {}

VA(0x004c63f0, 0x6c)
void AiPrint(char *param_1)
{
    if (giDebugLevel > 1) {
        FillBitmapArea(gpWindowManager->field_0x46, 0, 0x1cc, 0x280, 0x14, 0);
        smallFont->DrawBoundedString(param_1, 0, 0x1d0, 0x280, 0x10, 1, 0);
        BlitBitmapToScreen(gpWindowManager->field_0x46, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    }
}

VA(0x004c6460, 0x7a)
void AbsAiPrint(char *param_1)
{
    int saved = giDebugLevel;
    giDebugLevel = 9;
    FillBitmapArea(gpWindowManager->field_0x46, 0, 0x1cc, 0x280, 0x14, 0);
    smallFont->DrawBoundedString(param_1, 0, 0x1d0, 0x280, 0x10, 1, 0);
    BlitBitmapToScreen(gpWindowManager->field_0x46, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    giDebugLevel = saved;
}

VA(0x004c64e0, 0xf8)
void FadeTo(unsigned char *, unsigned char *, int) {}

VA(0x004c65e0, 0xb8)
void FadeToColorTable(unsigned char *, int) {}

VA(0x004c66a0, 0x29)
int IsCycleColor(int color)
{
    if ((color >= 0xD6 && color <= 0xDD) || (color >= 0xE7 && color <= 0xED)) {
        return 1;
    }
    return 0;
}

VA(0x004c66d0, 0x1ee)
void CreatePCXFile(char *, unsigned char *, int, int, unsigned char *) {}

VA(0x004c68c0, 0x52)
long int FileSize(char *filename)
{
    FILE *file = fopen(filename, "r+b");
    if (file == 0) {
        FileError(filename);
    }
    fseek(file, 0, 2);
    long size = ftell(file);
    fseek(file, 0, 0);
    fclose(file);
    return size;
}

VA(0x004c6920, 0xc)
struct IconEntry * GetIconEntry(class icon *iconPtr, int index)
{
    IconEntry *entries = *(IconEntry **)((char *)iconPtr + 0x12);
    return &entries[index];
}

VA(0x004c6930, 0xb8)
int SRandom(int low, int high)
{
    if (high == low) {
        return high;
    }
    if (high < low) {
        return low;
    }

    int high_term = (high * 13) & 0xff;
    int low_term = (low * 13) & 0xff;
    iLastSeed += high_term << 5;
    iLastSeed += low_term * 13233;
    iLastSeed += high_term;
    iLastSeed += (iLastSeed & 0x3f) << 8;
    iLastSeed &= 0xfff;

    int mix = iLastSeed * 7;
    mix += (mix & 0xff0) >> 4;
    int result = 0;
    for (int i = 31; i >= 0; --i) {
        if (mix & (1 << i)) {
            result |= 1 << i;
        }
    }
    iLastSeed = mix + low + high * 8;
    return low + result % (high - low + 1);
}

VA(0x004c69f0, 0x5c)
void SIncRandomize(int x, int y)
{
    int x_term = (x * 13) & 0xff;
    int y_term = (y * 13) & 0xff;
    iLastSeed += y_term << 5;
    iLastSeed += x_term * 13233;
    iLastSeed += y_term;
    iLastSeed += (iLastSeed & 0x3f) << 8;
}

VA(0x004c6a50, 0x10)
void SRand(int seed)
{
    iLastSeed = seed;
    srand(seed);
}

VA(0x004c6a60, 0x48)
int SGenRand(void)
{
    int result = 0;
    iLastSeed &= 0xfff;
    int mix = iLastSeed * 7;
    mix += (mix & 0xff0) >> 4;
    for (int i = 31; i >= 0; --i) {
        if (mix & (1 << i)) {
            result |= 1 << i;
        }
        iLastSeed = mix;
    }
    return result;
}

VA(0x004c6ab0, 0x6)
int MemSize(int)
{
    return 0x3ea2;
}

VA(0x004c6ac0, 0x386)
void GetDataEntry(char *, char *, int, char *, int, int) {}

VA(0x004c6e50, 0x173)
int DataEntryWindowHandler(struct tag_message &message)
{
    if (bDataEntryTime == 0) {
        bDataEntryTime = 1;
        message.type = 8;
        message.field4 = inBoxX;
        message.field8 = inBoxY;
        DataEntryWin->BroadcastMessage(message);
        return 1;
    }

    if (bDataEntryTime != 1) {
        if (message.type != 0x200) {
            return EventWindowHandler(message);
        }
        if (message.field4 == 0xC) {
            if (message.field8 != 0xA) {
                return EventWindowHandler(message);
            }
        } else if (message.field4 == 0xD) {
            if (message.field8 == 0x7802) {
                message.field8 = 0xA;
                message.field4 = 0xA;
                return 2;
            }
            return EventWindowHandler(message);
        } else {
            return EventWindowHandler(message);
        }
    }

    message.type = 0x200;
    message.field8 = 0xA;
    message.field4 = 7;
    DataEntryWin->BroadcastMessage(message);
    if (strlen(message.text) == 0) {
        return EventWindowHandler(message);
    }
    memset(cDEDest, 0, iDEMaxLen);
    strncpy(cDEDest, message.text, iDEMaxLen - 1);
    message.type = 0x200;
    message.field4 = 3;
    message.field8 = 0xA;
    message.text = cDEDest;
    DataEntryWin->BroadcastMessage(message);
    DataEntryWin->DrawWindow(1, 10, 10);
    if (gbTextEntryEscaped != 0) {
        return EventWindowHandler(message);
    }
    *(int *)((char *)gpWindowManager + 0x5a) = message.field8;
    message.field8 = 0xA;
    message.field4 = 0xA;
    return 2;
}

// ---- globals (definitions, RVA order) ----
int iMemEntries;
MemEntry *gpMemEntry;
int giTotalMemAllocated;
unsigned char *giChangeThreshold;
int iLastSeed;
class heroWindow *DataEntryWin;
char *cDEDest;
int iDEMaxLen;
int bDataEntryTime;
int inBoxX;
int inBoxY;
