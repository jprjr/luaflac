#include "luaflac-internal.h"
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>

static const char * const digits = "0123456789";
const char * const luaflac_uint64_mt = "FLAC__uint64";
const char * const luaflac_int64_mt = "FLAC__int64";

static char *
luaflac_uint64_to_str(FLAC__uint64 value, char buffer[21], size_t *len) {
    char *p = buffer + 20;
    *p = '\0';
    do {
        *--p = digits[value % 10];
    } while (value /= 10);

    *len = 20 - (p - buffer);
    return p;
}

static char *
luaflac_int64_to_str(FLAC__int64 value, char buffer[22], size_t *len) {
    int sign;
    FLAC__uint64 tmp;

    if(value < 0) {
        sign = 1;
        tmp = -value;
    } else {
        sign = 0;
        tmp = value;
    }
    char *p = luaflac_uint64_to_str(tmp,buffer+1,len);
    if(sign) {
        *--p = '-';
        *len += 1;
    }
    return p;
}

static int
luaflac_uint64(lua_State *L) {
    /* create a new int64 object from a number or string */
    FLAC__uint64 *t = 0;

    t = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    if(t == NULL) {
        return luaL_error(L,"out of memory");
    }

    if(lua_gettop(L) > 1) {
        *t = luaflac_touint64(L,1);
    }
    else {
        *t = 0;
    }

    luaL_setmetatable(L,luaflac_uint64_mt);
    return 1;
}


static int
luaflac_uint64__unm(lua_State *L) {
    FLAC__uint64 *o = NULL;
    FLAC__int64 *r = NULL;

    o = lua_touserdata(L,1);

    if(*o > 0x8000000000000000) {
        return luaL_error(L,"out of range");
    }

    r = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *r = -(*o);

    luaL_setmetatable(L,luaflac_int64_mt);
    return 1;
}

static int
luaflac_uint64__add(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__uint64 *res = NULL;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = a + b;

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__sub(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__uint64 *res = NULL;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = a - b;

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__mul(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__uint64 *res = NULL;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = a * b;

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__div(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__uint64 *res = NULL;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = a / b;

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__mod(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__uint64 *res = NULL;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = a % b;

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__pow(lua_State *L) {
    FLAC__uint64 base = 0;
    FLAC__uint64 exp = 0;
    FLAC__uint64 *res = NULL;
    FLAC__uint64 result = 1;

    base = luaflac_touint64(L,1);
    exp = luaflac_touint64(L,2);

    for (;;) {
        if(exp & 1) {
            result *= base;
        }
        exp >>= 1;
        if(!exp) {
            break;
        }
        base *= base;
    }

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = result;

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__eq(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    lua_pushboolean(L,a==b);
    return 1;
}

static int
luaflac_uint64__lt(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    lua_pushboolean(L,a<b);
    return 1;
}

static int
luaflac_uint64__le(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    lua_pushboolean(L,a<=b);
    return 1;
}

static int
luaflac_uint64__band(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__uint64 *res = NULL;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = a & b;

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__bor(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__uint64 *res = NULL;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = a | b;

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__bxor(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__uint64 *res = NULL;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = a ^ b;

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__bnot(lua_State *L) {
    FLAC__uint64 *o = NULL;
    FLAC__uint64 *r = NULL;

    o = lua_touserdata(L,1);

    r = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));

    *r = ~*o;

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__shl(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__uint64 *res = NULL;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = (FLAC__uint64)(a << b);

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__shr(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__uint64 *res = NULL;

    a = luaflac_touint64(L,1);
    b = luaflac_touint64(L,2);

    res = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
    *res = (FLAC__uint64)(a >> b);

    luaL_setmetatable(L,luaflac_uint64_mt);

    return 1;
}

static int
luaflac_uint64__tostring(lua_State *L) {
    char *p;
    char t[21];
    size_t l;

    FLAC__uint64 *o = (FLAC__uint64 *)lua_touserdata(L,1);
    p = luaflac_uint64_to_str(*o,t,&l);
    lua_pushlstring(L,p,l);
    return 1;
}

static int
luaflac_uint64__concat(lua_State *L) {
    lua_getglobal(L,"tostring");
    lua_pushvalue(L,1);
    lua_call(L,1,1);

    lua_getglobal(L,"tostring");
    lua_pushvalue(L,2);
    lua_call(L,1,1);

    lua_concat(L,2);
    return 1;
}

static int
luaflac_int64__unm(lua_State *L) {
    FLAC__int64 *o = NULL;
    FLAC__int64 *r = NULL;
    FLAC__uint64 *t = NULL;

    o = lua_touserdata(L,1);

    if(*o == ((FLAC__int64) 0x8000000000000000)) {
        t = (FLAC__uint64 *)lua_newuserdata(L,sizeof(FLAC__uint64));
        *t = -(*o);
        luaL_setmetatable(L,luaflac_uint64_mt);
    } else {
        r = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
        *r = -(*o);
        luaL_setmetatable(L,luaflac_int64_mt);
    }

    return 1;
}

static int
luaflac_int64__add(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;
    FLAC__int64 *res = NULL;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    if(res == NULL) {
        return luaL_error(L,"out of memory");
    }

    *res = a + b;

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__sub(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;
    FLAC__int64 *res = NULL;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *res = a - b;

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__mul(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;
    FLAC__int64 *res = NULL;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *res = a * b;

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__div(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;
    FLAC__int64 *res = NULL;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *res = a / b;

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__mod(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;
    FLAC__int64 *res = NULL;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *res = a % b;

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__pow(lua_State *L) {
    FLAC__int64 base = 0;
    FLAC__int64 exp = 0;
    FLAC__int64 *res = NULL;
    FLAC__int64 result = 1;

    base = luaflac_toint64(L,1);
    exp = luaflac_toint64(L,2);

    if(exp < 0) {
        return luaL_error(L,"exp must be positive");
    }

    for (;;) {
        if(exp & 1) {
            result *= base;
        }
        exp >>= 1;
        if(!exp) {
            break;
        }
        base *= base;
    }

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *res = result;

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__eq(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    lua_pushboolean(L,a==b);
    return 1;
}

static int
luaflac_int64__lt(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    lua_pushboolean(L,a<b);
    return 1;
}

static int
luaflac_int64__le(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    lua_pushboolean(L,a<=b);
    return 1;
}

static int
luaflac_int64__band(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;
    FLAC__int64 *res = NULL;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *res = a & b;

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__bor(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;
    FLAC__int64 *res = NULL;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *res = a | b;

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__bxor(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;
    FLAC__int64 *res = NULL;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *res = a ^ b;

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__bnot(lua_State *L) {
    FLAC__int64 *o = NULL;
    FLAC__int64 *r = NULL;

    o = lua_touserdata(L,1);

    r = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));

    *r = ~*o;

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__shl(lua_State *L) {
    FLAC__int64 a = 0;
    FLAC__int64 b = 0;
    FLAC__int64 *res = NULL;

    a = luaflac_toint64(L,1);
    b = luaflac_toint64(L,2);

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *res = (FLAC__int64)(a << b);

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__shr(lua_State *L) {
    FLAC__uint64 a = 0;
    FLAC__uint64 b = 0;
    FLAC__int64 *res = NULL;

    a = (FLAC__uint64)luaflac_toint64(L,1);
    b = (FLAC__uint64)luaflac_toint64(L,2);

    res = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    *res = (FLAC__int64)(a >> b);

    luaL_setmetatable(L,luaflac_int64_mt);

    return 1;
}

static int
luaflac_int64__tostring(lua_State *L) {
    char *p;
    char t[22];
    size_t l;

    FLAC__int64 *o = (FLAC__int64 *)lua_touserdata(L,1);
    p = luaflac_int64_to_str(*o,t,&l);
    lua_pushlstring(L,p,l);
    return 1;
}

static int
luaflac_int64__concat(lua_State *L) {
    lua_getglobal(L,"tostring");
    lua_pushvalue(L,1);
    lua_call(L,1,1);

    lua_getglobal(L,"tostring");
    lua_pushvalue(L,2);
    lua_call(L,1,1);

    lua_concat(L,2);
    return 1;
}

static int
luaflac_int64(lua_State *L) {
    /* create a new int64 object from a number or string */
    FLAC__int64 *t = 0;

    t = (FLAC__int64 *)lua_newuserdata(L,sizeof(FLAC__int64));
    if(t == NULL) {
        return luaL_error(L,"out of memory");
    }

    if(lua_gettop(L) > 1) {
        *t = luaflac_toint64(L,1);
    }
    else {
        *t = 0;
    }

    luaL_setmetatable(L,luaflac_int64_mt);
    return 1;
}

static const struct luaL_Reg luaflac_int64_metamethods[] = {
    { "__add", luaflac_int64__add },
    { "__sub", luaflac_int64__sub },
    { "__mul", luaflac_int64__mul },
    { "__div", luaflac_int64__div },
    { "__idiv", luaflac_int64__div },
    { "__mod", luaflac_int64__mod },
    { "__pow", luaflac_int64__pow },
    { "__unm", luaflac_int64__unm },
    { "__band", luaflac_int64__band },
    { "__bor", luaflac_int64__bor },
    { "__bxor", luaflac_int64__bxor },
    { "__bnot", luaflac_int64__bnot },
    { "__shl", luaflac_int64__shl },
    { "__shr", luaflac_int64__shr },
    { "__eq", luaflac_int64__eq },
    { "__lt", luaflac_int64__lt },
    { "__le", luaflac_int64__le },
    { "__tostring", luaflac_int64__tostring },
    { "__concat", luaflac_int64__concat },
    { NULL, NULL },
};

static const struct luaL_Reg luaflac_uint64_metamethods[] = {
    { "__add", luaflac_uint64__add },
    { "__sub", luaflac_uint64__sub },
    { "__mul", luaflac_uint64__mul },
    { "__div", luaflac_uint64__div },
    { "__idiv", luaflac_uint64__div },
    { "__mod", luaflac_uint64__mod },
    { "__pow", luaflac_uint64__pow },
    { "__unm", luaflac_uint64__unm },
    { "__band", luaflac_uint64__band },
    { "__bor", luaflac_uint64__bor },
    { "__bxor", luaflac_uint64__bxor },
    { "__bnot", luaflac_uint64__bnot },
    { "__shl", luaflac_uint64__shl },
    { "__shr", luaflac_uint64__shr },
    { "__eq", luaflac_uint64__eq },
    { "__lt", luaflac_uint64__lt },
    { "__le", luaflac_uint64__le },
    { "__tostring", luaflac_uint64__tostring },
    { "__concat", luaflac_uint64__concat },
    { NULL, NULL },
};

static void
register_metatables(lua_State *L) {
    /* whether we're loading int64 or uint64, ensure
     * metatables always exist since they depend on
     * eachother */

    luaL_newmetatable(L,luaflac_int64_mt);
    luaL_setfuncs(L,luaflac_int64_metamethods,0);
    lua_pop(L,1);

    luaL_newmetatable(L,luaflac_uint64_mt);
    luaL_setfuncs(L,luaflac_uint64_metamethods,0);
    lua_pop(L,1);
}

LUAFLAC_PRIVATE
FLAC__uint64 luaflac_touint64(lua_State *L, int idx) {
    FLAC__uint64 *t = NULL;
    FLAC__int64 *r = NULL;
    FLAC__uint64 tmp  = 0;
    const char *str = NULL;
    const char *s = NULL;
    char *end = NULL;

    switch(lua_type(L,idx)) {
        case LUA_TNONE: {
            return 0;
        }
        case LUA_TNIL: {
            return 0;
        }
        case LUA_TBOOLEAN: {
            return (FLAC__uint64)lua_toboolean(L,idx);
        }
        case LUA_TNUMBER: {
            return (FLAC__uint64)lua_tointeger(L,idx);
        }
        case LUA_TUSERDATA: {
            t = luaL_testudata(L,idx,luaflac_uint64_mt);
            if(t != NULL) {
                return *t;
            }
            r = luaL_testudata(L,idx,luaflac_int64_mt);
            if(r != NULL) {
                if(*r < 0) {
                    luaL_error(L,"out of range");
                    return 0;
                }
                return (FLAC__uint64)*r;
            }
            break;
        }
        /* we'll try converting to a string */
        default: break;
    }

    str = lua_tostring(L,idx);
    if(str == NULL) {
        luaL_error(L,"invalid value");
        return 0;
    }

    s = str;
    while(*s) {
        if(isspace(*s)) {
            s++;
        } else {
            break;
        }
    }

    if(!*s) { /* empty string, or just spaces */
        luaL_error(L,"invalid string, %s has no characters",str);
        return 0;
    }
    if(*s == '-') {
        luaL_error(L,"invalid string, %s is negative",str);
        return 0;
    }
    errno = 0;
    tmp = strtoull(s, &end, 10);
    if(errno) {
        luaL_error(L,"invalid integer string");
        return 0;
    }
    if(s == end) {
        luaL_error(L,"invalid string");
        return 0;
    }
    return tmp;
}

LUAFLAC_PRIVATE
FLAC__int64 luaflac_toint64(lua_State *L, int idx) {
    FLAC__int64 *t = NULL;
    FLAC__uint64 *r = NULL;
    FLAC__int64 tmp  = 0;
    const char *str = NULL;

    switch(lua_type(L,idx)) {
        case LUA_TNONE: {
            return 0;
        }
        case LUA_TNIL: {
            return 0;
        }
        case LUA_TBOOLEAN: {
            return (FLAC__int64)lua_toboolean(L,idx);
        }
        case LUA_TNUMBER: {
            return (FLAC__int64)lua_tointeger(L,idx);
        }
        case LUA_TUSERDATA: {
            t = luaL_testudata(L,idx,luaflac_int64_mt);
            if(t != NULL) {
                return *t;
            }
            r = luaL_testudata(L,idx,luaflac_uint64_mt);
            if(r != NULL) {
                if(*r > 0x7FFFFFFFFFFFFFFF) {
                    luaL_error(L,"out of range");
                    return 0;
                }
                return (FLAC__int64) *r;
            }
            break;
        }
        /* we'll try converting to a string */
        default: break;
    }

    str = lua_tostring(L,idx);
    if(str == NULL) {
        luaL_error(L,"invalid value");
        return 0;
    }
    errno = 0;
    tmp = strtoull(str, NULL, 10);
    if(errno) {
        luaL_error(L,"invalid integer string");
        return 0;
    }
    return tmp;
}

LUAFLAC_PRIVATE
void luaflac_pushuint64(lua_State *L, FLAC__uint64 v) {
    FLAC__uint64 *d = NULL;
    d = lua_newuserdata(L,sizeof(FLAC__uint64));
    if(d == NULL) {
        luaL_error(L,"out of memory");
    }
    *d = v;
    luaL_setmetatable(L,luaflac_uint64_mt);
}

LUAFLAC_PRIVATE
void luaflac_pushint64(lua_State *L, FLAC__int64 v) {
    FLAC__int64 *d = NULL;
    d = lua_newuserdata(L,sizeof(FLAC__int64));
    if(d == NULL) {
        luaL_error(L,"out of memory");
    }
    *d = v;
    luaL_setmetatable(L,luaflac_int64_mt);
}

LUAFLAC_PUBLIC
int luaopen_luaflac_int64(lua_State *L) {
    register_metatables(L);
    lua_pushcclosure(L,luaflac_int64,0);
    return 1;
}

LUAFLAC_PUBLIC
int luaopen_luaflac_uint64(lua_State *L) {
    register_metatables(L);
    lua_pushcclosure(L,luaflac_uint64,0);
    return 1;
}

