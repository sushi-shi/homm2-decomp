#ifndef HOMM2_IRONFIST_REGISTER_H
#define HOMM2_IRONFIST_REGISTER_H

#include <IRONFIST/lua.h>

namespace ironfist::script {

void RegisterConstants(lua_State* L);
void RegisterFunctions(lua_State* L);

} // namespace ironfist::script

#endif
