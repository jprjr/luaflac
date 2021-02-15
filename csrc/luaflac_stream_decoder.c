#include "luaflac_internal.h"
#include <FLAC/stream_decoder.h>

#include <string.h>
#include <assert.h>

LUAFLAC_PRIVATE
const char * const luaflac_stream_decoder_mt = "FLAC__StreamDecoder";

struct luaflac_decoder_userdata_s {
    lua_State *L;
    int table_ref;
    FLAC__StreamDecoder *decoder;
};

typedef struct luaflac_decoder_userdata_s luaflac_decoder_userdata;

static int
luaflac_stream_decoder_delete(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    if(u->decoder != NULL) {
        FLAC__stream_decoder_delete(u->decoder);
        u->decoder = NULL;
    }
    if(u->table_ref != LUA_NOREF) {
        luaL_unref(u->L,LUA_REGISTRYINDEX,u->table_ref);
        u->table_ref = LUA_NOREF;
    }
    return 0;
}


static int
luaflac_stream_decoder_new(lua_State *L) {
    luaflac_decoder_userdata *u = NULL;

    u = (luaflac_decoder_userdata *)lua_newuserdata(L,sizeof(luaflac_decoder_userdata));
    if(u == NULL) {
        return luaL_error(L,"out of memory");
    }

    u->L = L;
    u->decoder = FLAC__stream_decoder_new();
    if(u->decoder == NULL) {
        return luaL_error(L,"out of memory");
    }

    lua_newtable(L); /* table with lua i/o functions */
    u->table_ref = luaL_ref(u->L,LUA_REGISTRYINDEX);

    luaL_setmetatable(L,luaflac_stream_decoder_mt);

    return 1;
}

static int
luaflac_stream_decoder_set_ogg_serial_number(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_set_ogg_serial_number(u->decoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_decoder_set_md5_checking(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_set_md5_checking(u->decoder,lua_toboolean(L,2)));
    return 1;
}

static int
luaflac_stream_decoder_set_metadata_respond(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_set_metadata_respond(u->decoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_decoder_set_metadata_respond_application(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    const char *id = NULL;
    size_t idlen = 0;
    id = lua_tolstring(L,2,&idlen);
    if(idlen != 4) {
        return luaL_error(L,"invalid application id");
    }
    lua_pushboolean(L,FLAC__stream_decoder_set_metadata_respond_application(u->decoder,(const FLAC__byte *)id));
    return 1;
}

static int
luaflac_stream_decoder_set_metadata_respond_all(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_set_metadata_respond_all(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_set_metadata_ignore(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_set_metadata_ignore(u->decoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_decoder_set_metadata_ignore_application(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    const char *id = NULL;
    size_t idlen = 0;
    id = lua_tolstring(L,2,&idlen);
    if(idlen != 4) {
        return luaL_error(L,"invalid application id");
    }
    lua_pushboolean(L,FLAC__stream_decoder_set_metadata_ignore_application(u->decoder,(const FLAC__byte *)id));
    return 1;
}

static int
luaflac_stream_decoder_set_metadata_ignore_all(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_set_metadata_ignore_all(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_get_state(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushinteger(L,FLAC__stream_decoder_get_state(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_get_resolved_state_string(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushstring(L,FLAC__stream_decoder_get_resolved_state_string(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_get_md5_checking(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_get_md5_checking(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_get_total_samples(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    luaflac_pushuint64(L,FLAC__stream_decoder_get_total_samples(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_get_channels(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushinteger(L,FLAC__stream_decoder_get_channels(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_get_channel_assignment(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushinteger(L,FLAC__stream_decoder_get_channel_assignment(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_get_bits_per_sample(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushinteger(L,FLAC__stream_decoder_get_bits_per_sample(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_get_sample_rate(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushinteger(L,FLAC__stream_decoder_get_sample_rate(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_get_blocksize(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushinteger(L,FLAC__stream_decoder_get_blocksize(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_get_decode_position(lua_State *L) {
    luaflac_decoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    FLAC__uint64 position = 0;
    if(FLAC__stream_decoder_get_decode_position(u->decoder,&position)) {
        luaflac_pushuint64(L,position);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static FLAC__StreamDecoderReadStatus
luaflac_stream_decoder_read_callback(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[],
  size_t *bytes, void *client_data) {
    int top;
    FLAC__StreamDecoderReadStatus status;
    const char *data = NULL;
    size_t datalen = 0;
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"read");
    lua_getfield(u->L,-2,"userdata");
    lua_pushinteger(u->L,*bytes);

    lua_call(u->L,2,1);

    /* return values:
     *   nil = eof
     *   true = eof
     *   false = abort
     *   string = continue
     */

    if(lua_isnil(u->L,-1)) {
        status = FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
    } else if(lua_isboolean(u->L,-1)) {
        status = lua_toboolean(u->L,-1) ? FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM :
          FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    } else {
        data = lua_tolstring(u->L,-1,&datalen);
        if(datalen > *bytes) {
            luaL_error(u->L,"returned more bytes than requested");
            status = FLAC__STREAM_DECODER_READ_STATUS_ABORT;
        } else if(datalen == 0) {
            status = FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
        } else {
            memcpy(buffer,data,datalen);
            *bytes = datalen;
            status = FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
        }
    }

    lua_pop(u->L,2);
    assert(top == lua_gettop(u->L));

    (void)decoder;
    return status;
}

static FLAC__StreamDecoderSeekStatus
luaflac_stream_decoder_seek_callback(const FLAC__StreamDecoder *decoder, FLAC__uint64 absolute_byte_offset,
  void *client_data) {
    int top;
    FLAC__uint64 *t = NULL;
    FLAC__StreamDecoderSeekStatus status;
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"seek");
    lua_getfield(u->L,-2,"userdata");
    t = lua_newuserdata(u->L,sizeof(FLAC__uint64));
    if(t == NULL) {
        luaL_error(u->L,"out of memory");
        return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    }
    *t = absolute_byte_offset;
    luaL_setmetatable(u->L,luaflac_uint64_mt);

    lua_call(u->L,2,1);

    /* true: seek OK
     * false: seek error */

    /* assumption is if seeking is not supported, then
     * the seek callback should be nil */

    if(lua_isnil(u->L,-1)) {
        status = FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    } else {
        status = lua_toboolean(u->L,-1) ? FLAC__STREAM_DECODER_SEEK_STATUS_OK :
          FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    }
    lua_pop(u->L,2);
    assert(top == lua_gettop(u->L));

    (void)decoder;
    return status;

}

static FLAC__StreamDecoderTellStatus
luaflac_stream_decoder_tell_callback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *absolute_byte_offset,
  void *client_data) {
    int top;
    FLAC__StreamDecoderTellStatus status;
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"tell");
    lua_getfield(u->L,-2,"userdata");

    lua_call(u->L,1,1);

    /* true: tell OK
     * false: tell error */

    /* assumption is if tell is not supported, then
     * the tell callback should be nil */

    if(lua_isnumber(u->L,-1) || luaL_testudata(u->L,-1,luaflac_uint64_mt) || lua_isstring(u->L,-1)) {
        status = FLAC__STREAM_DECODER_TELL_STATUS_OK;
        *absolute_byte_offset = luaflac_touint64(u->L,-1);
    } else {
        status = FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
    }
    lua_pop(u->L,2);
    assert(top == lua_gettop(u->L));

    (void)decoder;
    return status;
}

static FLAC__StreamDecoderLengthStatus
luaflac_stream_decoder_length_callback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *stream_length,
  void *client_data) {
    int top;
    FLAC__StreamDecoderLengthStatus status;
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"length");
    lua_getfield(u->L,-2,"userdata");

    lua_call(u->L,1,1);

    /* true: length OK
     * false: length error */

    /* assumption is if lengthing is not supported, then
     * the length callback should be nil */

    if(lua_isnumber(u->L,-1) || luaL_testudata(u->L,-1,luaflac_uint64_mt) || lua_isstring(u->L,-1)) {
        status = FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
        *stream_length = luaflac_touint64(u->L,-1);
    } else {
        status = FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
    }
    lua_pop(u->L,2);
    assert(top == lua_gettop(u->L));

    (void)decoder;
    return status;
}

static FLAC__bool
luaflac_stream_decoder_eof_callback(const FLAC__StreamDecoder *decoder, void *client_data) {
    int top;
    FLAC__bool status;
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"eof");
    lua_getfield(u->L,-1,"userdata");

    lua_call(u->L,1,1);

    status = lua_toboolean(u->L,-1);

    lua_pop(u->L,2);
    assert(top == lua_gettop(u->L));

    (void)decoder;
    return status;
}

static void
luaflac_stream_decoder_metadata_callback(const FLAC__StreamDecoder *decoder,
  const FLAC__StreamMetadata *metadata,
  void *client_data) {
    int top;
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"metadata");
    lua_getfield(u->L,-2,"userdata");

    luaflac_pushstreammetadata(u->L,metadata);

    lua_call(u->L,2,0);

    lua_pop(u->L,1);
    assert(top == lua_gettop(u->L));
    (void)decoder;
}

static void
luaflac_stream_decoder_error_callback(const FLAC__StreamDecoder *decoder,
  const FLAC__StreamDecoderErrorStatus status,
  void *client_data) {
    int top;
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"error");
    lua_getfield(u->L,-2,"userdata");
    lua_pushinteger(u->L, status);

    lua_call(u->L,2,0);

    lua_pop(u->L,1);
    assert(top == lua_gettop(u->L));
    (void)decoder;
}

static FLAC__StreamDecoderWriteStatus
luaflac_stream_decoder_write_callback(const FLAC__StreamDecoder *decoder,
  const FLAC__Frame *frame,
  const FLAC__int32 *const buffer[],
  void *client_data) {
    int success;
    int top;
    unsigned int i;
    unsigned int j;
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)client_data;

    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"write");
    lua_getfield(u->L,-2,"userdata");

    lua_newtable(u->L); /* FLAC__Frame */

    lua_newtable(u->L); /* FLAC__FrameHeader */
    lua_pushinteger(u->L,frame->header.blocksize);
    lua_setfield(u->L,-2,"blocksize");
    lua_pushinteger(u->L,frame->header.sample_rate);
    lua_setfield(u->L,-2,"sample_rate");
    lua_pushinteger(u->L,frame->header.channels);
    lua_setfield(u->L,-2,"channels");
    lua_pushinteger(u->L,frame->header.channel_assignment);
    lua_setfield(u->L,-2,"channel_assignment");
    lua_pushinteger(u->L,frame->header.bits_per_sample);
    lua_setfield(u->L,-2,"bits_per_sample");
    lua_pushinteger(u->L,frame->header.number_type);
    lua_setfield(u->L,-2,"number_type");
    if(frame->header.number_type == FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER) {
        lua_pushinteger(u->L,frame->header.number.frame_number);
        lua_setfield(u->L,-2,"frame_number");
    } else {
        lua_pushinteger(u->L,frame->header.number.sample_number);
        lua_setfield(u->L,-2,"sample_number");
    }
    lua_pushinteger(u->L,frame->header.crc);
    lua_setfield(u->L,-2,"crc");
    lua_setfield(u->L,-2,"header"); /* end FLAC__FrameHeader */
    lua_newtable(u->L); /* FLAC__SubFrame[FLAC_MAX_CHANNELS] */

    i = 0;
    while(i<frame->header.channels) {
        lua_newtable(u->L);
        lua_pushinteger(u->L,frame->subframes[i].type);
        lua_setfield(u->L,-2,"type");
        lua_rawseti(u->L,-2,++i);
    }
    lua_setfield(u->L,-2,"subframes"); /* end FLAC__SubFrame */

    lua_newtable(u->L); /* FLAC__FrameFooter */
    lua_pushinteger(u->L,frame->footer.crc);
    lua_setfield(u->L,-2,"crc");
    lua_setfield(u->L,-2,"footer"); /* end FLAC__FrameFooter */

    /* end FLAC__Frame */
    lua_newtable(u->L); /* FLAC__int32 *const buffer[] */
    i=0;
    while(i<frame->header.channels) {
        lua_newtable(u->L);
        j=0;
        while(j<frame->header.blocksize) {
            lua_pushinteger(u->L,buffer[i][j]);
            lua_rawseti(u->L,-2,++j);
        }
        lua_rawseti(u->L,-2,++i);
    }

    lua_call(u->L,3,1);

    success = lua_toboolean(u->L,-1);
    lua_pop(u->L,2);

    assert(top == lua_gettop(u->L));

    (void)decoder;
    return success ? FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE : FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
}

static int
luaflac_stream_decoder_init_stream(lua_State *L) {
    FLAC__StreamDecoderInitStatus (*init_stream)(FLAC__StreamDecoder *,
      FLAC__StreamDecoderReadCallback,
      FLAC__StreamDecoderSeekCallback,
      FLAC__StreamDecoderTellCallback,
      FLAC__StreamDecoderLengthCallback,
      FLAC__StreamDecoderEofCallback,
      FLAC__StreamDecoderWriteCallback,
      FLAC__StreamDecoderMetadataCallback,
      FLAC__StreamDecoderErrorCallback,
      void *) = NULL;

    luaflac_decoder_userdata *u = NULL;
    FLAC__StreamDecoderReadCallback read_callback = NULL;
    FLAC__StreamDecoderSeekCallback seek_callback = NULL;
    FLAC__StreamDecoderTellCallback tell_callback = NULL;
    FLAC__StreamDecoderLengthCallback length_callback = NULL;
    FLAC__StreamDecoderEofCallback eof_callback = NULL;
    FLAC__StreamDecoderWriteCallback write_callback = NULL;
    FLAC__StreamDecoderMetadataCallback metadata_callback = NULL;
    FLAC__StreamDecoderErrorCallback error_callback = NULL;
    FLAC__StreamDecoderInitStatus status = 0;

    if(!lua_istable(L,2)) {
        return luaL_error(L,"missing required parameter table");
    }

    init_stream = lua_touserdata(L,lua_upvalueindex(1));

    u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_rawgeti(L,LUA_REGISTRYINDEX,u->table_ref);

    lua_getfield(L,2,"read");
    if(lua_isfunction(L,-1)) {
        read_callback = luaflac_stream_decoder_read_callback;
        lua_setfield(L,-2,"read");
    }
    else {
        return luaL_error(L,"read callback must not be null");
    }

    lua_getfield(L,2,"seek");
    if(lua_isfunction(L,-1)) {
        seek_callback = luaflac_stream_decoder_seek_callback;
        lua_setfield(L,-2,"seek");

        /* if seek callback is defined, then tell/length/eof are required */
        lua_getfield(L,2,"tell");
        if(!lua_isfunction(L,-1)) {
            return luaL_error(L,"missing tell callback");
        }
        tell_callback = luaflac_stream_decoder_tell_callback;
        lua_setfield(L,-2,"tell");

        lua_getfield(L,2,"length");
        if(!lua_isfunction(L,-1)) {
            return luaL_error(L,"missing length callback");
        }
        length_callback = luaflac_stream_decoder_length_callback;
        lua_setfield(L,-2,"length");

        lua_getfield(L,2,"eof");
        if(!lua_isfunction(L,-1)) {
            return luaL_error(L,"missing eof callback");
        }
        eof_callback = luaflac_stream_decoder_eof_callback;
        lua_setfield(L,-2,"eof");
    } else {
        lua_pop(L,1);
    }

    lua_getfield(L,2,"write");
    if(lua_isfunction(L,-1)) {
        write_callback = luaflac_stream_decoder_write_callback;
        lua_setfield(L,-2,"write");
    }
    else {
        return luaL_error(L,"write callback must not be null");
    }

    lua_getfield(L,2,"metadata");
    if(lua_isfunction(L,-1)) {
        metadata_callback = luaflac_stream_decoder_metadata_callback;
        lua_setfield(L,-2,"metadata");
    }
    else {
        lua_pop(L,1);
    }

    lua_getfield(L,2,"error");
    if(lua_isfunction(L,-1)) {
        error_callback = luaflac_stream_decoder_error_callback;
        lua_setfield(L,-2,"error");
    }
    else {
        return luaL_error(L,"error callback must not be nil");
    }

    lua_getfield(L,2,"userdata");
    lua_setfield(L,-2,"userdata");

    status = init_stream(u->decoder,
      read_callback,
      seek_callback,
      tell_callback,
      length_callback,
      eof_callback,
      write_callback,
      metadata_callback,
      error_callback,
      u);

    lua_pop(L,1);

    if(status == FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        lua_pushboolean(L,1);
        return 1;
    }
    lua_pushnil(L);
    lua_pushinteger(L,status);
    return 2;
}

static int
luaflac_stream_decoder_init_file(lua_State *L) {
    FLAC__StreamDecoderInitStatus (*init_file)(FLAC__StreamDecoder *, const char *,
      FLAC__StreamDecoderWriteCallback,
      FLAC__StreamDecoderMetadataCallback,
      FLAC__StreamDecoderErrorCallback,
      void *) = NULL;

    luaflac_decoder_userdata *u = NULL;
    const char *filename = NULL;
    FLAC__StreamDecoderWriteCallback write_callback = NULL;
    FLAC__StreamDecoderMetadataCallback metadata_callback = NULL;
    FLAC__StreamDecoderErrorCallback error_callback = NULL;
    FLAC__StreamDecoderInitStatus status = 0;

    if(!lua_istable(L,2)) {
        return luaL_error(L,"missing required parameter table");
    }

    init_file = lua_touserdata(L,lua_upvalueindex(1));

    u = luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_rawgeti(L,LUA_REGISTRYINDEX,u->table_ref);

    lua_getfield(L,2,"filename");
    if(lua_isstring(L,-1)) {
        filename = lua_tostring(L,-1);
    }
    else {
        return luaL_error(L,"filename must not be nil");
    }
    lua_pop(L,1);


    lua_getfield(L,2,"write");
    if(lua_isfunction(L,-1)) {
        write_callback = luaflac_stream_decoder_write_callback;
        lua_setfield(L,-2,"write");
    }
    else {
        return luaL_error(L,"write callback must not be null");
    }

    lua_getfield(L,2,"metadata");
    if(lua_isfunction(L,-1)) {
        metadata_callback = luaflac_stream_decoder_metadata_callback;
        lua_setfield(L,-2,"metadata");
    }
    else {
        lua_pop(L,1);
    }

    lua_getfield(L,2,"error");
    if(lua_isfunction(L,-1)) {
        error_callback = luaflac_stream_decoder_error_callback;
        lua_setfield(L,-2,"error");
    }
    else {
        return luaL_error(L,"error callback must not be nil");
    }

    lua_getfield(L,2,"userdata");
    lua_setfield(L,-2,"userdata");

    status = init_file(u->decoder,
      filename,
      write_callback,
      metadata_callback,
      error_callback,
      u);

    lua_pop(L,1);

    if(status == FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        lua_pushboolean(L,1);
        return 1;
    }
    lua_pushnil(L);
    lua_pushinteger(L,status);
    return 2;
}

static int
luaflac_stream_decoder_finish(lua_State *L) {
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_finish(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_flush(lua_State *L) {
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_flush(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_reset(lua_State *L) {
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_reset(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_process_single(lua_State *L) {
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_process_single(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_process_until_end_of_stream(lua_State *L) {
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_process_until_end_of_stream(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_process_until_end_of_metadata(lua_State *L) {
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_process_until_end_of_metadata(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_skip_single_frame(lua_State *L) {
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_skip_single_frame(u->decoder));
    return 1;
}

static int
luaflac_stream_decoder_seek_absolute(lua_State *L) {
    luaflac_decoder_userdata *u = (luaflac_decoder_userdata *)luaL_checkudata(L,1,luaflac_stream_decoder_mt);
    lua_pushboolean(L,FLAC__stream_decoder_seek_absolute(u->decoder,lua_tointeger(L,2)));
    return 1;
}

static const struct luaL_Reg luaflac_stream_decoder_functions[] = {
    { "FLAC__stream_decoder_new", luaflac_stream_decoder_new },
    { "FLAC__stream_decoder_set_md5_checking", luaflac_stream_decoder_set_md5_checking },
    { "FLAC__stream_decoder_set_metadata_respond", luaflac_stream_decoder_set_metadata_respond },
    { "FLAC__stream_decoder_set_metadata_respond_application", luaflac_stream_decoder_set_metadata_respond_application },
    { "FLAC__stream_decoder_set_metadata_respond_all", luaflac_stream_decoder_set_metadata_respond_all },
    { "FLAC__stream_decoder_set_metadata_ignore", luaflac_stream_decoder_set_metadata_ignore },
    { "FLAC__stream_decoder_set_metadata_ignore_application", luaflac_stream_decoder_set_metadata_ignore_application },
    { "FLAC__stream_decoder_set_metadata_ignore_all", luaflac_stream_decoder_set_metadata_ignore_all },
    { "FLAC__stream_decoder_get_state", luaflac_stream_decoder_get_state },
    { "FLAC__stream_decoder_get_resolved_state_string", luaflac_stream_decoder_get_resolved_state_string },
    { "FLAC__stream_decoder_get_md5_checking", luaflac_stream_decoder_get_md5_checking },
    { "FLAC__stream_decoder_get_total_samples", luaflac_stream_decoder_get_total_samples },
    { "FLAC__stream_decoder_get_channels", luaflac_stream_decoder_get_channels },
    { "FLAC__stream_decoder_get_channel_assignment", luaflac_stream_decoder_get_channel_assignment },
    { "FLAC__stream_decoder_get_bits_per_sample", luaflac_stream_decoder_get_bits_per_sample },
    { "FLAC__stream_decoder_get_sample_rate", luaflac_stream_decoder_get_sample_rate },
    { "FLAC__stream_decoder_get_blocksize", luaflac_stream_decoder_get_blocksize },
    { "FLAC__stream_decoder_get_decode_position", luaflac_stream_decoder_get_decode_position },
    { "FLAC__stream_decoder_finish", luaflac_stream_decoder_finish },
    { "FLAC__stream_decoder_flush", luaflac_stream_decoder_flush },
    { "FLAC__stream_decoder_reset", luaflac_stream_decoder_reset },
    { "FLAC__stream_decoder_process_single", luaflac_stream_decoder_process_single },
    { "FLAC__stream_decoder_process_until_end_of_metadata", luaflac_stream_decoder_process_until_end_of_metadata },
    { "FLAC__stream_decoder_process_until_end_of_stream", luaflac_stream_decoder_process_until_end_of_stream },
    { "FLAC__stream_decoder_skip_single_frame", luaflac_stream_decoder_skip_single_frame },
    { "FLAC__stream_decoder_seek_absolute", luaflac_stream_decoder_seek_absolute },
    { NULL, NULL },
};

static const luaflac_metamethods luaflac_stream_decoder_metamethods[] = {
    { "FLAC__stream_decoder_set_ogg_serial_number" , "set_ogg_serial_number" },
    { "FLAC__stream_decoder_set_md5_checking" , "set_md5_checking" },
    { "FLAC__stream_decoder_set_metadata_respond" , "set_metadata_respond" },
    { "FLAC__stream_decoder_set_metadata_respond_application" , "set_metadata_respond_application" },
    { "FLAC__stream_decoder_set_metadata_respond_all" , "set_metadata_respond_all" },
    { "FLAC__stream_decoder_set_metadata_ignore" , "set_metadata_ignore" },
    { "FLAC__stream_decoder_set_metadata_ignore_application" , "set_metadata_ignore_application" },
    { "FLAC__stream_decoder_set_metadata_ignore_all" , "set_metadata_ignore_all" },
    { "FLAC__stream_decoder_get_state" , "get_state" },
    { "FLAC__stream_decoder_get_resolved_state_string" , "get_resolved_state_string" },
    { "FLAC__stream_decoder_get_md5_checking" , "get_md5_checking" },
    { "FLAC__stream_decoder_get_total_samples" , "get_total_samples" },
    { "FLAC__stream_decoder_get_channels" , "get_channels" },
    { "FLAC__stream_decoder_get_channel_assignment" , "get_channel_assignment" },
    { "FLAC__stream_decoder_get_bits_per_sample" , "get_bits_per_sample" },
    { "FLAC__stream_decoder_get_sample_rate" , "get_sample_rate" },
    { "FLAC__stream_decoder_get_blocksize" , "get_blocksize" },
    { "FLAC__stream_decoder_get_decode_position" , "get_decode_position" },
    { "FLAC__stream_decoder_init_stream" , "init_stream" },
    { "FLAC__stream_decoder_init_ogg_stream" , "init_ogg_stream" },
    { "FLAC__stream_decoder_init_file" , "init_file" },
    { "FLAC__stream_decoder_init_ogg_file" , "init_ogg_file" },
    { "FLAC__stream_decoder_finish" , "finish" },
    { "FLAC__stream_decoder_flush" , "flush" },
    { "FLAC__stream_decoder_reset" , "reset" },
    { "FLAC__stream_decoder_process_single" , "process_single" },
    { "FLAC__stream_decoder_process_until_end_of_stream" , "process_until_end_of_stream" },
    { "FLAC__stream_decoder_process_until_end_of_metadata" , "process_until_end_of_metadata" },
    { "FLAC__stream_decoder_skip_single_frame" , "skip_single_frame" },
    { "FLAC__stream_decoder_seek_absolute" , "seek_absolute" },
    { NULL, NULL },
};

LUAFLAC_PUBLIC
int luaopen_luaflac_stream_decoder(lua_State *L) {
    const luaflac_metamethods *m = luaflac_stream_decoder_metamethods;
    lua_getglobal(L,"require");
    lua_pushstring(L,"luaflac.uint64");
    lua_call(L,1,1);
    lua_pop(L,1);

    lua_newtable(L);

    luaflac_push_const(FLAC__STREAM_DECODER_SEARCH_FOR_METADATA);
    luaflac_push_const(FLAC__STREAM_DECODER_READ_METADATA);
    luaflac_push_const(FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC);
    luaflac_push_const(FLAC__STREAM_DECODER_READ_FRAME);
    luaflac_push_const(FLAC__STREAM_DECODER_END_OF_STREAM);
    luaflac_push_const(FLAC__STREAM_DECODER_OGG_ERROR);
    luaflac_push_const(FLAC__STREAM_DECODER_SEEK_ERROR);
    luaflac_push_const(FLAC__STREAM_DECODER_ABORTED);
    luaflac_push_const(FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR);
    luaflac_push_const(FLAC__STREAM_DECODER_UNINITIALIZED);
    luaflac_push_const(FLAC__STREAM_DECODER_INIT_STATUS_OK);
    luaflac_push_const(FLAC__STREAM_DECODER_INIT_STATUS_UNSUPPORTED_CONTAINER);
    luaflac_push_const(FLAC__STREAM_DECODER_INIT_STATUS_INVALID_CALLBACKS);
    luaflac_push_const(FLAC__STREAM_DECODER_INIT_STATUS_MEMORY_ALLOCATION_ERROR);
    luaflac_push_const(FLAC__STREAM_DECODER_INIT_STATUS_ERROR_OPENING_FILE);
    luaflac_push_const(FLAC__STREAM_DECODER_INIT_STATUS_ALREADY_INITIALIZED);
    luaflac_push_const(FLAC__STREAM_DECODER_READ_STATUS_CONTINUE);
    luaflac_push_const(FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM);
    luaflac_push_const(FLAC__STREAM_DECODER_READ_STATUS_ABORT);
    luaflac_push_const(FLAC__STREAM_DECODER_SEEK_STATUS_OK);
    luaflac_push_const(FLAC__STREAM_DECODER_SEEK_STATUS_ERROR);
    luaflac_push_const(FLAC__STREAM_DECODER_SEEK_STATUS_UNSUPPORTED);
    luaflac_push_const(FLAC__STREAM_DECODER_TELL_STATUS_OK);
    luaflac_push_const(FLAC__STREAM_DECODER_TELL_STATUS_ERROR);
    luaflac_push_const(FLAC__STREAM_DECODER_TELL_STATUS_UNSUPPORTED);
    luaflac_push_const(FLAC__STREAM_DECODER_LENGTH_STATUS_OK);
    luaflac_push_const(FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR);
    luaflac_push_const(FLAC__STREAM_DECODER_LENGTH_STATUS_UNSUPPORTED);
    luaflac_push_const(FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE);
    luaflac_push_const(FLAC__STREAM_DECODER_WRITE_STATUS_ABORT);
    luaflac_push_const(FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC);
    luaflac_push_const(FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER);
    luaflac_push_const(FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH);
    luaflac_push_const(FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM);

    luaL_setfuncs(L,luaflac_stream_decoder_functions,0);

    lua_pushlightuserdata(L, FLAC__stream_decoder_init_stream);
    lua_pushcclosure(L,luaflac_stream_decoder_init_stream,1);
    lua_setfield(L,-2,"FLAC__stream_decoder_init_stream");

    lua_pushlightuserdata(L, FLAC__stream_decoder_init_file);
    lua_pushcclosure(L,luaflac_stream_decoder_init_file,1);
    lua_setfield(L,-2,"FLAC__stream_decoder_init_file");

    if(FLAC_API_SUPPORTS_OGG_FLAC) {
        lua_pushcclosure(L,luaflac_stream_decoder_set_ogg_serial_number, 0);
        lua_setfield(L,-2, "FLAC__stream_decoder_set_ogg_serial_number");

        lua_pushlightuserdata(L, FLAC__stream_decoder_init_ogg_stream);
        lua_pushcclosure(L,luaflac_stream_decoder_init_stream,1);
        lua_setfield(L,-2,"FLAC__stream_decoder_init_ogg_stream");

        lua_pushlightuserdata(L, FLAC__stream_decoder_init_ogg_file);
        lua_pushcclosure(L,luaflac_stream_decoder_init_file,1);
        lua_setfield(L,-2,"FLAC__stream_decoder_init_ogg_file");
    } else {
        lua_pushcclosure(L,luaflac_no_ogg, 0);
        lua_setfield(L,-2, "FLAC__stream_decoder_set_ogg_serial_number");

        lua_pushcclosure(L,luaflac_no_ogg, 0);
        lua_setfield(L,-2, "FLAC__stream_decoder_init_ogg_stream");

        lua_pushcclosure(L,luaflac_no_ogg, 0);
        lua_setfield(L,-2, "FLAC__fileg_decoder_init_ogg_stream");
    }

    luaL_newmetatable(L,luaflac_stream_decoder_mt);
    lua_pushcclosure(L,luaflac_stream_decoder_delete,0);
    lua_setfield(L,-2,"__gc");

    lua_newtable(L); /* __index */

    while(m->name != NULL) {
        lua_getfield(L,-3,m->name);
        lua_setfield(L,-2,m->metaname);
        m++;
    }

    lua_setfield(L,-2,"__index");

    lua_pop(L,1);

    return 1;
}
