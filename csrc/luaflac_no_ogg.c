#include "luaflac-internal.h"
#include <FLAC/export.h>

int
luaflac_no_ogg(lua_State *L) {
    return luaL_error(L,"libFLAC compiled without ogg support");
}
