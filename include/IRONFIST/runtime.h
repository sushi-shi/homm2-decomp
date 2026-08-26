#ifndef HOMM2_IRONFIST_RUNTIME_H
#define HOMM2_IRONFIST_RUNTIME_H

namespace ironfist::runtime {

void Initialize();
void ResetAdventureState();
void BeginMap(const char* filename);
void AdventureManagerReady();
void Shutdown();

} // namespace ironfist::runtime

#endif
