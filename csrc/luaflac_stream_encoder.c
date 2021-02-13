#include "luaflac-internal.h"
#include <FLAC/stream_encoder.h>
#include <FLAC/metadata.h>

#include <string.h>
#include <assert.h>

LUAFLAC_PRIVATE
const char * const luaflac_stream_encoder_mt = "FLAC__StreamEncoder";

struct luaflac_encoder_userdata_s {
    lua_State *L;
    int table_ref;
    int metadata_ref;
    FLAC__StreamEncoder *encoder;
    FLAC__StreamMetadata **metadata;
    unsigned num_blocks;
};

typedef struct luaflac_encoder_userdata_s luaflac_encoder_userdata;

static void
luaflac_stream_encoder_free_metadata(lua_State *L, luaflac_encoder_userdata *u) {
    unsigned int i = 0;
    if(u->metadata != NULL) {
        luaL_unref(L,LUA_REGISTRYINDEX,u->metadata_ref);
        u->metadata_ref = LUA_NOREF;
        while(i<u->num_blocks) {
            if(u->metadata[i] != NULL) {
                FLAC__metadata_object_delete(u->metadata[i]);
                u->metadata[i] = NULL;
            }
            i++;
        }
        u->metadata = NULL;
    }
}


static int
luaflac_stream_encoder_delete(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    if(u->encoder != NULL) {
        FLAC__stream_encoder_delete(u->encoder);
        u->encoder = NULL;
    }

    if(u->table_ref != LUA_NOREF) {
        luaL_unref(L,LUA_REGISTRYINDEX,u->table_ref);
        u->table_ref = LUA_NOREF;
    }

    luaflac_stream_encoder_free_metadata(L,u);

    return 0;
}

static int
luaflac_stream_encoder_new(lua_State *L) {
    luaflac_encoder_userdata *u = NULL;

    u = (luaflac_encoder_userdata *)lua_newuserdata(L,sizeof(luaflac_encoder_userdata));
    if(u == NULL) {
        return luaL_error(L,"out of memory");
    }

    u->L = L;
    u->metadata = NULL;
    u->metadata_ref = LUA_NOREF;
    u->num_blocks = 0;
    u->encoder = FLAC__stream_encoder_new();
    if(u->encoder == NULL) {
        return luaL_error(L,"out of memory");
    }

    lua_newtable(u->L);
    u->table_ref = luaL_ref(u->L,LUA_REGISTRYINDEX);

    luaL_setmetatable(L,luaflac_stream_encoder_mt);

    return 1;
}

static int
luaflac_stream_encoder_set_ogg_serial_number(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_ogg_serial_number(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_verify(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_verify(u->encoder,lua_toboolean(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_streamable_subset(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_streamable_subset(u->encoder,lua_toboolean(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_channels(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_channels(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_bits_per_sample(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_bits_per_sample(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_sample_rate(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_sample_rate(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_compression_level(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_compression_level(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_blocksize(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_blocksize(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_do_mid_side_stereo(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_do_mid_side_stereo(u->encoder,lua_toboolean(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_loose_mid_side_stereo(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_loose_mid_side_stereo(u->encoder,lua_toboolean(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_apodization(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_apodization(u->encoder,lua_tostring(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_max_lpc_order(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_max_lpc_order(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_qlp_coeff_precision(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_qlp_coeff_precision(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_do_qlp_coeff_prec_search(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_do_qlp_coeff_prec_search(u->encoder,lua_toboolean(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_do_escape_coding(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_do_escape_coding(u->encoder,lua_toboolean(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_do_exhaustive_model_search(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_do_exhaustive_model_search(u->encoder,lua_toboolean(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_min_residual_partition_order(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_min_residual_partition_order(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_max_residual_partition_order(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_max_residual_partition_order(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_rice_parameter_search_dist(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_rice_parameter_search_dist(u->encoder,lua_tointeger(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_total_samples_estimate(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_set_total_samples_estimate(u->encoder,luaflac_touint64(L,2)));
    return 1;
}

static int
luaflac_stream_encoder_set_metadata(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    unsigned int i = 0;

    luaflac_stream_encoder_free_metadata(L,u);

    u->num_blocks = lua_rawlen(L,2);
    u->metadata = lua_newuserdata(L,sizeof(FLAC__StreamMetadata *) * u->num_blocks);
    u->metadata_ref = luaL_ref(L,LUA_REGISTRYINDEX);

    while(i<u->num_blocks) {
        lua_rawgeti(L,2,i+1);
        u->metadata[i] = luaflac_toflac_streammetadata(L,-1);
        lua_pop(L,1);
        i++;
    }

    lua_pushboolean(L,FLAC__stream_encoder_set_metadata(u->encoder,
      u->metadata,
      u->num_blocks));
    return 1;
}


static int
luaflac_stream_encoder_get_state(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_state(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_verify_decoder_state(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_verify_decoder_state(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_resolved_state_string(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushstring(L,FLAC__stream_encoder_get_resolved_state_string(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_verify_decoder_error_stats(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    FLAC__uint64 absolute_sample = 0;
    unsigned frame_number = 0;
    unsigned channel = 0;
    unsigned sample = 0;
    FLAC__int32 expected = 0;
    FLAC__int32 got = 0;
    FLAC__stream_encoder_get_verify_decoder_error_stats(u->encoder,
      &absolute_sample,
      &frame_number,
      &channel,
      &sample,
      &expected,
      &got);
    lua_newtable(L);
    luaflac_pushuint64(L,absolute_sample);
    lua_setfield(L,-2,"absolute_sample");
    lua_pushinteger(L,frame_number);
    lua_setfield(L,-2,"frame_number");
    lua_pushinteger(L,channel);
    lua_setfield(L,-2,"channel");
    lua_pushinteger(L,sample);
    lua_setfield(L,-2,"sample");
    lua_pushinteger(L,expected);
    lua_setfield(L,-2,"expected");
    lua_pushinteger(L,got);
    lua_setfield(L,-2,"got");
    return 1;
}

static int
luaflac_stream_encoder_get_verify(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_get_verify(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_streamable_subset(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_get_streamable_subset(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_channels(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_channels(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_bits_per_sample(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_bits_per_sample(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_sample_rate(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_sample_rate(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_blocksize(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_blocksize(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_do_mid_side_stereo(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_get_do_mid_side_stereo(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_loose_mid_side_stereo(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_get_loose_mid_side_stereo(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_max_lpc_order(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_max_lpc_order(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_qlp_coeff_precision(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_qlp_coeff_precision(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_do_qlp_coeff_prec_search(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_get_do_qlp_coeff_prec_search(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_do_escape_coding(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_get_do_escape_coding(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_do_exhaustive_model_search(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_get_do_exhaustive_model_search(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_min_residual_partition_order(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_min_residual_partition_order(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_max_residual_partition_order(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_max_residual_partition_order(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_rice_parameter_search_dist(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushinteger(L,FLAC__stream_encoder_get_rice_parameter_search_dist(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_get_total_samples_estimate(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    luaflac_pushuint64(L,FLAC__stream_encoder_get_total_samples_estimate(u->encoder));
    return 1;
}

static FLAC__StreamEncoderReadStatus
luaflac_stream_encoder_read_callback(const FLAC__StreamEncoder *encoder, FLAC__byte buffer[],
  size_t *bytes, void *client_data) {
    int top;
    FLAC__StreamEncoderReadStatus status;
    const char *data = NULL;
    size_t datalen = 0;
    luaflac_encoder_userdata *u = (luaflac_encoder_userdata *)client_data;
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
        status = FLAC__STREAM_ENCODER_READ_STATUS_END_OF_STREAM;
    } else if(lua_isboolean(u->L,-1)) {
        status = lua_toboolean(u->L,-1) ? FLAC__STREAM_ENCODER_READ_STATUS_END_OF_STREAM :
          FLAC__STREAM_ENCODER_READ_STATUS_ABORT;
    } else {
        data = lua_tolstring(u->L,-1,&datalen);
        if(datalen > *bytes) {
            luaL_error(u->L,"returned more bytes than requested");
            status = FLAC__STREAM_ENCODER_READ_STATUS_ABORT;
        } else if(datalen == 0) {
            status = FLAC__STREAM_ENCODER_READ_STATUS_END_OF_STREAM;
        } else {
            memcpy(buffer,data,datalen);
            *bytes = datalen;
            status = FLAC__STREAM_ENCODER_READ_STATUS_CONTINUE;
        }
    }

    lua_pop(u->L,2);
    assert(top == lua_gettop(u->L));

    (void)encoder;
    return status;
}

static FLAC__StreamEncoderWriteStatus
luaflac_stream_encoder_write_callback(const FLAC__StreamEncoder *encoder, const FLAC__byte buffer[],
  size_t bytes, unsigned samples, unsigned current_frame, void *client_data) {
    int top;
    FLAC__StreamEncoderWriteStatus status;

    luaflac_encoder_userdata *u = (luaflac_encoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"write");
    lua_getfield(u->L,-2,"userdata");
    lua_pushlstring(u->L,(const char *)buffer,bytes);
    lua_pushinteger(u->L,samples);
    lua_pushinteger(u->L,current_frame);

    lua_call(u->L,4,1);

    /* return values: boolean true/false for OK/ERROR */

    status = lua_toboolean(u->L,-1) ? FLAC__STREAM_ENCODER_WRITE_STATUS_OK :
      FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
    lua_pop(u->L,2);

    assert(top == lua_gettop(u->L));
    (void)encoder;
    return status;
}

static FLAC__StreamEncoderSeekStatus
luaflac_stream_encoder_seek_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 absolute_byte_offset,
  void *client_data) {
    int top;
    FLAC__uint64 *t = NULL;
    FLAC__StreamEncoderSeekStatus status;
    luaflac_encoder_userdata *u = (luaflac_encoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"seek");
    lua_getfield(u->L,-2,"userdata");
    t = lua_newuserdata(u->L,sizeof(FLAC__uint64));
    if(t == NULL) {
        luaL_error(u->L,"out of memory");
        return FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;
    }
    *t = absolute_byte_offset;
    luaL_setmetatable(u->L,luaflac_uint64_mt);

    lua_call(u->L,2,1);

    /* true: seek OK
     * false: seek error */

    /* assumption is if seeking is not supported, then
     * the seek callback should be nil */

    if(lua_isnil(u->L,-1)) {
        status = FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;
    } else {
        status = lua_toboolean(u->L,-1) ? FLAC__STREAM_ENCODER_SEEK_STATUS_OK :
          FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;
    }
    lua_pop(u->L,2);
    assert(top == lua_gettop(u->L));

    (void)encoder;
    return status;
}

static FLAC__StreamEncoderTellStatus
luaflac_stream_encoder_tell_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 *absolute_byte_offset,
  void *client_data) {
    int top;
    FLAC__StreamEncoderTellStatus status;
    luaflac_encoder_userdata *u = (luaflac_encoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"tell");
    lua_getfield(u->L,-2,"userdata");

    lua_call(u->L,1,1);

    /* true: tell OK
     * false: tell error */

    /* assumption is if tell is not supported, then
     * the tell callback should be nil */

    if(lua_isnumber(u->L,-1)) {
        status = FLAC__STREAM_ENCODER_TELL_STATUS_OK;
        *absolute_byte_offset = luaflac_touint64(u->L,-1);
    } else {
        status = FLAC__STREAM_ENCODER_TELL_STATUS_ERROR;
    }
    lua_pop(u->L,2);
    assert(top == lua_gettop(u->L));

    (void)encoder;
    return status;
}

static void
luaflac_stream_encoder_progress_callback(const FLAC__StreamEncoder *encoder,
  FLAC__uint64 bytes_written,
  FLAC__uint64 samples_written,
  unsigned frames_written,
  unsigned total_frames_estimate,
  void *client_data) {

    int top;
    luaflac_encoder_userdata *u = (luaflac_encoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"progress");
    lua_getfield(u->L,-2,"userdata");
    luaflac_pushuint64(u->L,bytes_written);
    luaflac_pushuint64(u->L,samples_written);
    lua_pushinteger(u->L,frames_written);
    lua_pushinteger(u->L,total_frames_estimate);
    lua_call(u->L,5,0);
    
    lua_pop(u->L,1);
    assert(top == lua_gettop(u->L));
    (void)encoder;
    return;
}

static void
luaflac_stream_encoder_metadata_callback(const FLAC__StreamEncoder *encoder,
  const FLAC__StreamMetadata *metadata,
  void *client_data) {
    int top;
    luaflac_encoder_userdata *u = (luaflac_encoder_userdata *)client_data;
    top = lua_gettop(u->L);

    lua_rawgeti(u->L,LUA_REGISTRYINDEX,u->table_ref);
    lua_getfield(u->L,-1,"metadata");
    lua_getfield(u->L,-2,"userdata");

    luaflac_pushstreammetadata(u->L,metadata);

    lua_call(u->L,2,0);

    lua_pop(u->L,1);
    assert(top == lua_gettop(u->L));
    (void)encoder;
}

static int
luaflac_stream_encoder_init_stream(lua_State *L) {

    luaflac_encoder_userdata *u = NULL;
    FLAC__StreamEncoderWriteCallback write_callback = NULL;
    FLAC__StreamEncoderSeekCallback seek_callback = NULL;
    FLAC__StreamEncoderTellCallback tell_callback = NULL;
    FLAC__StreamEncoderMetadataCallback metadata_callback = NULL;
    FLAC__StreamEncoderInitStatus status = 0;
    int top = lua_gettop(L);

    if(!lua_istable(L,2)) {
        return luaL_error(L,"missing parameter table");
    }

    u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_rawgeti(L,LUA_REGISTRYINDEX,u->table_ref);

    lua_getfield(L,2,"write");
    if(lua_isfunction(L,-1)) {
        write_callback = luaflac_stream_encoder_write_callback;
        lua_setfield(L,-2,"write");
    }
    else {
        return luaL_error(L,"write callback must not be nil");
    }

    lua_getfield(L,2,"seek");
    if(lua_isfunction(L,-1)) {
        seek_callback = luaflac_stream_encoder_seek_callback;
        lua_setfield(L,-2,"seek");

        /* if seek callback is defined, then tell is required */
        lua_getfield(L,2,"tell");
        if(!lua_isfunction(L,-1)) {
            return luaL_error(L,"missing tell callback");
        }
        tell_callback = luaflac_stream_encoder_tell_callback;
        lua_setfield(L,-2,"tell");
    } else {
        lua_pop(L,1);
    }

    lua_getfield(L,2,"metadata");
    if(lua_isfunction(L,-1)) {
        metadata_callback = luaflac_stream_encoder_metadata_callback;
        lua_setfield(L,-2,"metadata");
    }
    else {
        lua_pop(L,1);
    }

    lua_getfield(L,2,"userdata");
    lua_setfield(L,-2,"userdata");

    status = FLAC__stream_encoder_init_stream(u->encoder,
      write_callback,
      seek_callback,
      tell_callback,
      metadata_callback,
      u);

    lua_pop(L,1);
    lua_pushinteger(L,status);
    assert(lua_gettop(L) == top + 1);
    return 1;
}

static int
luaflac_stream_encoder_init_ogg_stream(lua_State *L) {
    luaflac_encoder_userdata *u = NULL;
    FLAC__StreamEncoderReadCallback read_callback = NULL;
    FLAC__StreamEncoderWriteCallback write_callback = NULL;
    FLAC__StreamEncoderSeekCallback seek_callback = NULL;
    FLAC__StreamEncoderTellCallback tell_callback = NULL;
    FLAC__StreamEncoderMetadataCallback metadata_callback = NULL;
    FLAC__StreamEncoderInitStatus status = 0;
    int top = lua_gettop(L);

    if(!lua_istable(L,2)) {
        return luaL_error(L,"missing parameter table");
    }

    u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_rawgeti(L,LUA_REGISTRYINDEX,u->table_ref);

    lua_getfield(L,2,"write");
    if(lua_isfunction(L,-1)) {
        write_callback = luaflac_stream_encoder_write_callback;
        lua_setfield(L,-2,"write");
    }
    else {
        return luaL_error(L,"write callback must not be nil");
    }

    lua_getfield(L,2,"seek");
    if(lua_isfunction(L,-1)) {
        seek_callback = luaflac_stream_encoder_seek_callback;
        lua_setfield(L,-2,"seek");

        /* if seek callback is defined, then read is required */
        lua_getfield(L,2,"read");
        if(!lua_isfunction(L,-1)) {
            return luaL_error(L,"missing read callback");
        }
        read_callback = luaflac_stream_encoder_read_callback;
        lua_setfield(L,-2,"read");

        /* if seek callback is defined, then tell is required */
        lua_getfield(L,2,"tell");
        if(!lua_isfunction(L,-1)) {
            return luaL_error(L,"missing tell callback");
        }
        tell_callback = luaflac_stream_encoder_tell_callback;
        lua_setfield(L,-2,"tell");
    } else {
        lua_pop(L,1);
    }

    lua_getfield(L,2,"metadata");
    if(lua_isfunction(L,-1)) {
        metadata_callback = luaflac_stream_encoder_metadata_callback;
        lua_setfield(L,-2,"metadata");
    }
    else {
        lua_pop(L,1);
    }

    lua_getfield(L,2,"userdata");
    lua_setfield(L,-2,"userdata");

    status = FLAC__stream_encoder_init_ogg_stream(u->encoder,
      read_callback,
      write_callback,
      seek_callback,
      tell_callback,
      metadata_callback,
      u);

    lua_pop(L,1);
    lua_pushinteger(L,status);
    assert(lua_gettop(L) == top + 1);
    return 1;
}

static int
luaflac_stream_encoder_init_file(lua_State *L) {
    FLAC__StreamEncoderInitStatus (*init_file)(FLAC__StreamEncoder *, const char *,
      FLAC__StreamEncoderProgressCallback,
      void *) = NULL;

    luaflac_encoder_userdata *u = NULL;
    const char *filename = NULL;
    FLAC__StreamEncoderProgressCallback progress_callback = NULL;
    FLAC__StreamEncoderInitStatus status = 0;

    init_file = lua_touserdata(L,lua_upvalueindex(1));

    u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_rawgeti(L,LUA_REGISTRYINDEX,u->table_ref);

    if(!lua_istable(L,2)) {
        return luaL_error(L,"missing required parameter table");
    }

    lua_getfield(L,2,"filename");
    if(lua_isfunction(L,-1)) {
        filename = lua_tostring(L,-1);
    } else {
        return luaL_error(L,"filename must not be nil");
    }
    lua_pop(L,1);

    lua_getfield(L,2,"progress");
    if(lua_isfunction(L,-1)) {
        progress_callback = luaflac_stream_encoder_progress_callback;
        lua_setfield(L,-2,"progress");
    }
    else {
        lua_pop(L,1);
    }

    lua_getfield(L,2,"userdata");
    lua_setfield(L,-2,"userdata");

    status = init_file(u->encoder,
      filename,
      progress_callback,
      u);

    lua_pop(L,1);
    lua_pushinteger(L,status);
    return 1;
}

static int
luaflac_stream_encoder_finish(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    lua_pushboolean(L,FLAC__stream_encoder_finish(u->encoder));
    return 1;
}

static int
luaflac_stream_encoder_process(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    unsigned int channels = lua_rawlen(L,2);
    unsigned int samples = 0;
    unsigned int c = 0;
    unsigned int s = 0;
    FLAC__int32 **buffer = NULL;

    buffer = lua_newuserdata(L,sizeof(FLAC__int32 *) * channels);
    if(buffer == NULL) {
        return luaL_error(L,"out of memory");
    }
    lua_pop(L,1);

    while(c<channels) {
        lua_rawgeti(L,2,c+1);

        if(c == 0) {
            samples = lua_rawlen(L,-1);
        } else {
            if(samples != lua_rawlen(L,-1)) {
                return luaL_error(L,"mis-matched channel buffers");
            }
        }

        buffer[c] = lua_newuserdata(L,sizeof(FLAC__int32) * samples);
        if(buffer[c] == NULL) {
            return luaL_error(L,"out of memory");
        }
        lua_pop(L,1);

        s = 0;
        while(s<samples) {
            lua_rawgeti(L,-1,s+1);
            buffer[c][s] = lua_tointeger(L,-1);
            lua_pop(L,1);
            s++;
        }
        lua_pop(L,1);
        c++;
    }

    lua_pushboolean(L,FLAC__stream_encoder_process(u->encoder,
      (const FLAC__int32 *const *)buffer,
      samples));
    return 1;
}

static int
luaflac_stream_encoder_process_interleaved(lua_State *L) {
    luaflac_encoder_userdata *u = luaL_checkudata(L,1,luaflac_stream_encoder_mt);
    unsigned int channels = FLAC__stream_encoder_get_channels(u->encoder);
    unsigned int s = lua_rawlen(L,2);
    unsigned int samples = s / channels;
    unsigned int c = 0;
    FLAC__int32 *buffer = NULL;

    buffer = lua_newuserdata(L,sizeof(FLAC__int32) * channels * samples);
    if(buffer == NULL) {
        return luaL_error(L,"out of memory");
    }
    lua_pop(L,1);

    while(c<s) {
        lua_rawgeti(L,2,c+1);
        buffer[c] = lua_tointeger(L,-1);
        lua_pop(L,1);
        c++;
    }

    lua_pushboolean(L,FLAC__stream_encoder_process_interleaved(u->encoder,
      (const FLAC__int32 *)buffer,
      samples));
    return 1;
}

static const struct luaL_Reg luaflac_stream_encoder_functions[] = {
    { "FLAC__stream_encoder_new", luaflac_stream_encoder_new },
    { "FLAC__stream_encoder_set_verify", luaflac_stream_encoder_set_verify },
    { "FLAC__stream_encoder_set_streamable_subset", luaflac_stream_encoder_set_streamable_subset },
    { "FLAC__stream_encoder_set_channels", luaflac_stream_encoder_set_channels },
    { "FLAC__stream_encoder_set_bits_per_sample", luaflac_stream_encoder_set_bits_per_sample },
    { "FLAC__stream_encoder_set_sample_rate", luaflac_stream_encoder_set_sample_rate },
    { "FLAC__stream_encoder_set_compression_level", luaflac_stream_encoder_set_compression_level },
    { "FLAC__stream_encoder_set_blocksize", luaflac_stream_encoder_set_blocksize },
    { "FLAC__stream_encoder_set_do_mid_side_stereo", luaflac_stream_encoder_set_do_mid_side_stereo },
    { "FLAC__stream_encoder_set_loose_mid_side_stereo", luaflac_stream_encoder_set_loose_mid_side_stereo },
    { "FLAC__stream_encoder_set_apodization", luaflac_stream_encoder_set_apodization },
    { "FLAC__stream_encoder_set_max_lpc_order", luaflac_stream_encoder_set_max_lpc_order },
    { "FLAC__stream_encoder_set_qlp_coeff_precision", luaflac_stream_encoder_set_qlp_coeff_precision },
    { "FLAC__stream_encoder_set_do_qlp_coeff_prec_search", luaflac_stream_encoder_set_do_qlp_coeff_prec_search },
    { "FLAC__stream_encoder_set_do_escape_coding", luaflac_stream_encoder_set_do_escape_coding },
    { "FLAC__stream_encoder_set_do_exhaustive_model_search", luaflac_stream_encoder_set_do_exhaustive_model_search },
    { "FLAC__stream_encoder_set_min_residual_partition_order", luaflac_stream_encoder_set_min_residual_partition_order },
    { "FLAC__stream_encoder_set_max_residual_partition_order", luaflac_stream_encoder_set_max_residual_partition_order },
    { "FLAC__stream_encoder_set_rice_parameter_search_dist", luaflac_stream_encoder_set_rice_parameter_search_dist },
    { "FLAC__stream_encoder_set_total_samples_estimate", luaflac_stream_encoder_set_total_samples_estimate },
    { "FLAC__stream_encoder_set_metadata", luaflac_stream_encoder_set_metadata },
    { "FLAC__stream_encoder_get_state", luaflac_stream_encoder_get_state },
    { "FLAC__stream_encoder_get_verify_decoder_state", luaflac_stream_encoder_get_verify_decoder_state },
    { "FLAC__stream_encoder_get_resolved_state_string", luaflac_stream_encoder_get_resolved_state_string },
    { "FLAC__stream_encoder_get_verify_decoder_error_stats", luaflac_stream_encoder_get_verify_decoder_error_stats },
    { "FLAC__stream_encoder_get_verify", luaflac_stream_encoder_get_verify },
    { "FLAC__stream_encoder_get_streamable_subset", luaflac_stream_encoder_get_streamable_subset },
    { "FLAC__stream_encoder_get_channels", luaflac_stream_encoder_get_channels },
    { "FLAC__stream_encoder_get_bits_per_sample", luaflac_stream_encoder_get_bits_per_sample },
    { "FLAC__stream_encoder_get_sample_rate", luaflac_stream_encoder_get_sample_rate },
    { "FLAC__stream_encoder_get_blocksize", luaflac_stream_encoder_get_blocksize },
    { "FLAC__stream_encoder_get_do_mid_side_stereo", luaflac_stream_encoder_get_do_mid_side_stereo },
    { "FLAC__stream_encoder_get_loose_mid_side_stereo", luaflac_stream_encoder_get_loose_mid_side_stereo },
    { "FLAC__stream_encoder_get_max_lpc_order", luaflac_stream_encoder_get_max_lpc_order },
    { "FLAC__stream_encoder_get_qlp_coeff_precision", luaflac_stream_encoder_get_qlp_coeff_precision },
    { "FLAC__stream_encoder_get_do_qlp_coeff_prec_search", luaflac_stream_encoder_get_do_qlp_coeff_prec_search },
    { "FLAC__stream_encoder_get_do_escape_coding", luaflac_stream_encoder_get_do_escape_coding },
    { "FLAC__stream_encoder_get_do_exhaustive_model_search", luaflac_stream_encoder_get_do_exhaustive_model_search },
    { "FLAC__stream_encoder_get_min_residual_partition_order", luaflac_stream_encoder_get_min_residual_partition_order },
    { "FLAC__stream_encoder_get_max_residual_partition_order", luaflac_stream_encoder_get_max_residual_partition_order },
    { "FLAC__stream_encoder_get_rice_parameter_search_dist", luaflac_stream_encoder_get_rice_parameter_search_dist },
    { "FLAC__stream_encoder_get_total_samples_estimate", luaflac_stream_encoder_get_total_samples_estimate },
    { "FLAC__stream_encoder_init_stream" , luaflac_stream_encoder_init_stream },
    { "FLAC__stream_encoder_finish", luaflac_stream_encoder_finish },
    { "FLAC__stream_encoder_process", luaflac_stream_encoder_process },
    { "FLAC__stream_encoder_process_interleaved", luaflac_stream_encoder_process_interleaved },

    { NULL, NULL },
};

static const luaflac_metamethods luaflac_stream_encoder_metamethods[] = {
    { "FLAC__stream_encoder_set_ogg_serial_number" , "set_ogg_serial_number" },
    { "FLAC__stream_encoder_set_verify" , "set_verify" },
    { "FLAC__stream_encoder_set_streamable_subset" , "set_streamable_subset" },
    { "FLAC__stream_encoder_set_channels" , "set_channels" },
    { "FLAC__stream_encoder_set_bits_per_sample" , "set_bits_per_sample" },
    { "FLAC__stream_encoder_set_sample_rate" , "set_sample_rate" },
    { "FLAC__stream_encoder_set_compression_level" , "set_compression_level" },
    { "FLAC__stream_encoder_set_blocksize" , "set_blocksize" },
    { "FLAC__stream_encoder_set_do_mid_side_stereo" , "set_do_mid_side_stereo" },
    { "FLAC__stream_encoder_set_loose_mid_side_stereo" , "set_loose_mid_side_stereo" },
    { "FLAC__stream_encoder_set_apodization" , "set_apodization" },
    { "FLAC__stream_encoder_set_max_lpc_order" , "set_max_lpc_order" },
    { "FLAC__stream_encoder_set_qlp_coeff_precision" , "set_qlp_coeff_precision" },
    { "FLAC__stream_encoder_set_do_qlp_coeff_prec_search" , "set_do_qlp_coeff_prec_search" },
    { "FLAC__stream_encoder_set_do_escape_coding" , "set_do_escape_coding" },
    { "FLAC__stream_encoder_set_do_exhaustive_model_search" , "set_do_exhaustive_model_search" },
    { "FLAC__stream_encoder_set_min_residual_partition_order" , "set_min_residual_partition_order" },
    { "FLAC__stream_encoder_set_max_residual_partition_order" , "set_max_residual_partition_order" },
    { "FLAC__stream_encoder_set_rice_parameter_search_dist" , "set_rice_parameter_search_dist" },
    { "FLAC__stream_encoder_set_total_samples_estimate" , "set_total_samples_estimate" },
    { "FLAC__stream_encoder_set_metadata" , "set_metadata" },
    { "FLAC__stream_encoder_get_state" , "get_state" },
    { "FLAC__stream_encoder_get_verify_decoder_state" , "get_verify_decoder_state" },
    { "FLAC__stream_encoder_get_resolved_state_string" , "get_resolved_state_string" },
    { "FLAC__stream_encoder_get_verify_decoder_error_stats" , "get_verify_decoder_error_stats" },
    { "FLAC__stream_encoder_get_verify" , "get_verify" },
    { "FLAC__stream_encoder_get_streamable_subset" , "get_streamable_subset" },
    { "FLAC__stream_encoder_get_channels" , "get_channels" },
    { "FLAC__stream_encoder_get_bits_per_sample" , "get_bits_per_sample" },
    { "FLAC__stream_encoder_get_sample_rate" , "get_sample_rate" },
    { "FLAC__stream_encoder_get_blocksize" , "get_blocksize" },
    { "FLAC__stream_encoder_get_do_mid_side_stereo" , "get_do_mid_side_stereo" },
    { "FLAC__stream_encoder_get_loose_mid_side_stereo" , "get_loose_mid_side_stereo" },
    { "FLAC__stream_encoder_get_max_lpc_order" , "get_max_lpc_order" },
    { "FLAC__stream_encoder_get_qlp_coeff_precision" , "get_qlp_coeff_precision" },
    { "FLAC__stream_encoder_get_do_qlp_coeff_prec_search" , "get_do_qlp_coeff_prec_search" },
    { "FLAC__stream_encoder_get_do_escape_coding" , "get_do_escape_coding" },
    { "FLAC__stream_encoder_get_do_exhaustive_model_search" , "get_do_exhaustive_model_search" },
    { "FLAC__stream_encoder_get_min_residual_partition_order" , "get_min_residual_partition_order" },
    { "FLAC__stream_encoder_get_max_residual_partition_order" , "get_max_residual_partition_order" },
    { "FLAC__stream_encoder_get_rice_parameter_search_dist" , "get_rice_parameter_search_dist" },
    { "FLAC__stream_encoder_get_total_samples_estimate" , "get_total_samples_estimate" },
    { "FLAC__stream_encoder_init_stream" , "init_stream" },
    { "FLAC__stream_encoder_init_ogg_stream" , "init_ogg_stream" },
    { "FLAC__stream_encoder_init_file" , "init_file" },
    { "FLAC__stream_encoder_init_ogg_file" , "init_ogg_file" },
    { "FLAC__stream_encoder_finish" , "finish" },
    { "FLAC__stream_encoder_process" , "process" },
    { "FLAC__stream_encoder_process_interleaved" , "process_interleaved" },
    { NULL, NULL },
};

LUAFLAC_PUBLIC
int luaopen_luaflac_stream_encoder(lua_State *L) {
    const luaflac_metamethods *m = luaflac_stream_encoder_metamethods;
    lua_getglobal(L,"require");
    lua_pushstring(L,"luaflac.uint64");
    lua_call(L,1,1);
    lua_pop(L,1);

    lua_newtable(L);

    luaflac_push_const(FLAC__STREAM_ENCODER_OK);
    luaflac_push_const(FLAC__STREAM_ENCODER_UNINITIALIZED);
    luaflac_push_const(FLAC__STREAM_ENCODER_OGG_ERROR);
    luaflac_push_const(FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR);
    luaflac_push_const(FLAC__STREAM_ENCODER_VERIFY_MISMATCH_IN_AUDIO_DATA);
    luaflac_push_const(FLAC__STREAM_ENCODER_CLIENT_ERROR);
    luaflac_push_const(FLAC__STREAM_ENCODER_IO_ERROR);
    luaflac_push_const(FLAC__STREAM_ENCODER_FRAMING_ERROR);
    luaflac_push_const(FLAC__STREAM_ENCODER_MEMORY_ALLOCATION_ERROR);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_OK);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_UNSUPPORTED_CONTAINER);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_CALLBACKS);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_NUMBER_OF_CHANNELS);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BITS_PER_SAMPLE);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_SAMPLE_RATE);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BLOCK_SIZE);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_MAX_LPC_ORDER);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_QLP_COEFF_PRECISION);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_NOT_STREAMABLE);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA);
    luaflac_push_const(FLAC__STREAM_ENCODER_INIT_STATUS_ALREADY_INITIALIZED);
    luaflac_push_const(FLAC__STREAM_ENCODER_READ_STATUS_CONTINUE);
    luaflac_push_const(FLAC__STREAM_ENCODER_READ_STATUS_END_OF_STREAM);
    luaflac_push_const(FLAC__STREAM_ENCODER_READ_STATUS_ABORT);
    luaflac_push_const(FLAC__STREAM_ENCODER_READ_STATUS_UNSUPPORTED);
    luaflac_push_const(FLAC__STREAM_ENCODER_WRITE_STATUS_OK);
    luaflac_push_const(FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR);
    luaflac_push_const(FLAC__STREAM_ENCODER_SEEK_STATUS_OK);
    luaflac_push_const(FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR);
    luaflac_push_const(FLAC__STREAM_ENCODER_SEEK_STATUS_UNSUPPORTED);
    luaflac_push_const(FLAC__STREAM_ENCODER_TELL_STATUS_OK);
    luaflac_push_const(FLAC__STREAM_ENCODER_TELL_STATUS_ERROR);
    luaflac_push_const(FLAC__STREAM_ENCODER_TELL_STATUS_UNSUPPORTED);

    luaL_setfuncs(L,luaflac_stream_encoder_functions,0);

    lua_pushlightuserdata(L,FLAC__stream_encoder_init_file);
    lua_pushcclosure(L,luaflac_stream_encoder_init_file,1);
    lua_setfield(L,-2,"FLAC__stream_encoder_init_file");

    if(FLAC_API_SUPPORTS_OGG_FLAC) {
        lua_pushcclosure(L,  luaflac_stream_encoder_set_ogg_serial_number, 0);
        lua_setfield(L,-2, "FLAC__stream_encoder_set_ogg_serial_number");

        lua_pushlightuserdata(L,FLAC__stream_encoder_init_ogg_file);
        lua_pushcclosure(L,luaflac_stream_encoder_init_file,1);
        lua_setfield(L,-2,"FLAC__stream_encoder_init_ogg_file");

        lua_pushcclosure(L,luaflac_stream_encoder_init_ogg_stream,0);
        lua_setfield(L,-2,"FLAC__stream_encoder_init_ogg_stream");

    } else {
        lua_pushcclosure(L, luaflac_no_ogg, 0);
        lua_setfield(L,-2, "FLAC__stream_encoder_set_ogg_serial_number");

        lua_pushcclosure(L, luaflac_no_ogg, 0);
        lua_setfield(L,-2, "FLAC__stream_encoder_init_ogg_file");

        lua_pushcclosure(L, luaflac_no_ogg, 0);
        lua_setfield(L,-2, "FLAC__stream_encoder_init_ogg_stream");
    }


    luaL_newmetatable(L,luaflac_stream_encoder_mt);
    lua_pushcclosure(L,luaflac_stream_encoder_delete,0);
    lua_setfield(L,-2,"__gc");

    lua_newtable(L);

    while(m->name != NULL) {
        lua_getfield(L,-3,m->name);
        lua_setfield(L,-2,m->metaname);
        m++;
    }

    lua_setfield(L,-2,"__index");

    lua_pop(L,1);
    return 1;
}
