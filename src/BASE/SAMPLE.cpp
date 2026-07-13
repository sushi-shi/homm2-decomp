// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\SAMPLE.OBJ   from: .\basewin.lib
// functions: 8   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/MIDIWrap.h>
#include <BASE/sample.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <string.h>
// @early-stop
// base/retail .text are both 0x181. Relocation-masked raw comparison differs in
// exactly 27 bytes at +0x24..+0x25, +0x29..+0x39, +0x43..+0x48, and +0x4b..+0x4c:
// base hoists EBP=2 across the resource constructor and schedules strcpy's EAX zero
// after the volume store; retail schedules both after the argument reloads. Bytes
// +0x4d..+0x180 are identical, including the switch table and load tail. Relocations
// resolve base 7/retail 23, only-base=0; the extra retail entries are local jump-table
// aliases. Moving/splitting formatFlags initialization and all member-store orders
// were exhausted; later placement regresses the otherwise exact loop/tail.
VA(0x004dad60, 0x181)
sample::sample(char *name, long channelType, long volume, long loopCount)
    : resource(6, gpResourceManager->MakeId(name, 1), 1, 0)
{
    int formatFlags;
    formatFlags = SAMPLE_FORMAT_STEREO;
    m_channelType = channelType;
    m_volume = volume;
    m_loopCount = loopCount;

    char filename[32];
    strcpy(filename, name);
    _strrev(filename);

    for (int i = 0; i < 3; i++) {
        switch (filename[i]) {
        case '1':
            m_sampleRate = SAMPLE_RATE_11025;
            break;
        case '2':
            m_sampleRate = SAMPLE_RATE_22050;
            break;
        case '4':
            m_sampleRate = SAMPLE_RATE_44100;
            break;
        case '6':
            m_format = SAMPLE_FORMAT_16_BIT;
            break;
        case '8':
            m_format = SAMPLE_FORMAT_8_BIT;
            break;
        case 'M':
        case 'm':
            formatFlags = 0;
            break;
        }
    }
    m_format += formatFlags;

    unsigned long size = gpResourceManager->GetFileSize(m_id);
#line 57
    m_data = static_cast<char *>(BaseAlloc(size, __FILE__, __LINE__));
    m_size = size;
    gpResourceManager->PointToFile(m_id);
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_data), size);
}

// @early-stop
// Applies to compiler-generated aliases at 0x004daef0: base emits ??_G, while the
// delinker exposes two duplicate ??_E symbols for the retail ??_E/??_G CodeView aliases.
// Comparing base ??_G to either retail ??_E: .text 0x41/0x41, relocation-masked raw
// diffs=0, manual relocation targets 5/5. `homm2 relocs` reports 0/10 solely because
// it looks for absent base ??_E and combines both duplicate retail ??_E bodies.
VA(0x004daf40, 0x2c)
inline sample::~sample()
{
#line 97
    BaseFree(m_data, __FILE__, __LINE__);
    m_data = 0;
    m_size = 0;
    m_volume = 0;
}

VA(0x004daf70, 0x72)
MIDIWrap::MIDIWrap(char *name) : resource(6, gpResourceManager->MakeId(name, 1), 1, 0)
{
    unsigned long size = gpResourceManager->GetFileSize(m_id);
#line 110
    m_data = static_cast<char *>(BaseAlloc(size, __FILE__, __LINE__));
    gpResourceManager->PointToFile(m_id);
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_data), size);
}

// @early-stop
// Applies to compiler-generated aliases at 0x004daff0: base emits ??_G, while the
// delinker exposes two duplicate ??_E symbols for the retail ??_E/??_G CodeView aliases.
// Comparing base ??_G to either retail ??_E: .text 0x3d/0x3d, relocation-masked raw
// diffs=0, manual relocation targets 5/5. `homm2 relocs` reports 0/10 solely because
// it looks for absent base ??_E and combines both duplicate retail ??_E bodies.
VA(0x004db030, 0x28)
inline MIDIWrap::~MIDIWrap()
{
#line 118
    BaseFree(m_data, __FILE__, __LINE__);
    m_data = 0;
}


// ===== vtable MIDIWrap (root)  (1 slots) =====
//  [ 0] VA(0x004daff0, 0x3d)  void * MIDIWrap::scalar_dtor(unsigned int)   <- introduces virtual

// ===== vtable sample (root)  (1 slots) =====
//  [ 0] VA(0x004daef0, 0x41)  void * sample::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(sample, 0x004ebab4);
VTBL(MIDIWrap, 0x004ebab8);
