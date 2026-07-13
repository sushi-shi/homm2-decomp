// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Blur.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Blur.h>
#include <BASE/bitmap.h>
#include <BASE/palette.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>

DATA(0x004fa954) static int giCombatSpeed;

#define BLUR_COMPONENT(table, offset)                                                              \
    (*reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(table) + (offset)))

// @early-stop
// MSVC 4.2 /O2 register-allocation wall: base +0x00b0..+0x011e vs retail
// +0x00b0..+0x011c (palette-table setup), base +0x017c..+0x04e1 vs retail
// +0x017a..+0x04da (blur kernel), and base +0x04fb..+0x0530 vs retail
// +0x04f4..+0x0529 (palette allocation setup). Both frames are exactly 0xc5c bytes;
// samples 0..12 spill identically at esp+0x20..esp+0x50, with row/output/count at
// esp+0x14/0x54/0x64 and RGB tables at esp+0x6c/0x46c/0x86c. All 43 text
// relocations have equal semantic targets; retail merely delinks the real 0x4fa954
// speed-index global and local 350.0f constant under bogus/local names. Exhausted
// scalar, 16-array, and 13-array-plus-3-scalar layouts; all six scalar declaration
// orders; signed/unsigned sums; flat vs chained forward/reverse expressions;
// input/output/count order; pre/post increment; and early/late palette aliases.
VA(0x004d28e0, 0x6a4)
void DoBlur(
    bitmap* destination,
    bitmap* source,
    int height,
    int redAdjust,
    int greenAdjust,
    int blueAdjust
) {
    const int screenWidth = 0x280;
    const int border = 4;
    const int componentCount = 0x100;
    const int paletteBytes = 0x300;
    const int lookupSize = 0x8000;
    const int fizzleDelay = 0x96;

    PollSound();
    gpMouseManager->HideColorPointer();
    gpWindowManager->SaveFizzleSource(0, 0, screenWidth, height);

    bitmap* saved = new bitmap(0, screenWidth, static_cast<short>(height));
    unsigned int imageSize = height * screenWidth;
    memcpy(saved->m_pixels, source->m_pixels, imageSize);

#line 25
    unsigned char* lookup = static_cast<unsigned char*>(BaseAlloc(lookupSize, __FILE__, __LINE__));
    int red[componentCount];
    int green[componentCount];
    int blue[componentCount];
    signed char* paletteColor = gpBufferPalette->m_data;
    int componentOffset = 0;
    do {
        componentOffset += sizeof(unsigned int);
        paletteColor += 3;
        BLUR_COMPONENT(red, componentOffset - 4) = static_cast<unsigned char>(paletteColor[-3]);
        BLUR_COMPONENT(green, componentOffset - 4) = static_cast<unsigned char>(paletteColor[-2]);
        BLUR_COMPONENT(blue, componentOffset - 4) = static_cast<unsigned char>(paletteColor[-1]);
    } while (componentOffset < 0x400);

    resourceManager* resourceMgr;
    unsigned long lookupId = (resourceMgr = gpResourceManager)->MakeId("RGBLOOKP.BIN", 1);
    resourceMgr->PointToFile(lookupId);
    gpResourceManager->ReadBlock(reinterpret_cast<signed char*>(lookup), lookupSize);
    memcpy(destination->m_pixels, source->m_pixels, imageSize);
    PollSound();

    int lastRow = height - border;
    int y = border;
    if (y < lastRow) {
        int rowOffset = border * screenWidth;
        do {
            if ((y & 0x3f) == 0x3f) {
                PollSound();
            }
            int samples[13];
            int sample14;
            int sample13;
            int sample15;
            unsigned char* input = destination->m_pixels + rowOffset + border;
            int remaining = screenWidth - border * 2;
            unsigned char* output = source->m_pixels + rowOffset + border;
            do {
                samples[0] = static_cast<unsigned int>(input[screenWidth]) << 2;
                samples[1] = static_cast<unsigned int>(input[-screenWidth * 4]) << 2;
                samples[2] = static_cast<unsigned int>(input[screenWidth * 2]) << 2;
                samples[3] = static_cast<unsigned int>(input[-screenWidth * 2]) << 2;
                samples[4] = static_cast<unsigned int>(input[1]) << 2;
                samples[5] = static_cast<unsigned int>(input[-1]) << 2;
                samples[6] = static_cast<unsigned int>(input[2]) << 2;
                samples[7] = static_cast<unsigned int>(input[screenWidth * 3]) << 2;
                samples[8] = static_cast<unsigned int>(input[-2]) << 2;
                samples[9] = static_cast<unsigned int>(input[-screenWidth * 3]) << 2;
                samples[10] = static_cast<unsigned int>(input[3]) << 2;
                samples[11] = static_cast<unsigned int>(input[4]) << 2;
                samples[12] = static_cast<unsigned int>(input[-3]) << 2;
                sample13 = static_cast<unsigned int>(input[-4]) << 2;
                sample14 = static_cast<unsigned int>(input[-screenWidth]) << 2;
                sample15 = static_cast<unsigned int>(input[screenWidth * 4]) << 2;

                int redSum = BLUR_COMPONENT(red, sample15) + BLUR_COMPONENT(red, sample14);
                redSum += BLUR_COMPONENT(red, sample13);
                redSum += BLUR_COMPONENT(red, samples[12]);
                redSum += BLUR_COMPONENT(red, samples[11]);
                redSum += BLUR_COMPONENT(red, samples[10]);
                redSum += BLUR_COMPONENT(red, samples[9]);
                redSum += BLUR_COMPONENT(red, samples[8]);
                redSum += BLUR_COMPONENT(red, samples[7]);
                redSum += BLUR_COMPONENT(red, samples[6]);
                redSum += BLUR_COMPONENT(red, samples[5]);
                redSum += BLUR_COMPONENT(red, samples[4]);
                redSum += BLUR_COMPONENT(red, samples[3]);
                redSum += BLUR_COMPONENT(red, samples[2]);
                redSum += BLUR_COMPONENT(red, samples[0]);
                redSum += BLUR_COMPONENT(red, samples[1]);

                int greenSum = BLUR_COMPONENT(green, samples[1]) + BLUR_COMPONENT(green, sample13);
                greenSum += BLUR_COMPONENT(green, samples[12]);
                greenSum += BLUR_COMPONENT(green, samples[11]);
                greenSum += BLUR_COMPONENT(green, samples[10]);
                greenSum += BLUR_COMPONENT(green, samples[9]);
                greenSum += BLUR_COMPONENT(green, samples[8]);
                greenSum += BLUR_COMPONENT(green, samples[7]);
                greenSum += BLUR_COMPONENT(green, samples[6]);
                greenSum += BLUR_COMPONENT(green, samples[5]);
                greenSum += BLUR_COMPONENT(green, samples[4]);
                greenSum += BLUR_COMPONENT(green, samples[3]);
                greenSum += BLUR_COMPONENT(green, samples[2]);
                greenSum += BLUR_COMPONENT(green, sample14);
                greenSum += BLUR_COMPONENT(green, sample15);
                greenSum += BLUR_COMPONENT(green, samples[0]);

                int blueSum = BLUR_COMPONENT(blue, sample13) + BLUR_COMPONENT(blue, samples[12]);
                blueSum += BLUR_COMPONENT(blue, samples[11]);
                blueSum += BLUR_COMPONENT(blue, samples[10]);
                blueSum += BLUR_COMPONENT(blue, samples[9]);
                blueSum += BLUR_COMPONENT(blue, samples[8]);
                blueSum += BLUR_COMPONENT(blue, samples[7]);
                blueSum += BLUR_COMPONENT(blue, samples[6]);
                blueSum += BLUR_COMPONENT(blue, samples[5]);
                blueSum += BLUR_COMPONENT(blue, samples[4]);
                blueSum += BLUR_COMPONENT(blue, samples[3]);
                blueSum += BLUR_COMPONENT(blue, samples[2]);
                blueSum += BLUR_COMPONENT(blue, samples[1]);
                blueSum += BLUR_COMPONENT(blue, sample15);
                blueSum += BLUR_COMPONENT(blue, sample14);
                blueSum += BLUR_COMPONENT(blue, samples[0]);
                int lookupIndex = ((redSum & ~0x1f) << 5) + (greenSum & ~0x1f) + (blueSum >> 5);
                *output++ = lookup[lookupIndex];
                input++;
            } while (--remaining != 0);
            rowOffset += screenWidth;
            y++;
        } while (y < lastRow);
    }

    PollSound();
#line 139
    signed char* oldPalette = static_cast<signed char*>(BaseAlloc(paletteBytes, __FILE__, __LINE__));
#line 140
    signed char* newPalette = static_cast<signed char*>(BaseAlloc(paletteBytes, __FILE__, __LINE__));
    memcpy(oldPalette, gPalette->m_data, paletteBytes);
    signed char* oldColor = oldPalette;
    int remainingColors = componentCount;
    signed char* newColor = newPalette;
    do {
        newColor[0] = oldColor[0] + static_cast<signed char>(redAdjust);
        newColor[1] = oldColor[1] + static_cast<signed char>(greenAdjust);
        newColor[2] = oldColor[2] + static_cast<signed char>(blueAdjust);
        if (newColor[0] > 0x3f) {
            newColor[0] = 0x3f;
        }
        if (newColor[0] < 0) {
            newColor[0] = 0;
        }
        if (newColor[1] > 0x3f) {
            newColor[1] = 0x3f;
        }
        if (newColor[1] < 0) {
            newColor[1] = 0;
        }
        if (newColor[2] > 0x3f) {
            newColor[2] = 0x3f;
        }
        if (newColor[2] < 0) {
            newColor[2] = 0;
        }
        oldColor += 3;
        newColor += 3;
    } while (--remainingColors != 0);

    gpWindowManager->FizzleForward(0, 0, screenWidth, height, fizzleDelay, oldPalette, newPalette);
    DelayMilli(static_cast<long>(gfCombatSpeedMod[giCombatSpeed] * 350.0f));
    gpWindowManager->SaveFizzleSource(0, 0, screenWidth, height);
    memcpy(source->m_pixels, saved->m_pixels, imageSize);
    gpWindowManager->FizzleForward(0, 0, screenWidth, height, fizzleDelay, newPalette, oldPalette);
#line 168
    BaseFree(lookup, __FILE__, __LINE__);
    if (saved != 0) {
        delete saved;
    }
    gpMouseManager->ShowColorPointer();
#line 173
    BaseFree(oldPalette, __FILE__, __LINE__);
    BaseFree(newPalette, __FILE__, __LINE__);
}

#undef BLUR_COMPONENT
