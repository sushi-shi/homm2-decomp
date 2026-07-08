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
// @early-stop 4% fuzzy — body is structurally exact (base-init, inline strcpy+_strrev,
// hoisted esi=0x2b11/edi=0x5622, correct case bodies + tail). The switch jump table
// desyncs objdiff: our fresh COFF relocates jump-table entries to compiler-local labels
// ($L1268), the delinked retail expresses them as ??0sample+offset — the reloc targets
// never match so objdiff drops alignment for the whole tail. Correct source, tooling wall.
VA(0x004dad60, 0x181)
sample::sample(char *param_1, long int param_2, long int param_3, long int param_4)
    : resource(6, gpResourceManager->MakeId(param_1, 1), 1, 0)
{
    char local_20[32];
    int iVar7 = 2;
    field_0x1c = param_2;
    field_0x28 = param_3;
    field_0x2c = param_4;
    int hz11 = 0x2b11;   // 11025 Hz  — kept live across the loop (hoisted to esi)
    int hz22 = 0x5622;   // 22050 Hz  — hoisted to edi
    // The last three chars of the filename (extension, e.g. ".82M") encode the
    // sample format: reverse the name, then read them front-to-back.
    strcpy(local_20, param_1);
    _strrev(local_20);
    int iVar6 = 0;
    do {
        switch (local_20[iVar6]) {
        case '1': m_sampleRate = hz11; break;     // 11025 Hz
        case '2': m_sampleRate = hz22; break;     // 22050 Hz
        case '4': m_sampleRate = 0xac44; break;   // 44100 Hz
        case '6': m_format = 1; break;        // 16-bit
        case '8': m_format = 0; break;        // 8-bit
        case 'M':
        case 'm': iVar7 = 0; break;             // mono
        }
        iVar6 = iVar6 + 1;
    } while (iVar6 < 3);
    m_format = m_format + iVar7;
    unsigned long size = gpResourceManager->GetFileSize(m_id);
    m_data = static_cast<char *>(BaseAlloc(size, __FILE__, __LINE__));
    m_size = size;
    gpResourceManager->PointToFile(m_id);
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_data), size);
}

VA(0x004daf40, 0x2c)
sample::~sample()
{
    BaseFree(m_data, __FILE__, __LINE__);
    m_data = 0;
    m_size = 0;
    field_0x28 = 0;
}

VA(0x004daf70, 0x72)
MIDIWrap::MIDIWrap(char *name) : resource(6, gpResourceManager->MakeId(name, 1), 1, 0)
{
    unsigned long size = gpResourceManager->GetFileSize(m_id);
    m_data = static_cast<char *>(BaseAlloc(size, __FILE__, __LINE__));
    gpResourceManager->PointToFile(m_id);
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_data), size);
}

VA(0x004db030, 0x28)
MIDIWrap::~MIDIWrap()
{
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
