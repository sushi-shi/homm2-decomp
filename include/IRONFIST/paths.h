#ifndef HOMM2_IRONFIST_PATHS_H
#define HOMM2_IRONFIST_PATHS_H

#include <string>

// Resolve a retail-relative path against the platform data root, matching
// each component case-insensitively so retail directory layouts survive
// Unix filesystems.
std::string ResolveDataPath(const std::string& relative);

#endif
