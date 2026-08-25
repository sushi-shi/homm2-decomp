#include <IRONFIST/prefs.h>

#include <cstdlib>

#include <fstream>
#include <map>
#include <string>

#include <PLATFORM/Platform.h>

static std::string PrefsPath() {
    return platform::Files().UserRoot() + "/ironfist.cfg";
}

static std::map<std::string, std::string> ReadAllPrefs() {
    std::map<std::string, std::string> prefs;
    std::ifstream in(PrefsPath());
    std::string line;
    while (std::getline(in, line)) {
        const std::size_t split = line.find('=');
        if (split != std::string::npos) {
            prefs[line.substr(0, split)] = line.substr(split + 1);
        }
    }
    return prefs;
}

static bool WriteAllPrefs(const std::map<std::string, std::string>& prefs) {
    std::ofstream out(PrefsPath(), std::ios::trunc);
    if (!out) {
        return false;
    }
    for (const auto& pref : prefs) {
        out << pref.first << '=' << pref.second << '\n';
    }
    return static_cast<bool>(out);
}

template <>
i32 read_pref(const std::string& key) {
    std::map<std::string, std::string> prefs = ReadAllPrefs();
    const auto found = prefs.find(key);
    if (found == prefs.end()) {
        return -1;
    }
    return atoi(found->second.c_str());
}

template <>
std::string read_pref(const std::string& key) {
    std::map<std::string, std::string> prefs = ReadAllPrefs();
    const auto found = prefs.find(key);
    return found == prefs.end() ? std::string() : found->second;
}

template <>
bool read_pref(const std::string& key, std::string& value) {
    std::map<std::string, std::string> prefs = ReadAllPrefs();
    const auto found = prefs.find(key);
    if (found == prefs.end()) {
        return false;
    }
    value = found->second;
    return true;
}

template <>
bool write_pref(const std::string& key, const i32& value) {
    std::map<std::string, std::string> prefs = ReadAllPrefs();
    prefs[key] = std::to_string(value);
    return WriteAllPrefs(prefs);
}

bool write_pref(const std::string& key, const std::string& value) {
    std::map<std::string, std::string> prefs = ReadAllPrefs();
    prefs[key] = value;
    return WriteAllPrefs(prefs);
}

static bool IsWellDisabled_impl() {
    const std::string key = "Disable Well";
    const i32 wellSetting = read_pref<i32>(key);
    if (wellSetting == -1) {
        write_pref<i32>(key, 0);
        return false;
    }

    return wellSetting == 1;
}

bool IsWellDisabled() {
    static const bool isDisabled = IsWellDisabled_impl();
    return isDisabled;
}
