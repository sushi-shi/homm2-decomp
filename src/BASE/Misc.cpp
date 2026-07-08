// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Misc.obj   from: .\basewin.lib
// functions: 46   data: 11
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/kbwin.h>
#include <BASE/heroWindow.h>
#include <BASE/mouseManager.h>
#include <_globals_model.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <BASE/font.h>
#include <BASE/Misc.h>
#include <BASE/miscwin.h>        // this TU's own free functions + indexArray/IconEntry
#include <SOURCE/KB.h>        // EventWindowHandler, FileError, ShutDown
#include <SOURCE/wingraph.h>
#include <SOURCE/NOOPT.h>  // SetFullScreenStatus
#include <_carcass_types.h>   // tag_message (member access)
#include <windows.h>      // MessageBoxA
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <BASE/palette.h>
#include <SOURCE/X_GLOBAL.h>

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
void *BaseAlloc(unsigned int param_1, char *param_2, int param_3)
{
    char local_2bc[200];
    char local_1f4[500];
    void *pvVar2;
    if (param_1 == 0) {
        pvVar2 = 0;
    } else {
        if (gpMemEntry == 0) {
            LogInt("Memory tracking table allocated, entries", iMemEntries, -999, -999, -999, -999,
                   -999, -999);
            gpMemEntry = static_cast<MemEntry *>(malloc(0x24220));
            for (int i = 0; i < 2000; i++)
                gpMemEntry[i].used = 0;
        }
        giTotalMemAllocated = giTotalMemAllocated + param_1;
        pvVar2 = malloc(param_1);
        if (pvVar2 == 0) {
            MemError();
            pvVar2 = 0;
        } else {
            iMemEntries = iMemEntries + 1;
            for (int i = 0; i < 2000; i++) {
                if (gpMemEntry[i].used == 0) {
                    gpMemEntry[i].used = 1;
                    gpMemEntry[i].ptr = pvVar2;
                    gpMemEntry[i].size = param_1;
                    strcpy(gpMemEntry[i].file, param_2);
                    gpMemEntry[i].line = param_3;
                    i = 99999;
                }
            }
            FILE *_File;
            if (giDebugLevel == 4 &&
                (sprintf(local_2bc, "KBAlloc    Size %d   Ptr %d   File %s  Line %d", param_1, pvVar2,
                         param_2, param_3),
                 1 < giDebugLevel) &&
                (_File = fopen("KB.LOG", "a"), _File != 0)) {
                strcpy(local_1f4, local_2bc);
                strcat(local_1f4, "\n");
                fputs(local_1f4, _File);
                fclose(_File);
                if (giDebugLevel == 4)
                    OutputDebugStringA(local_1f4);
            }
        }
    }
    return pvVar2;
}

VA(0x004c3f80, 0x386)
void BaseFree(void *param_1, char *param_2, int param_3)
{
    char local_2bc[500];
    char local_c8[200];
    if (gpMemEntry == 0) {
        LogInt("Memory tracking table allocated, entries", iMemEntries, -999, -999, -999, -999, -999,
               -999);
        gpMemEntry = static_cast<MemEntry *>(malloc(0x24220));
        for (int i = 0; i < 2000; i++)
            gpMemEntry[i].used = 0;
    }
    if (giDebugLevel == 4)
        LogInt("Free ", reinterpret_cast<int>(param_1), -999, -999, -999, -999, -999, -999);
    if (param_1 == 0) {
        if (1 < giDebugLevel) {
            FILE *_File = fopen("KB.LOG", "a");
            if (_File != 0) {
                strcpy(local_2bc, "NULL POINTER");
                strcat(local_2bc, "\n");
                fputs(local_2bc, _File);
                fclose(_File);
                if (giDebugLevel == 4)
                    OutputDebugStringA(local_2bc);
            }
        }
    } else {
        iMemEntries = iMemEntries - 1;
        if (iMemEntries < 0)
            LogInt("MemEntries Below 0", iMemEntries, -999, -999, -999, -999, -999, -999);
        int iVar6 = 0;
        do {
            if (gpMemEntry[iVar6].ptr == param_1) {
                FILE *_File;
                if (giDebugLevel == 4 &&
                    (sprintf(local_c8, "KBFree    Size %d   Ptr %d   File %s  Line %d",
                             gpMemEntry[iVar6].size, param_1, gpMemEntry[iVar6].file,
                             gpMemEntry[iVar6].line),
                     1 < giDebugLevel) &&
                    (_File = fopen("KB.LOG", "a"), _File != 0)) {
                    strcpy(local_2bc, local_c8);
                    strcat(local_2bc, "\n");
                    fputs(local_2bc, _File);
                    fclose(_File);
                    if (giDebugLevel == 4)
                        OutputDebugStringA(local_2bc);
                }
                gpMemEntry[iVar6].used = 0;
                giTotalMemAllocated = giTotalMemAllocated - gpMemEntry[iVar6].size;
                iVar6 = 99999;
            }
            iVar6 = iVar6 + 1;
        } while (iVar6 < 2000);
        free(param_1);
    }
}

VA(0x004c4310, 0x134)
void PrintMemoryLeaks(void)
{
    char local_1f4[500];
    if (0 < giDebugLevel && gpMemEntry != 0) {
        LogInt("Total Memory Leaks", iMemEntries, -999, -999, -999, -999, -999, -999);
        int i = 0;
        do {
            if (gpMemEntry[i].used != 0) {
                sprintf(gText, "Memory Leak,  File '%13s'  Line % 4d, ptr %12d   size %6d",
                        gpMemEntry[i].file, gpMemEntry[i].line, reinterpret_cast<int>(gpMemEntry[i].ptr),
                        gpMemEntry[i].size);
                if (1 < giDebugLevel) {
                    FILE *_File = fopen("KB.LOG", "a");
                    if (_File != 0) {
                        strcpy(local_1f4, gText);
                        strcat(local_1f4, "\n");
                        fputs(local_1f4, _File);
                        fclose(_File);
                        if (giDebugLevel == 4)
                            OutputDebugStringA(local_1f4);
                    }
                }
            }
            i = i + 1;
        } while (i < 2000);
    }
}

VA(0x004c4450, 0x91)
void ShowMemoryStatus(void)
{
    sprintf(gText, "Mem Left %dK", 0x3ea2);
    int iVar1 = giDebugLevel;
    giDebugLevel = 9;
    FillBitmapArea(gpWindowManager->field_0x46, 0, 0x1cc, 0x280, 0x14, 0);
    gDebugFont->DrawBoundedString(gText, 0, 0x1d0, 0x280, 0x10, 1, 0);
    BlitBitmapToScreen(gpWindowManager->field_0x46, 0, 0x1cc, 0x280, 0x14, 0, 0x1cc);
    giDebugLevel = iVar1;
}

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
void FadeIn(int param_1)
{
    palette *pal = new palette();
    if (pal == 0)
        MemError();
    int done = 0;
    int local_8 = param_1;
    if (reinterpret_cast<int *>(&gConfig.pad[0x30])[giCurExe * 7] == 0)
        local_8 = param_1 * 2;
    memset(pal->field_0x10, 0, 0x300);
    int iVar3 = 0;
    do {
        if (0x3f < iVar3) {
            if (done) {
                delete pal;
                return;
            }
            iVar3 = 0x3f;
        }
        int local_4 = KBTickCount() + 0x14;
        PollSound();
        signed char *pcVar4;
        if (iVar3 == 0x3f) {
            done = 1;
            pcVar4 = gpBufferPalette->field_0x10;
        } else {
            int iVar5 = 0;
            do {
                if (0x3f - iVar3 < static_cast<int>(gpBufferPalette->field_0x10[iVar5]))
                    pal->field_0x10[iVar5] =
                        gpBufferPalette->field_0x10[iVar5] - static_cast<char>(0x3f - iVar3);
                iVar5++;
            } while (iVar5 < 0x300);
            pcVar4 = pal->field_0x10;
        }
        UpdatePalette(pcVar4);
        DelayTil(&local_4);
        iVar3 = iVar3 + local_8;
    } while (1);
}

VA(0x004c46d0, 0xe6)
void FadeOut(int param_1)
{
    palette *pal = new palette();
    if (pal == 0)
        MemError();
    int bVar3 = 0;
    memcpy(pal->field_0x10, gpBufferPalette->field_0x10, 0x300);
    int iVar5 = 0;
    do {
        if (0x3f < iVar5) {
            if (bVar3) {
                delete pal;
                return;
            }
            iVar5 = 0x3f;
        }
        int local_4 = KBTickCount() + 0x14;
        PollSound();
        if (iVar5 == 0x3f)
            bVar3 = 1;
        int iVar6 = 0;
        do {
            char cVar2 = pal->field_0x10[iVar6];
            if (cVar2 > 0) {
                if (static_cast<char>(param_1) < cVar2)
                    pal->field_0x10[iVar6] = cVar2 - static_cast<char>(param_1);
                else
                    pal->field_0x10[iVar6] = 0;
            }
            iVar6++;
        } while (iVar6 < 0x300);
        iVar5 = iVar5 + param_1;
        UpdatePalette(pal->field_0x10);
        DelayTil(&local_4);
    } while (1);
}

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
void ReadPrefs(void)
{
    memset(&gConfig, 0, 0x19d);
    ReadPrefsFromRegistry();
    sprintf(&gConfig.pad[0x13a], "RMT%sRL.BIN", &gConfig.pad[0x122]);
    sprintf(&gConfig.pad[0x147], "RMT%sRC.BIN", &gConfig.pad[0x122]);
    sprintf(&gConfig.pad[0x154], "RMT%sRD.BIN", &gConfig.pad[0x122]);
    sprintf(&gConfig.pad[0x161], "RMT%sSL.BIN", &gConfig.pad[0x122]);
    sprintf(&gConfig.pad[0x16e], "RMT%sSC.BIN", &gConfig.pad[0x122]);
    sprintf(&gConfig.pad[0x17b], "RMT%sSD.BIN", &gConfig.pad[0x122]);
}

VA(0x004c5500, 0x6a)
void WritePrefsToFile(void)
{
    int local_64[25];
    int i;
    int *p = local_64;
    for (i = 0x19; i != 0; i--) {
        *p = 0;
        p++;
    }
    sprintf(gText, "%s", "HEROES2.CFG");
    int fd = _open(gText, 0x8301, 0x80);
    if (fd != -1) {
        _write(fd, &gConfig, 0x19d);
        _close(fd);
    }
}

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
    BlitBitmapToScreen(bmp, 0, 0, bmp->width, bmp->height, 0, 0);
}

VA(0x004c5e70, 0x3d)
void SetPalette(signed char *param_1, int param_2)
{
    memcpy(gpBufferPalette->field_0x10, param_1, 0x300);
    memcpy(gCyclePal, param_1 + 0x282, 0x60);
    if (param_2 != 0)
        UpdatePalette(gpBufferPalette->field_0x10);
}

VA(0x004c5eb0, 0x25)
void BlitBitmapToScreenNoMouseCheck(class bitmap *bmp, int p2, int p3, int p4, int p5, int p6, int p7)
{
    BlitBitmapToScreenVesa(reinterpret_cast<int>(bmp), p2, p3, p4, p5, p6, p7);
}

VA(0x004c5ee0, 0x18b)
void BlitBitmapToScreen(class bitmap *param_1, int param_2, int param_3, int param_4, int param_5,
                        int param_6, int param_7)
{
    int local_8;
    if (gbColorMice == 0) {
        BlitBitmapToScreenVesa(reinterpret_cast<int>(param_1), param_2, param_3, param_4, param_5,
                               param_6, param_7);
        return;
    }
    if (gBlitClipX != 0 || (local_8 = param_2, gBlitClipY != 0)) {
        param_4 = 0x1c0;
        local_8 = gBlitClipX + 0x10;
        param_3 = gBlitClipY + 0x10;
        param_5 = 0x1c0;
    }
    gBlitRight = param_4 + param_6 - 1;
    gBlitBottom = param_5 + param_7 - 1;
    if (gpMouseManager->IsVis() != 0 && gpMouseManager->field_0x5e <= gBlitRight &&
        param_6 <= gpMouseManager->field_0x6e && gpMouseManager->field_0x62 <= gBlitBottom &&
        param_7 <= gpMouseManager->field_0x72) {
        gpMouseManager->SaveAndDraw();
        BlitBitmapToScreenVesa(reinterpret_cast<int>(param_1), local_8, param_3, param_4, param_5,
                               param_6, param_7);
        if (gBlitRight < gpMouseManager->field_0x6e || gpMouseManager->field_0x5e < param_6 ||
            gBlitBottom < gpMouseManager->field_0x72 || gpMouseManager->field_0x62 < param_7) {
            int iVar1 = gpMouseManager->field_0x62;
            int iVar2 = gpMouseManager->field_0x5e;
            BlitBitmapToScreenVesa(reinterpret_cast<int>(param_1), iVar2, iVar1,
                                   gpMouseManager->field_0x6e - iVar2 + 1,
                                   gpMouseManager->field_0x72 - iVar1 + 1, iVar2, iVar1);
        }
        gpMouseManager->RestoreUnderlying();
        return;
    }
    BlitBitmapToScreenVesa(reinterpret_cast<int>(param_1), local_8, param_3, param_4, param_5, param_6,
                           param_7);
}

VA(0x004c6070, 0xa6)
void LogTruncate(void)
{
    char local_1f4[500];
    if (1 < giDebugLevel) {
        int _FileHandle = _open("KB.LOG", 0x4301, 0x80);
        if (_FileHandle != -1) {
            strcpy(local_1f4, "===========New Log==========\n");
            _write(_FileHandle, local_1f4, strlen(local_1f4));
            _close(_FileHandle);
        }
    }
}

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
void LogInt(char *param_1, int param_2, int param_3, int param_4, int param_5, int param_6,
            int param_7, int param_8)
{
    char local_2bc[200];
    char local_1f4[500];
    if (param_8 == -999) {
        if (param_7 == -999) {
            if (param_6 == -999) {
                if (param_5 == -999) {
                    if (param_4 == -999) {
                        if (param_3 == -999)
                            sprintf(local_2bc, "%s : % 8d", param_1, param_2);
                        else
                            sprintf(local_2bc, "%s : % 8d % 8d", param_1, param_2, param_3);
                    } else {
                        sprintf(local_2bc, "%s : % 8d % 8d % 8d", param_1, param_2, param_3, param_4);
                    }
                } else {
                    sprintf(local_2bc, "%s : % 8d % 8d % 8d % 8d", param_1, param_2, param_3, param_4,
                            param_5);
                }
            } else {
                sprintf(local_2bc, "%s : % 8d % 8d % 8d % 8d % 8d", param_1, param_2, param_3, param_4,
                        param_5, param_6);
            }
        } else {
            sprintf(local_2bc, "%s : % 8d % 8d % 8d % 8d % 8d % 8d", param_1, param_2, param_3, param_4,
                    param_5, param_6, param_7);
        }
    } else {
        sprintf(local_2bc, "%s : % 8d % 8d % 8d % 8d % 8d % 8d % 8d", param_1, param_2, param_3,
                param_4, param_5, param_6, param_7, param_8);
    }
    if (1 < giDebugLevel) {
        FILE *_File = fopen("KB.LOG", "a");
        if (_File != 0) {
            strcpy(local_1f4, local_2bc);
            strcat(local_1f4, "\n");
            fputs(local_1f4, _File);
            fclose(_File);
            if (giDebugLevel == 4)
                OutputDebugStringA(local_1f4);
        }
    }
}

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
void FadeTo(unsigned char *param_1, unsigned char *param_2, int param_3)
{
    int local_310, local_304;
    unsigned char local_300[768];
    memcpy(local_300, param_1, 0x300);
    param_3 = param_3 >> 2;
    if (param_3 < 1)
        param_3 = 1;
    local_310 = 0x30;
    do {
        local_304 = KBTickCount() + 0x32;
        PollSound();
        int iVar6 = (0x40 - local_310) - param_3;
        if (iVar6 < 0)
            iVar6 = 0;
        unsigned char *pbVar8 = local_300;
        unsigned char bVar1 = giChangeThreshold[iVar6];
        iVar6 = 0x300;
        unsigned char *pbVar9 = param_2;
        do {
            unsigned char bVar2 = *pbVar8;
            unsigned int uVar7 = (unsigned int)*pbVar9 - (unsigned int)bVar2;
            int iVar5 = (uVar7 ^ ((int)uVar7 >> 0x1f)) - ((int)uVar7 >> 0x1f);
            if (static_cast<int>(bVar1) < iVar5) {
                char cVar3 = (char)iVar5 - bVar1;
                if ((int)uVar7 < 1)
                    *pbVar8 = bVar2 - cVar3;
                else
                    *pbVar8 = cVar3 + bVar2;
            }
            pbVar8++;
            pbVar9++;
            iVar6--;
        } while (iVar6 != 0);
        UpdatePalette(reinterpret_cast<signed char *>(local_300));
        DelayTil(&local_304);
        local_310 = local_310 + param_3;
    } while (local_310 < 0x40);
    UpdatePalette(reinterpret_cast<signed char *>(param_2));
}

VA(0x004c65e0, 0xb8)
void FadeToColorTable(unsigned char *param_1, int param_2)
{
    unsigned char auStack_300[768];
    int uVar1 = gpWindowManager->field_0x56;
    gpWindowManager->field_0x56 = 0;
    int iVar8 = 0;
    unsigned char *puVar2 = reinterpret_cast<unsigned char *>(gpBufferPalette->field_0x10);
    unsigned char *puVar3 = auStack_300;
    unsigned char *puVar4;
    do {
        puVar4 = puVar3 + 3;
        int iVar5 = static_cast<unsigned int>(param_1[iVar8]) * 3;
        iVar8++;
        *puVar3 = puVar2[iVar5];
        puVar3[1] = puVar2[iVar5 + 1];
        puVar3[2] = puVar2[iVar5 + 2];
        puVar3 = puVar4;
    } while (puVar4 < auStack_300 + 0x300);
    iVar8 = 0x1e0;
    FadeTo(puVar2, auStack_300, param_2);
    unsigned char *pbVar7 = gpWindowManager->field_0x46->pixels;
    do {
        int iVar5 = 0x280;
        unsigned char *pbVar6 = pbVar7;
        do {
            pbVar7 = pbVar6 + 1;
            iVar5--;
            *pbVar6 = param_1[*pbVar6];
            pbVar6 = pbVar7;
        } while (iVar5 != 0);
        iVar8--;
    } while (iVar8 != 0);
    gpWindowManager->UpdateScreen();
    UpdatePalette(reinterpret_cast<signed char *>(puVar2));
    gpWindowManager->field_0x56 = uVar1;
}

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
unsigned char giChangeThreshold[16];
int iLastSeed;
class heroWindow *DataEntryWin;
char *cDEDest;
int iDEMaxLen;
int bDataEntryTime;
int inBoxX;
int inBoxY;
