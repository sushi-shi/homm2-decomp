#include <PLATFORM/FileSystem.h>

#include <cstdio>
#include <string>

namespace {

bool Expect(const std::string& actual, const char* expected, const char* description) {
    if (actual == expected) {
        return true;
    }
    std::fprintf(
        stderr,
        "filesystem mismatch: %s: got '%s', expected '%s'\n",
        description,
        actual.c_str(),
        expected
    );
    return false;
}

bool Expect(bool actual, bool expected, const char* description) {
    if (actual == expected) {
        return true;
    }
    std::fprintf(stderr, "filesystem mismatch: %s\n", description);
    return false;
}

}

int main() {
    bool valid = true;
    valid &= Expect(
        platform::ResolveIn("/game", ".\\GAMES\\AUTOSAVE.GIC"),
        "/game/GAMES/AUTOSAVE.GIC",
        "retail separators"
    );
    valid &= Expect(
        platform::ResolveIn("/game", "DATA/../MAPS/TEST.MP2"),
        "/game/MAPS/TEST.MP2",
        "parent component inside root"
    );
    valid &= Expect(
        platform::ResolveIn("/game", "../../outside"),
        "/game/outside",
        "parent component cannot escape root"
    );
    valid &= Expect(platform::IsUserState("HEROES2.CFG"), true, "preferences are user state");
    valid &= Expect(
        platform::IsUserState(".\\GAMES\\AUTOSAVE.GIC"),
        true,
        "saved games are user state"
    );
    valid &= Expect(
        platform::IsUserState("DATA\\HEROES2.AGG"),
        false,
        "installation aggregate is not user state"
    );
    valid &= Expect(
        platform::IsUserState("DATA\\H2CABCD.NW"),
        false,
        "removed compressor scratch names are not user state"
    );
    valid &= Expect(
        platform::IsUserState("GAMES/../DATA/HEROES2.AGG"),
        false,
        "state classification normalizes parent components"
    );
    return valid ? 0 : 1;
}
