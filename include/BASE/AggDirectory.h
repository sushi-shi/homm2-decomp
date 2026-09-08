#ifndef HOMM2_BASE_AGG_DIRECTORY_H
#define HOMM2_BASE_AGG_DIRECTORY_H

#include <Ints.h>
#include <PLATFORM/FileSystem.h>

#include <string>
#include <vector>

namespace resources {

// Runtime metadata decoded from the three little-endian words on disk.
struct AggEntry {
    u32 id;
    i32 offset;
    u32 size;
};

bool ReadAggDirectory(platform::IFileSystem& files, i32 file,
                      std::vector<AggEntry>& entries, std::string& error);
bool ReadAggMember(platform::IFileSystem& files, i32 file, const AggEntry& entry,
                   void* destination, u32 size);

}

#endif
