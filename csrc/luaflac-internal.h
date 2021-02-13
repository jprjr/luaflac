#include "luaflac.h"
#include <FLAC/ordinals.h>
#include <FLAC/metadata.h>

#if __GNUC__ > 4
#define LUAFLAC_PRIVATE __attribute__ ((visibility ("hidden")))
#else
#define LUAFLAC_PRIVATE
#endif

typedef struct luaflac_metamethods_s {
    const char *name;
    const char *metaname;
} luaflac_metamethods;


#if !defined(luaL_newlibtable) \
  && (!defined LUA_VERSION_NUM || LUA_VERSION_NUM==501)
static void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
    luaL_checkstack(L, nup+1, "too many upvalues");
    for (; l->name != NULL; l++) {  /* fill the table with given functions */
        int i;
        lua_pushlstring(L, l->name,strlen(l->name));
        for (i = 0; i < nup; i++)  /* copy upvalues to the top */
            lua_pushvalue(L, -(nup+1));
        lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
        lua_settable(L, -(nup + 3));
    }
    lua_pop(L, nup);  /* remove upvalues */
}
#endif

#if (!defined LUA_VERSION_NUM) || LUA_VERSION_NUM == 501
#define lua_setuservalue(L,i) lua_setfenv((L),(i))
#define lua_getuservalue(L,i) lua_getfenv((L),(i))
#endif

#define luaflac_push_const(x) lua_pushinteger(L,x) ; lua_setfield(L,-2, #x)

#ifdef __cplusplus
extern "C" {
#endif

LUAFLAC_PRIVATE
FLAC__uint64
luaflac_touint64(lua_State *L, int idx);

LUAFLAC_PRIVATE
void
luaflac_pushuint64(lua_State *L, FLAC__uint64 v);

LUAFLAC_PRIVATE
int
luaflac_pushstreammetadata(lua_State *L, const FLAC__StreamMetadata *m);

LUAFLAC_PRIVATE
void
luaflac_pushint64(lua_State *L, FLAC__int64 v);

LUAFLAC_PRIVATE
FLAC__int64
luaflac_toint64(lua_State *L, int idx);

/* this will copy data in, need to call FLAC__metadata_object_delete when done with metadata */
LUAFLAC_PRIVATE
FLAC__StreamMetadata *
luaflac_toflac_streammetadata(lua_State *L, int idx);

LUAFLAC_PRIVATE
int
luaflac_no_ogg(lua_State *L);

LUAFLAC_PRIVATE
extern const char * const luaflac_uint64_mt;

LUAFLAC_PRIVATE
extern const char * const luaflac_int64_mt;

LUAFLAC_PRIVATE
extern const char * const luaflac_metadata_mt;

#ifdef __cplusplus
}
#endif
