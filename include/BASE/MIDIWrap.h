#ifndef HOMM2_BASE_MIDIWRAP_H
#define HOMM2_BASE_MIDIWRAP_H

#include <va.h>
#include <BASE/resource.h>

#pragma pack(push, 1)
class MIDIWrap : public resource {
public:
    char* m_data;
    MIDIWrap(H2_CONST char*);
    virtual H2_RETAIL_INLINE ~MIDIWrap() OVERRIDE;
};
#pragma pack(pop)
SIZE(MIDIWrap, 0x14);
#endif
