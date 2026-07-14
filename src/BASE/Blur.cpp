// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Blur.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#define HOMM2_MISC_INLINE_ICONENTRY
#include <va.h>
#include <BASE/Blur.h>
#include <BASE/bitmap.h>
#include <BASE/palette.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#undef HOMM2_MISC_INLINE_ICONENTRY
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/X_GLOBAL.h>
#include <string.h>

DATA(0x004fa954) static int giCombatSpeed;

#define BLUR_COMPONENT(table, offset)                                                              \
    (*reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(table) + (offset)))
#define BLUR_COMPONENT_CHANNELS 3
#define BLUR_COMPONENT_MAXIMUM 0x3f
#define BLUR_COMPONENT_TABLE_BYTES 0x400
#define BLUR_QUANTIZATION_SHIFT 5
#define BLUR_QUANTIZATION_MASK ((1 << BLUR_QUANTIZATION_SHIFT) - 1)
#define BLUR_SOUND_POLL_MASK 0x3f

// @match-note
// Coverage-phase structural checkpoint, not a proven wall. The complete CFG saves the source,
// loads the RGB quantization table, applies the 16-sample blur to rows 4..height-5 and columns
// 4..635, builds a clamped adjusted palette, performs both fizzles, restores the source, and
// releases all three explicit-provenance allocations. Both sides reserve 0xc5c bytes and agree on
// the saved bitmap (+0x10), row/y (+0x14/+0x18), sample spills (+0x20..+0x50), output/count
// (+0x54/+0x64), and red/green/blue tables (+0x6c/+0x46c/+0x86c). First divergence is +0xb0:
// ours loads gpBufferPalette before spilling the lookup pointer; retail spills it first and then
// loads gpBufferPalette. Ours ends at +0x6aa versus retail +0x6a4. All 43 relocation identities
// and their order agree; retail's delinked names at 0x4fa954 and for 350.0f were audited manually.
// Correct channel-table declaration order, retail channel-addition order, named algorithm
// constants, and direct resource/palette aliases were tried. Revisit after a reachable TU-state
// change or in the post-coverage last-mile phase; no permutation tool was used.
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

    unsigned char* lookup = static_cast<unsigned char*>(
        H2_ALLOC(lookupSize, "I:\\Projects\\Heroes\\Prog\\BASE\\Blur.cpp", 0x19)
    );
    int red[componentCount];
    int blue[componentCount];
    int green[componentCount];
    signed char* paletteColor = gpBufferPalette->m_data;
    int componentOffset = 0;
    do {
        componentOffset += sizeof(unsigned int);
        paletteColor += BLUR_COMPONENT_CHANNELS;
        BLUR_COMPONENT(red, componentOffset - 4) = static_cast<unsigned char>(paletteColor[-3]);
        BLUR_COMPONENT(green, componentOffset - 4) = static_cast<unsigned char>(paletteColor[-2]);
        BLUR_COMPONENT(blue, componentOffset - 4) = static_cast<unsigned char>(paletteColor[-1]);
    } while (componentOffset < BLUR_COMPONENT_TABLE_BYTES);

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
            if ((y & BLUR_SOUND_POLL_MASK) == BLUR_SOUND_POLL_MASK) {
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

                int redSum = BLUR_COMPONENT(red, samples[1]) + BLUR_COMPONENT(red, samples[0]);
                redSum += BLUR_COMPONENT(red, samples[2]);
                redSum += BLUR_COMPONENT(red, samples[3]);
                redSum += BLUR_COMPONENT(red, samples[4]);
                redSum += BLUR_COMPONENT(red, samples[5]);
                redSum += BLUR_COMPONENT(red, samples[6]);
                redSum += BLUR_COMPONENT(red, samples[7]);
                redSum += BLUR_COMPONENT(red, samples[8]);
                redSum += BLUR_COMPONENT(red, samples[9]);
                redSum += BLUR_COMPONENT(red, samples[10]);
                redSum += BLUR_COMPONENT(red, samples[11]);
                redSum += BLUR_COMPONENT(red, samples[12]);
                redSum += BLUR_COMPONENT(red, sample13);
                redSum += BLUR_COMPONENT(red, sample14);
                redSum += BLUR_COMPONENT(red, sample15);

                int greenSum = BLUR_COMPONENT(green, samples[0]) + BLUR_COMPONENT(green, sample15);
                greenSum += BLUR_COMPONENT(green, sample14);
                greenSum += BLUR_COMPONENT(green, samples[2]);
                greenSum += BLUR_COMPONENT(green, samples[3]);
                greenSum += BLUR_COMPONENT(green, samples[4]);
                greenSum += BLUR_COMPONENT(green, samples[5]);
                greenSum += BLUR_COMPONENT(green, samples[6]);
                greenSum += BLUR_COMPONENT(green, samples[7]);
                greenSum += BLUR_COMPONENT(green, samples[8]);
                greenSum += BLUR_COMPONENT(green, samples[9]);
                greenSum += BLUR_COMPONENT(green, samples[10]);
                greenSum += BLUR_COMPONENT(green, samples[11]);
                greenSum += BLUR_COMPONENT(green, samples[12]);
                greenSum += BLUR_COMPONENT(green, sample13);
                greenSum += BLUR_COMPONENT(green, samples[1]);

                int blueSum = BLUR_COMPONENT(blue, samples[0]) + BLUR_COMPONENT(blue, sample14);
                blueSum += BLUR_COMPONENT(blue, sample15);
                blueSum += BLUR_COMPONENT(blue, samples[1]);
                blueSum += BLUR_COMPONENT(blue, samples[2]);
                blueSum += BLUR_COMPONENT(blue, samples[3]);
                blueSum += BLUR_COMPONENT(blue, samples[4]);
                blueSum += BLUR_COMPONENT(blue, samples[5]);
                blueSum += BLUR_COMPONENT(blue, samples[6]);
                blueSum += BLUR_COMPONENT(blue, samples[7]);
                blueSum += BLUR_COMPONENT(blue, samples[8]);
                blueSum += BLUR_COMPONENT(blue, samples[9]);
                blueSum += BLUR_COMPONENT(blue, samples[10]);
                blueSum += BLUR_COMPONENT(blue, samples[11]);
                blueSum += BLUR_COMPONENT(blue, samples[12]);
                blueSum += BLUR_COMPONENT(blue, sample13);
                int lookupIndex = ((redSum & ~BLUR_QUANTIZATION_MASK) << BLUR_QUANTIZATION_SHIFT)
                                  + (greenSum & ~BLUR_QUANTIZATION_MASK)
                                  + (blueSum >> BLUR_QUANTIZATION_SHIFT);
                *output++ = lookup[lookupIndex];
                input++;
            } while (--remaining != 0);
            rowOffset += screenWidth;
            y++;
        } while (y < lastRow);
    }

    PollSound();
    signed char* oldPalette = static_cast<signed char*>(
        H2_ALLOC(paletteBytes, "I:\\Projects\\Heroes\\Prog\\BASE\\Blur.cpp", 0x8b)
    );
    signed char* newPalette = static_cast<signed char*>(
        H2_ALLOC(paletteBytes, "I:\\Projects\\Heroes\\Prog\\BASE\\Blur.cpp", 0x8c)
    );
    memcpy(oldPalette, gPalette->m_data, paletteBytes);
    signed char* oldColor = oldPalette;
    int remainingColors = componentCount;
    signed char* newColor = newPalette;
    do {
        newColor[0] = oldColor[0] + static_cast<signed char>(redAdjust);
        newColor[1] = oldColor[1] + static_cast<signed char>(greenAdjust);
        newColor[2] = oldColor[2] + static_cast<signed char>(blueAdjust);
        if (newColor[0] > BLUR_COMPONENT_MAXIMUM) {
            newColor[0] = BLUR_COMPONENT_MAXIMUM;
        }
        if (newColor[0] < 0) {
            newColor[0] = 0;
        }
        if (newColor[1] > BLUR_COMPONENT_MAXIMUM) {
            newColor[1] = BLUR_COMPONENT_MAXIMUM;
        }
        if (newColor[1] < 0) {
            newColor[1] = 0;
        }
        if (newColor[2] > BLUR_COMPONENT_MAXIMUM) {
            newColor[2] = BLUR_COMPONENT_MAXIMUM;
        }
        if (newColor[2] < 0) {
            newColor[2] = 0;
        }
        oldColor += BLUR_COMPONENT_CHANNELS;
        newColor += BLUR_COMPONENT_CHANNELS;
    } while (--remainingColors != 0);

    gpWindowManager->FizzleForward(0, 0, screenWidth, height, fizzleDelay, oldPalette, newPalette);
    DelayMilli(static_cast<long>(gfCombatSpeedMod[giCombatSpeed] * 350.0f));
    gpWindowManager->SaveFizzleSource(0, 0, screenWidth, height);
    memcpy(source->m_pixels, saved->m_pixels, imageSize);
    gpWindowManager->FizzleForward(0, 0, screenWidth, height, fizzleDelay, newPalette, oldPalette);
    H2_FREE(lookup, "I:\\Projects\\Heroes\\Prog\\BASE\\Blur.cpp", 0xa8);
    if (saved != 0) {
        delete saved;
    }
    gpMouseManager->ShowColorPointer();
    H2_FREE(oldPalette, "I:\\Projects\\Heroes\\Prog\\BASE\\Blur.cpp", 0xad);
    H2_FREE(newPalette, "I:\\Projects\\Heroes\\Prog\\BASE\\Blur.cpp", 0xae);
}

#undef BLUR_COMPONENT
#undef BLUR_COMPONENT_CHANNELS
#undef BLUR_COMPONENT_MAXIMUM
#undef BLUR_COMPONENT_TABLE_BYTES
#undef BLUR_QUANTIZATION_SHIFT
#undef BLUR_QUANTIZATION_MASK
#undef BLUR_SOUND_POLL_MASK
