#include "luaflac.h"
#include <assert.h>

static void
copydown(lua_State *L, const char *tablename) {
    lua_getglobal(L,"require");
    lua_pushstring(L,tablename);
    lua_call(L,1,1);

    /* copies keys from table on top of stack to table below */
    lua_pushnil(L);
    while(lua_next(L,-2) != 0) {
        /* -1 = value
         * -2 = key */
        lua_pushvalue(L,-2);
        lua_insert(L,-2);
        lua_settable(L,-5);
    }
    lua_pop(L,1);
}

LUAFLAC_PUBLIC
int luaopen_luaflac(lua_State *L) {
    lua_newtable(L);

    lua_getglobal(L,"require");
    lua_pushstring(L,"luaflac.int64");
    lua_call(L,1,1);
    lua_setfield(L,-2,"FLAC__int64");
    lua_getglobal(L,"require");
    lua_pushstring(L,"luaflac.uint64");
    lua_call(L,1,1);
    lua_setfield(L,-2,"FLAC__uint64");

    copydown(L,"luaflac.version");
    copydown(L,"luaflac.stream_decoder");
    copydown(L,"luaflac.stream_encoder");
    copydown(L,"luaflac.format");
    copydown(L,"luaflac.export");

    return 1;
}
