#include <IMPORT/LegacySave.h>
#include <BASE/Bzip2.h>

#include <cstdio>
#include <fstream>
#include <iterator>

int main(int argc, char** argv) {
    if (argc != 2) return 1;
    const char* files[] = {"legacy-base.GM2.bz2", "legacy-gold.GX2.bz2",
                           "legacy-campaign.GMC.bz2", "legacy-expansion.GXC.bz2"};
    for (int kind = 0; kind < 4; ++kind) {
        std::ifstream input(std::string(argv[1]) + "/" + files[kind], std::ios::binary);
        const std::vector<u8> compressed{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
        std::vector<u8> legacy(65536);
        u32 size{};
        if (!compression::Bzip2Decompress(legacy.data(), static_cast<u32>(legacy.size()),
                compressed.data(), static_cast<u32>(compressed.size()), size)) return 2;
        legacy.resize(size);
        savegame::Snapshot state;
        std::string error;
        if (!legacy_save::Decode(legacy, localization::TextEncoding::Windows1251, state, error)) {
            std::fprintf(stderr, "%s: %s\n", files[kind], error.c_str());
            return 3;
        }
        const auto expectedKind = kind == 2 ? savegame::Kind::Campaign
            : kind == 3 ? savegame::Kind::ExpansionCampaign : savegame::Kind::Scenario;
        if (state.kind != expectedKind || state.expansionMap != (kind == 1 || kind == 3)
            || state.heroes[0].m_name != "Александр" || state.towns[0].m_name != "Белокаменный"
            || state.playerNames[0] != "Александр" || state.playerNames[1] != "Second player"
            || state.rumour != "Привет из старого сохранения" || state.playerSystemIds[0] != "abc"
            || state.heroes[0].m_army.m_creatureCounts[0] != 1234 || state.heroes[0].m_spellPoints != 123
            || state.heroes[0].m_artifactExtra[13] != ((kind == 1 || kind == 3) ? 7 : 0)
            || state.towns[0].m_spellCounts[5] != 4
            || state.players[0].m_resources[H2EnumIndex(RES_GOLD)] != 23456
            || state.currentPlayer != 1 || !state.human[0] || !state.human[1]
            || state.m_day != 7 || state.m_week != 4 || state.m_month != 10
            || state.cells[0].m_extraIndex != 1 || state.overlays[1].objectIndex != 17
            || state.fog[0] != 3 || state.events[1] != std::vector<u8>({0,0,'m','a','p',0})) {
            std::fprintf(stderr, "%s: imported state differs from the old writer's fixture\n", files[kind]);
            return 4;
        }
        if (kind == 2 && (state.m_campaignScenario != 3 || state.m_campaignScenarioDays[0][3] != 99)) return 5;
        if (kind == 3 && (state.expansion.campaignId != 1 || state.expansion.currentMap != 3
                         || state.expansion.mapCount != 8 || state.expansion.awards[10] != 1)) return 6;
        std::vector<u8> native;
        if (!savegame::Encode(state, native, error)) return 7;
        savegame::Snapshot reloaded;
        if (!savegame::Decode(native, reloaded, error)
            || reloaded.heroes[0].m_name != state.heroes[0].m_name
            || reloaded.towns[0].m_name != state.towns[0].m_name) return 8;

        for (std::size_t length : {std::size_t{0}, std::size_t{4}, std::size_t{8}, std::size_t{420},
                                   std::size_t{500}, std::size_t{800}, std::size_t{1000}, legacy.size() - 1}) {
            if (legacy_save::Decode(std::span(legacy).first(length), localization::TextEncoding::Windows1251,
                                    reloaded, error) || reloaded.heroes[0].m_name != "Александр") return 9;
        }
        legacy.back() ^= 1;
        if (legacy_save::Decode(legacy, localization::TextEncoding::Windows1251, reloaded, error)) return 10;
    }
    std::puts("base, expansion and campaign legacy fixtures import without losing UTF-8 names or state");
    return 0;
}
