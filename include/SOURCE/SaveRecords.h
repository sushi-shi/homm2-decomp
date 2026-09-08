#ifndef HOMM2_SOURCE_SAVERECORDS_H
#define HOMM2_SOURCE_SAVERECORDS_H

#include <Ints.h>
#include <array>
#include <span>

class armyGroup;
class hero;
class town;
class playerData;
class game;
class ExpCampaign;
struct mineRecord;
struct boatRecord;

// Retail little-endian records. Their lengths never depend on runtime padding,
// pointer size, or sizeof a game object. Text is already in the file encoding.
namespace save_records {
using Army = std::array<u8, 15>;
using HeroBase = std::array<u8, 236>;
using HeroExpansion = std::array<u8, 250>;
using Town = std::array<u8, 100>;
using Player = std::array<u8, 207>;
using Campaign = std::array<u8, 327>;
using Setup = std::array<u8, 65>;
using ExpansionCampaign = std::array<u8, 79>;
using Mine = std::array<u8, 7>;
using Boat = std::array<u8, 8>;

Army EncodeArmy(const armyGroup& value);
HeroBase EncodeHeroBase(const hero& value);
HeroExpansion EncodeHeroExpansion(const hero& value);
Town EncodeTown(const town& value);
Player EncodePlayer(const playerData& value, u8 cheated);
Campaign EncodeCampaign(const game& value);
Setup EncodeSetup(const game& value);
ExpansionCampaign EncodeExpansionCampaign(const ExpCampaign& value);
Mine EncodeMine(const mineRecord& value);
Boat EncodeBoat(const boatRecord& value);

// Exact length is checked before changing the destination. These codecs retain
// open numeric codes; callers validate text and gameplay domains at their boundary.
bool DecodeArmy(std::span<const u8> bytes, armyGroup& value);
bool DecodeHeroBase(std::span<const u8> bytes, hero& value);
bool DecodeHeroExpansion(std::span<const u8> bytes, hero& value);
bool DecodeTown(std::span<const u8> bytes, town& value);
bool DecodePlayer(std::span<const u8> bytes, playerData& value, u8& cheated);
bool DecodeCampaign(std::span<const u8> bytes, game& value);
bool DecodeSetup(std::span<const u8> bytes, game& value);
bool DecodeExpansionCampaign(std::span<const u8> bytes, ExpCampaign& value);
bool DecodeMine(std::span<const u8> bytes, mineRecord& value);
bool DecodeBoat(std::span<const u8> bytes, boatRecord& value);
}
#endif
