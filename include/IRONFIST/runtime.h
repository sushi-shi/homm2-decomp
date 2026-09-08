#ifndef HOMM2_IRONFIST_RUNTIME_H
#define HOMM2_IRONFIST_RUNTIME_H

#include <string>
#include <Ints.h>

namespace ironfist { struct SessionData; }

namespace ironfist::runtime {

void Initialize();
void ResetAdventureState();
void BeginMap(const char* filename);
void AdventureManagerReady();
void Shutdown();

// A new map runs its start callbacks only after its managers are ready.
// Restoring a session has a distinct lifecycle and never replays map start.
enum class SessionPhase { SESSION_IDLE, SESSION_PREPARING_MAP, SESSION_RESTORING, SESSION_READY };
SessionPhase Phase();
void BeginSessionLoad();
void FinishSessionLoad();

SessionData CaptureSession();
void RestoreSession(const SessionData& data);

enum class LoadResult { LOAD_RETAIL, LOAD_LOADED, LOAD_FAILED };
LoadResult LoadGame(const char* filename, i32 loadFromFile);
void RetailGameLoaded();
i32 SaveGame(const char* filename, i32 autosave);
std::string FileExtension(b32 isPickLoad);

} // namespace ironfist::runtime

#endif
