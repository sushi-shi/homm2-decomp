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
SessionPhase sessionPhase = SessionPhase::Idle;
}

SessionPhase Phase() {
    return sessionPhase;
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
    sessionPhase = SessionPhase::PreparingMap;
    std::string mapName(filename);
    script::InitializeMap(mapName);
}

void AdventureManagerReady() {
    state::AdventureState& adventure = state::Get().adventure;
    if (sessionPhase == SessionPhase::PreparingMap && !adventure.firstDayEventDone) {
        sessionPhase = SessionPhase::Ready;
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
    sessionPhase = SessionPhase::Idle;
}

void BeginSessionLoad() {
    script::Shutdown();
    state::Get().combat.EndBattle();
    ResetAdventureState();
    sessionPhase = SessionPhase::Restoring;
}

void FinishSessionLoad() {
    sessionPhase = SessionPhase::Ready;
}

} // namespace ironfist::runtime
