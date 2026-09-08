#include <IRONFIST/scripting.h>
#include <IRONFIST/callback.h>
#include <IRONFIST/dialog.h>
#include <IRONFIST/lua.h>
#include <IRONFIST/hooks.h>
#include <cstdio>
#include <string>

namespace ironfist {
std::string ResolveDataPath(const std::string&) { return "/tmp/ironfist-empty.lua"; }
void DisplayError(const char* s, const char*) { std::fprintf(stderr, "error: %s\n", s); }
void DisplayError(std::string s, std::string t) { DisplayError(s.c_str(), t.c_str()); }
namespace script {
void RegisterConstants(lua_State*) {}
void RegisterFunctions(lua_State*) {}
}
}

char gText[768] = {};

int main(int argc, char**) {
    using namespace ironfist::script;
    if (argc > 1) {
        InitializeFromSave("function OnShowLuckInfo(h) return string.rep('x', 800) end");
        ironfist::hooks::AppendLuckInfo(nullptr);
        return 0;
    }
    InitializeFromSave("function GetTooltipText() return nil end");
    auto* L = MapState();
    std::printf("initial stack=%d\n", lua_gettop(L));
    for (int i = 0; i < 4; ++i)
        InvokeStateResult<std::string>(L, "GetTooltipText");
    std::printf("after 4 nil callbacks stack=%d\n", lua_gettop(L));
    lua_settop(L, 0);
    for (int i = 0; i < 4; ++i)
        LoadMapVariablesFromLua();
    std::printf("after 4 saves without mapVariables stack=%d\n", lua_gettop(L));
    lua_settop(L, 0);
    luaL_dostring(L, "mapVariables={'big','small'}; big=4294967297; small=0.00000001");
    auto vars = LoadMapVariablesFromLua();
    std::printf("encoded big=%s small=%s\n", vars["big"].value.c_str(), vars["small"].value.c_str());
    WriteMapVariablesToLua(vars);
    lua_getglobal(L, "big");
    std::printf("restored big=%lld\n", static_cast<long long>(lua_tointeger(L, -1)));
    lua_getglobal(L, "small");
    std::printf("restored small=%.17g\n", lua_tonumber(L, -1));
    Shutdown();
}
