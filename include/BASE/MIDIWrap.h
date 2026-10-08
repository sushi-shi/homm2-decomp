#ifndef HOMM2_BASE_MIDIWRAP_H
#define HOMM2_BASE_MIDIWRAP_H

#include <H2/Ints.h>
#include <Domains.h>
#include <BASE/resource.h>

#pragma pack(push, 1)
class MIDIWrap : public resource {
public:
    char* m_data;
    MIDIWrap(const char* name);
    virtual  ~MIDIWrap() override;
};
#pragma pack(pop)
#endif
