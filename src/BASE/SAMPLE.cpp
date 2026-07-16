// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\SAMPLE.OBJ   from: .\basewin.lib
// functions: 8   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/MIDIWrap.h>
#include <BASE/sample.h>
#include <BASE/SAMPLE_TYPES.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <string.h>

// @data-layout-note Retail stores four distinct writable source-file copies in one
// 0xa0-byte interval at RVA 0x120df4. The sample allocation/destruction and MIDI
// allocation/destruction call sites reference this owner at +0x00/+0x28/+0x50/+0x78.
// Independent function-local arrays put both constructor copies before the inline-
// destructor COMDATs, which conflicts with the retail order. This record emits one
// section whose complete payload is byte-exact against the retail interval.
DATA(0x00520df4) static SSampleSourceFiles gSampleSourceFiles = {
    SAMPLE_SOURCE_FILE,
    SAMPLE_SOURCE_FILE,
    SAMPLE_SOURCE_FILE,
    SAMPLE_SOURCE_FILE
};

// @early-stop
// The explicit 0x181-byte CodeView range is raw-exact after relocation-union masking;
// retail's enclosing row has three trailing padding bytes. Frame/slots and CFG are
// exact, as are all 13 external targets/addends. The remaining ten of 23 ordered sites
// are dispatch/table locals at +0x94/+0x9b and +0x124..+0x143 (the table is recorded in
// build/gen/jump_tables.csv): MSVC emits $L symbols, while the delinker rewrites them
// as this constructor plus the same local offsets.
VA(0x004dad60, 0x181)
sample::sample(char *name, long channelType, long volume, long loopCount)
    : resource(6, gpResourceManager->MakeId(name, 1), 1, 0)
{
    int formatFlags;
    m_channelType = channelType;
    m_volume = volume;
    m_loopCount = loopCount;
    formatFlags = SAMPLE_FORMAT_STEREO;

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
    m_data = static_cast<char *>(
        H2_ALLOC(size, gSampleSourceFiles.sampleAllocation, 0x39));
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
    H2_FREE(m_data, gSampleSourceFiles.sampleDestruction, 0x61);
    m_data = 0;
    m_size = 0;
    m_volume = 0;
}

VA(0x004daf70, 0x72)
MIDIWrap::MIDIWrap(char *name) : resource(6, gpResourceManager->MakeId(name, 1), 1, 0)
{
    unsigned long size = gpResourceManager->GetFileSize(m_id);
#line 110
    m_data = static_cast<char *>(
        H2_ALLOC(size, gSampleSourceFiles.midiAllocation, 0x6e));
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
    H2_FREE(m_data, gSampleSourceFiles.midiDestruction, 0x76);
    m_data = 0;
}


// ===== vtable MIDIWrap (root)  (1 slots) =====
//  [ 0] VA(0x004daff0, 0x3d)  void * MIDIWrap::scalar_dtor(unsigned int)   <- introduces virtual

// ===== vtable sample (root)  (1 slots) =====
//  [ 0] VA(0x004daef0, 0x41)  void * sample::scalar_dtor(unsigned int)   <- introduces virtual

// ---- vtables (compiler-emitted; census) ----
VTBL(sample, 0x004ebab4);
VTBL(MIDIWrap, 0x004ebab8);
