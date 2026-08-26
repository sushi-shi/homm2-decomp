#include <IRONFIST/runtime.h>

#include <cstring>
#include <string>

#include <IRONFIST/artifacts.h>
#include <IRONFIST/callback.h>
#include <IRONFIST/campaigns.h>
#include <IRONFIST/creatures.h>
#include <IRONFIST/state.h>
#include <IRONFIST/scripting.h>
#include <IRONFIST/townconsts.h>

#include <SOURCE/game.h>
#include <SOURCE/KB.h>

namespace ironfist::runtime {

void Initialize() {
    LoadCreatures();
    LoadArtifacts();
    InitializeTownConstants();
    InitializeCampaigns();
}

void ResetAdventureState() {
    state::Get().adventure = state::AdventureState();
}

void BeginMap(const char* filename) {
    ResetAdventureState();
    std::string mapName(filename);
    script::InitializeMap(mapName);
}

void FinishMap() {
    // Upstream resets this after the retail NewMap body, so a map's top-level
    // script cannot change it. Preserve that ordering.
    state::Get().adventure.allowAIArmySharing = true;
}

void AdventureManagerReady() {
    state::AdventureState& adventure = state::Get().adventure;
    if (!std::strcmp(gpGame->m_saveName, "NEWGAME") && !adventure.firstDayEventDone) {
        script::Invoke("OnMapStart");
        script::Invoke(
            "OnNewDay", static_cast<i32>(gpGame->m_month),
            static_cast<i32>(gpGame->m_week), static_cast<i32>(gpGame->m_day)
        );
        adventure.firstDayEventDone = true;
    }
}

void Shutdown() {
    UnloadCreatures();
    script::Shutdown();
}

} // namespace ironfist::runtime
