#include <SOURCE/SaveRecords.h>
#include <SOURCE/game.h>
#include <SOURCE/ExpCampaign.h>

#include <algorithm>
#include <bit>
#include <cstdio>
#include <cstring>

// Only the production codecs are linked; these constructors avoid starting UI,
// resource, and map services while constructing their runtime destinations.
armyGroup::armyGroup() = default;
hero::hero() = default;
town::town() = default;
fullMap::fullMap() = default;
fullMap::~fullMap() = default;
ExpCampaign::ExpCampaign() = default;
ExpCampaign::~ExpCampaign() = default;

namespace {
int failures = 0;
void Check(bool value, const char* message) {
    if (!value) { std::fprintf(stderr, "%s\n", message); ++failures; }
}

template<class Record> Record Pattern() {
    Record bytes{};
    for (std::size_t i = 0; i < bytes.size(); ++i)
        bytes[i] = static_cast<u8>(i * 37 + 11);
    return bytes;
}

template<class Record, class Value, class Encoder, class Decoder>
void RoundTrip(const Record& bytes, Value& value, Encoder encode, Decoder decode) {
    Check(decode(bytes, value), "decode exact record");
    Check(encode(value) == bytes, "preserve every record byte");
    const auto expected = encode(value);
    for (std::size_t length = 0; length < bytes.size(); ++length) {
        Check(!decode(std::span{bytes}.first(length), value), "reject every truncated record");
        Check(encode(value) == expected, "truncated decode preserves destination");
    }
    std::array<u8, std::tuple_size_v<Record> + 1> oversized{};
    Check(!decode(oversized, value), "reject oversized record");
    Check(encode(value) == expected, "oversized decode preserves destination");
}
}

int main() {
    using namespace save_records;
    static_assert(alignof(hero) >= alignof(i32) && alignof(town) >= alignof(u32));
    static_assert(alignof(armyGroup) >= alignof(i16) && alignof(game) >= alignof(void*));
    static_assert(sizeof(Army) == 15 && sizeof(HeroBase) == 236 && sizeof(HeroExpansion) == 250);
    static_assert(sizeof(Town) == 100 && sizeof(Player) == 207 && sizeof(Campaign) == 327);
    static_assert(sizeof(Setup) == 65 && sizeof(ExpansionCampaign) == 79);

    armyGroup army;
    RoundTrip(Pattern<Army>(), army, EncodeArmy, DecodeArmy);
    hero h;
    auto heroBytes = Pattern<HeroExpansion>();
    // IEEE-754 -1.25, independent of the host's scalar byte order.
    heroBytes[232] = 0; heroBytes[233] = 0; heroBytes[234] = 0xa0; heroBytes[235] = 0xbf;
    RoundTrip(heroBytes, h, EncodeHeroExpansion, DecodeHeroExpansion);
    Check(h.m_aiFightValue == -1.25f, "decode IEEE-754 hero strength");
    h.m_spellPoints = -2;
    h.m_x = 0x12345678;
    h.m_locationType = static_cast<i16>(0x4321);
    const auto encodedHero = EncodeHeroBase(h);
    Check(encodedHero[0] == 0xfe && encodedHero[1] == 0xff, "encode signed 16-bit little endian");
    Check(encodedHero[25] == 0x78 && encodedHero[26] == 0x56
        && encodedHero[27] == 0x34 && encodedHero[28] == 0x12, "encode 32-bit little endian");
    Check(encodedHero[45] == 0x21 && encodedHero[46] == 0x43, "preserve open location code");
    HeroBase baseBytes{};
    std::copy_n(heroBytes.begin(), baseBytes.size(), baseBytes.begin());
    RoundTrip(baseBytes, h, EncodeHeroBase, DecodeHeroBase);
    Check(static_cast<u8>(h.m_artifactExtra[0]) == heroBytes[236]
        && static_cast<u8>(h.m_artifactExtra[13]) == heroBytes[249],
        "base hero leaves expansion-only state unchanged");

    town t;
    auto townBytes = Pattern<Town>();
    RoundTrip(townBytes, t, EncodeTown, DecodeTown);
    Check(static_cast<u8>(t.m_spellCounts[0]) == townBytes[79]
        && static_cast<u8>(static_cast<i8>(t.m_spellSlots[19])) == townBytes[79]
        && static_cast<u8>(t.m_spellCounts[5]) == townBytes[84], "preserve overlapping spell/count bytes");
    t.m_buildings = 0x87654321;
    Check(EncodeTown(t)[24] == 0x21 && EncodeTown(t)[27] == 0x87, "town building flags occupy four bytes");

    game g;
    g._pad_0xd2[119] = 'X';
    RoundTrip(Pattern<Campaign>(), g, EncodeCampaign, DecodeCampaign);
    Check(g._pad_0xd2[119] == 'X', "campaign leaves omitted reserved byte unchanged");
    g.m_campaignCarryoverCreatureTypes[0] = static_cast<i16>(0x1234);
    Check(EncodeCampaign(g)[186] == 0x34 && EncodeCampaign(g)[187] == 0x12,
        "serialize storage bits without narrowing through the enum underlying type");
    RoundTrip(Pattern<Setup>(), g, EncodeSetup, DecodeSetup);
    const auto setupBefore = EncodeSetup(g);
    Check(DecodeCampaign(Campaign{}, g) && EncodeCampaign(g) == Campaign{}
        && EncodeSetup(g) == setupBefore, "campaign reset changes only campaign fields");
    g.m_playerCount = 6;
    Check(DecodeSetup(Setup{}, g) && EncodeSetup(g) == Setup{} && g.m_playerCount == 6,
        "setup reset stops before live player state");
    RoundTrip(Pattern<Mine>(), g.m_mines[0], EncodeMine, DecodeMine);
    RoundTrip(Pattern<Boat>(), g.m_boats[0], EncodeBoat, DecodeBoat);

    ExpCampaign x;
    // Use an object address as an opaque identity; the codec must not touch it.
    x.m_window = reinterpret_cast<heroWindow*>(&g);
    const auto* windowBefore = x.m_window;
    auto expansionBytes = Pattern<ExpansionCampaign>();
    Check(DecodeExpansionCampaign(expansionBytes, x), "read legacy expansion pointer slot");
    Check(x.m_window == windowBefore, "never restore a pointer from a save");
    std::fill_n(expansionBytes.begin() + 71, 4, 0);
    Check(EncodeExpansionCampaign(x) == expansionBytes, "write zero in fixed four-byte pointer slot");
    RoundTrip(expansionBytes, x, EncodeExpansionCampaign, DecodeExpansionCampaign);

    playerData p;
    p.m_aiData.m_attentionWeights.heroValue = 19.5f;
    p.m_unusedSaveData[0] = 'P';
    p.m_aiData.m_obeliskValue = 76543;
    u8 cheated = 0;
    auto playerBytes = Pattern<Player>();
    Check(DecodePlayer(playerBytes, p, cheated), "read player record");
    Check(cheated == playerBytes[56], "read global cheated byte");
    Check(static_cast<u8>(p.m_barrierTents) == playerBytes[200], "second barrier byte wins");
    Check(p.m_aiData.m_attentionWeights.heroValue == 19.5f && p.m_unusedSaveData[0] == 'P'
        && p.m_aiData.m_obeliskValue == 76543, "leave unsaved AI/runtime fields unchanged");
    std::fill(playerBytes.begin() + 14, playerBytes.begin() + 56, 0);
    playerBytes[199] = playerBytes[200];
    Check(EncodePlayer(p, cheated) == playerBytes, "canonicalize reserved and duplicate player bytes");
    RoundTrip(playerBytes, p,
        [&](const playerData& value) { return EncodePlayer(value, cheated); },
        [&](std::span<const u8> bytes, playerData& value) { return DecodePlayer(bytes, value, cheated); });
    Check(failures == 0, "save record tests failed");
    return failures != 0;
}
