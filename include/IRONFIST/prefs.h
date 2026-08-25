#ifndef HOMM2_IRONFIST_PREFS_H
#define HOMM2_IRONFIST_PREFS_H

#include <string>

#include <Ints.h>

/*
 * Ironfist kept its preferences in the Windows registry under the retail
 * Heroes II key. The port stores them as key=value lines in ironfist.cfg in
 * the platform user directory. The template surface matches Ironfist's
 * read_pref/write_pref so ported call sites stay unchanged; i32 stands in
 * for their DWORD instantiation.
 */
template <typename T>
T read_pref(const std::string& key);

template <typename T>
bool read_pref(const std::string& key, T& value);

template <typename T>
bool write_pref(const std::string& key, const T& value);

bool write_pref(const std::string& key, const std::string& value);

// The "Disable Well" preference turns off the Well's +2 weekly growth game
// modification; town growth code queries it.
bool IsWellDisabled();

#endif
