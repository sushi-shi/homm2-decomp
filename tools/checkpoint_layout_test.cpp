#include <BASE/message.h>
#include <SOURCE/COMMAND.h>
#include <SOURCE/CONFIG_TYPES.h>
#include <SOURCE/REMOTE_SAVE.h>
#include <SOURCE/town.h>

#include <cstddef>
#include <type_traits>

// The checkpoint replaces overlapping aliases with their actual owners.
// Keep the wire/save footprint while checking the new array boundaries.
static_assert(sizeof(RemoteSaveInitialization) == 16);
static_assert(offsetof(RemoteSaveInitialization, playerExited) == 12);
static_assert(sizeof(RemoteSaveBuffer) == 256);
static_assert(sizeof(RemoteSaveMessage) == 256);
static_assert(sizeof(CombatRemotePacket) == 256);
static_assert(offsetof(RemoteSaveMessage, payload) == 9);
static_assert(offsetof(CombatRemotePacket, text) == 9);
static_assert(offsetof(CombatRemotePacket, nextAction) == 9);
static_assert(offsetof(RemoteSaveChunk, data) == 2);
static_assert(REMOTE_SAVE_CHUNK_SIZE <= REMOTE_SAVE_CHUNK_CAPACITY);
static_assert(offsetof(configStruct, walkSpeeds) == 0);
static_assert(offsetof(configStruct, musicVolume) == 8);
static_assert(std::extent_v<decltype(configStruct::walkSpeeds)> == 2);
static_assert(std::extent_v<decltype(town::m_spellCounts)> == 5);
static_assert(offsetof(town, m_spellCounts) == offsetof(town, m_spells) + 20);
static_assert(offsetof(town, m_turnsOwned) == offsetof(town, m_spellCounts) + 5);
static_assert(static_cast<i32>(WIDGET_NOTIFY_SELECT) == 12);
static_assert(static_cast<i32>(WIDGET_NOTIFY_DESELECT) == 13);
static_assert(static_cast<i32>(WIDGET_NOTIFY_RIGHT_CLICK) == 14);
static_assert(sizeof(MessageType) == 4 && sizeof(BaseWidgetCommand) == 4);
static_assert(static_cast<i32>(MESSAGE_TEXT_INPUT) == 0x80);

int main() {}
