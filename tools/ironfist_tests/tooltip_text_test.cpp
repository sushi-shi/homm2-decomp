#include <BASE/Utf8.h>
#include <IRONFIST/callback.h>
#include <IRONFIST/hooks.h>
#include <IRONFIST/scripting.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/KB.h>

#include <cassert>
#include <cstring>
#include <string>

int main() {
    constexpr size_t capacity = GLOBAL_TEXT_BUFFER_SIZE;
    ironfist::script::InitializeFromSave(R"(
        function OnShowLuckInfo() return extra end
        function OnShowMoraleInfo() return extra end
    )");
    auto* state = ironfist::script::MapState();
    for (auto append : {ironfist::hooks::AppendLuckInfo, ironfist::hooks::AppendMoraleInfo}) {
        for (const std::string& extra : {std::string("bonus"), std::string(800, 'x'),
                                       std::string("Żółć🙂"), std::string()}) {
            lua_pushstring(state, extra.c_str());
            lua_setglobal(state, "extra");
            for (const size_t used : {size_t(0), capacity-5, capacity-4,
                                     capacity-3, capacity-2, capacity-1}) {
                const std::string prefix(used, 'a');
                std::strcpy(gText, prefix.c_str());
                append(nullptr);
                assert(std::memchr(gText, 0, capacity));
                assert(std::strlen(gText) < capacity);
                assert(utf8::IsValid(gText));
                assert(std::string(gText).starts_with(prefix));
                const std::string complete = prefix + "\n" + extra;
                assert(complete.starts_with(gText));
                if (complete.size() < capacity)
                    assert(gText == complete);
            }
        }
        lua_pushnil(state);
        lua_setglobal(state, "extra");
        std::strcpy(gText, "unchanged");
        append(nullptr);
        assert(std::strcmp(gText, "unchanged") == 0);
    }
    ironfist::script::Shutdown();
}
