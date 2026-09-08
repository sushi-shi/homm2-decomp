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

namespace {
SessionPhase gSessionPhase = SessionPhase::SESSION_IDLE;
}

SessionPhase Phase() {
    return gSessionPhase;
}

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
    state::Get().combat.EndBattle();
    ResetAdventureState();
    gSessionPhase = SessionPhase::SESSION_PREPARING_MAP;
    std::string mapName(filename);
    script::InitializeMap(mapName);
}

void AdventureManagerReady() {
    state::AdventureState& adventure = state::Get().adventure;
    if (gSessionPhase == SessionPhase::SESSION_PREPARING_MAP && !adventure.firstDayEventDone) {
        gSessionPhase = SessionPhase::SESSION_READY;
        adventure.firstDayEventDone = true;
        script::Invoke("OnMapStart");
        script::Invoke(
            "OnNewDay", static_cast<i32>(gpGame->m_month),
            static_cast<i32>(gpGame->m_week), static_cast<i32>(gpGame->m_day)
        );
    }
}

void Shutdown() {
    script::Shutdown();
    UnloadCreatures();
    ResetGeneratedArtifacts();
    state::Get() = state::State{};
    gSessionPhase = SessionPhase::SESSION_IDLE;
}

void BeginSessionLoad() {
    script::Shutdown();
    state::Get().combat.EndBattle();
    ResetAdventureState();
    gSessionPhase = SessionPhase::SESSION_RESTORING;
}

void FinishSessionLoad() {
    gSessionPhase = SessionPhase::SESSION_READY;
}

} // namespace ironfist::runtime
