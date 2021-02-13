#include "luaflac_internal.h"
#include <FLAC/export.h>

LUAFLAC_PUBLIC
int luaopen_luaflac_export(lua_State *L) {
    lua_newtable(L);

    luaflac_push_const(FLAC_API_VERSION_CURRENT);
    luaflac_push_const(FLAC_API_VERSION_REVISION);
    luaflac_push_const(FLAC_API_VERSION_AGE);

    lua_pushboolean(L,FLAC_API_SUPPORTS_OGG_FLAC);
    lua_setfield(L,-2,"FLAC_API_SUPPORTS_OGG_FLAG");

    return 1;
}
