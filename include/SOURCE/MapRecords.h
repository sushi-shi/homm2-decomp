#ifndef HOMM2_SOURCE_MAP_RECORDS_H
#define HOMM2_SOURCE_MAP_RECORDS_H

#include <Ints.h>
#include <EDITOR/mapcell.h>
#include <span>
#include <string_view>

namespace map_records {

using Record = std::span<const u8>;
enum class Kind { Sign, Rumour, Sphinx, TimeEvent, MapEvent, Hero, Jail, Town };
constexpr i32 ExtraCapacity = 65536; // Stored references are unsigned 16-bit indices.

// All functions inspect only the supplied spans; nullptr means valid.
const char* RecordError(Record record, Kind kind);
std::string_view Text(Record record, std::size_t offset);
const char* CellDataError(std::span<const mapCell> cells,
                          std::span<const mapCellExtra> extras);
const char* ExtraTableError(std::span<const mapCell> cells,
                            std::span<const Record> records,
                            std::span<const u16> rumours,
                            std::span<const u16> timeEvents,
                            std::span<const u16> mapEvents, bool newMap);

}

// Runtime adapters preserve the recorded allocation bounds and report invalid data.
void* MapExtraRecord(i32 index, map_records::Kind kind);
std::string_view MapExtraText(i32 index, std::size_t offset);
std::string_view MapExtraText(const void* record, std::size_t offset);

#endif
