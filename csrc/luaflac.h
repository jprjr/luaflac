#include <lua.h>
#include <lauxlib.h>

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(_MSC_VER)
#define LUAFLAC_PUBLIC __declspec(dllexport)
#elif __GNUC__ > 4
#define LUAFLAC_PUBLIC __attribute__ ((visibility ("default")))
#else
#define LUAFLAC_PUBLIC
#endif

#ifdef __cplusplus
extern "C" {
#endif

LUAFLAC_PUBLIC
int luaopen_luaflac(lua_State *L);

LUAFLAC_PUBLIC
int luaopen_luaflac_format(lua_State *L);

LUAFLAC_PUBLIC
int luaopen_luaflac_int64(lua_State *L);

LUAFLAC_PUBLIC
int luaopen_luaflac_uint64(lua_State *L);

LUAFLAC_PUBLIC
int luaopen_luaflac_stream_decoder(lua_State *L);

LUAFLAC_PUBLIC
int luaopen_luaflac_stream_encoder(lua_State *L);

#ifdef __cplusplus
}
#endif
