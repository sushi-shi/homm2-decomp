#ifndef HOMM2_IRONFIST_ARTIFACTS_H
#define HOMM2_IRONFIST_ARTIFACTS_H

#include <map>
#include <string>
#include <vector>

#include <Ints.h>
#include <SOURCE/X_GLOBAL.h>

/*
 * Ironfist's data-driven artifact metadata: DATA/artifacts.xml is the
 * authority and fills the retail tables (gArtifactNames, gArtifactDesc,
 * gArtifactLevel), which this branch extends to KB_ARTIFACT_TABLE_CAPACITY.
 * Ironfist adds the Pandora Box (103) and Iron Fist (104) artifacts and
 * per-artifact cursed/campaign-only traits the retail tables cannot express.
 */

enum IronfistArtifact {
    ARTIFACT_PANDORA_BOX = 103,
    ARTIFACT_IRON_FIST   = 104
};

void LoadArtifacts();
i32 IsCursedItem(i32 artId);
bool IsArtifactValid(i32 id);
bool IsArtifactGenerated(i32 id);
bool IsArtifactGenerationAllowed(i32 id);
void GenerateArtifact(i32 id);
void ResetGeneratedArtifacts();
void ResetGeneratedArtifacts(i32 matchingLevels);
i32 GetArtifactLevel(i32 id);
std::string GetArtifactName(i32 id);
std::string GetArtifactDescription(i32 id);
const char* GetArtifactEvent(i32 id);

void DeserializeGeneratedArtifacts(const std::vector<i32>& source);
std::vector<i32> SerializeGeneratedArtifacts();

#endif
