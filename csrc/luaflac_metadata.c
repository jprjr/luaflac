#include "luaflac-internal.h"
#include <FLAC/metadata.h>
#include <string.h>
#include <assert.h>

/* internal methods for converting a lua table to a FLAC__StreamMetadata object */
/* most fields optional */

static int
luaflac_toflac_streammetadata_streaminfo(lua_State *L, int idx, FLAC__StreamMetadata *m) {
    int top = lua_gettop(L);
    const char *md5sum = NULL;
    size_t md5sumlen = 0;

    lua_getfield(L,idx,"stream_info");
    if(!lua_istable(L,-1)) {
        lua_pop(L,1);
        lua_pushliteral(L,"missing key: stream_info");
        return 1;
    }

    lua_getfield(L,-1,"min_blocksize");
    if(!lua_isnil(L,-1)) {
        m->data.stream_info.min_blocksize = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"max_blocksize");
    if(!lua_isnil(L,-1)) {
        m->data.stream_info.max_blocksize = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"min_framesize");
    if(!lua_isnil(L,-1)) {
        m->data.stream_info.min_framesize = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"max_framesize");
    if(!lua_isnil(L,-1)) {
        m->data.stream_info.max_framesize = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"sample_rate");
    if(!lua_isnil(L,-1)) {
        m->data.stream_info.sample_rate = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"channels");
    if(!lua_isnil(L,-1)) {
        m->data.stream_info.channels = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"bits_per_sample");
    if(!lua_isnil(L,-1)) {
        m->data.stream_info.bits_per_sample = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"total_samples");
    if(!lua_isnil(L,-1)) {
        m->data.stream_info.total_samples = luaflac_touint64(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"md5sum");
    if(!lua_isnil(L,-1)) {
        md5sum = luaL_checklstring(L,-1,&md5sumlen);
        if(md5sumlen != 16) {
            lua_pop(L,2);
            lua_pushliteral(L,"incorrect md5sum length");
            return 1;
        }
        memcpy(m->data.stream_info.md5sum,md5sum,16);
    }
    lua_pop(L,1);

    lua_pop(L,1);
    assert(top == lua_gettop(L));
    return 0;
}


static int
luaflac_toflac_streammetadata_padding(lua_State *L, int idx, FLAC__StreamMetadata *m) {
    int top = lua_gettop(L);
    lua_getfield(L,idx,"padding");
    if(!lua_istable(L,-1)) {
        lua_pop(L,1);
        lua_pushliteral(L,"missing key: padding");
        return 1;
    }

    lua_getfield(L,-1,"dummy");
    if(!lua_isnil(L,-1)) {
        m->data.padding.dummy = lua_tointeger(L,-1);
    }
    lua_pop(L,2);

    lua_getfield(L,-1,"length");
    m->length = lua_tointeger(L,-1);
    lua_pop(L,1);
    assert(top == lua_gettop(L));
    return 0;
}

/* all fields requires for the application block */
static int
luaflac_toflac_streammetadata_application(lua_State *L, int idx, FLAC__StreamMetadata *m) {
    int top = lua_gettop(L);
    const char *id = NULL;
    const char *data = NULL;
    size_t idlen = 0;
    size_t datalen = 0;

    lua_getfield(L,idx,"application");
    if(!lua_istable(L,-1)) {
        lua_pop(L,1);
        lua_pushliteral(L,"missing key: application");
        return 1;
    }

    lua_getfield(L,-1,"id");
    id = luaL_checklstring(L,-1,&idlen); lua_pop(L,1);

    if(idlen < 4) {
        lua_pop(L,1);
        lua_pushliteral(L,"incorrect id length");
        return 1;
    }

    memcpy(m->data.application.id,id,4);

    lua_getfield(L,-1,"data");
    data = luaL_checklstring(L,-1,&datalen); lua_pop(L,1);

    if(!FLAC__metadata_object_application_set_data(m,(FLAC__byte *)data,datalen,1)) {
        lua_pop(L,1);
        lua_pushliteral(L,"error setting application data");
        return 1;
    }

    lua_pop(L,1);
    assert(top == lua_gettop(L));
    return 0;
}

/* all fields requires for the seek_table */
static int
luaflac_toflac_streammetadata_seek_table(lua_State *L, int idx, FLAC__StreamMetadata *m) {
    int top = lua_gettop(L);
    unsigned int i = 0;
    unsigned num_points = 0;
    FLAC__StreamMetadata_SeekPoint point;

    lua_getfield(L,idx,"seek_table");
    if(!lua_istable(L,-1)) {
        lua_pop(L,1);
        lua_pushliteral(L,"missing key: seek_table");
        return 1;
    }

    lua_getfield(L,-1,"points");
    if(!lua_istable(L,-1)) {
        lua_pop(L,2);
        lua_pushliteral(L,"missing key: points");
        return 1;
    }

    num_points = lua_rawlen(L,-1);
    if(!FLAC__metadata_object_seektable_resize_points(m,num_points)) {
        lua_pop(L,2);
        lua_pushliteral(L,"error resizing points");
        return 1;
    }

    while(i<num_points) {
        lua_rawgeti(L,-1,i+1);
        lua_getfield(L,-1,"sample_number");
        point.sample_number = luaflac_touint64(L,-1); lua_pop(L,1);
        lua_getfield(L,-1,"stream_offset");
        point.stream_offset = luaflac_touint64(L,-1); lua_pop(L,1);
        lua_getfield(L,-1,"frame_samples");
        point.frame_samples = luaL_checkinteger(L,-1); lua_pop(L,1);

        FLAC__metadata_object_seektable_set_point(m,i,point);
        lua_pop(L,1);
        i++;
    }

    assert(m->data.seek_table.num_points == num_points);

    lua_pop(L,2);
    assert(top == lua_gettop(L));
    return 0;
}

static int
luaflac_toflac_streammetadata_vorbis_comment(lua_State *L, int idx, FLAC__StreamMetadata *m) {
    int top = lua_gettop(L);
    unsigned int i = 0;
    const char *entry = NULL;
    size_t entrylen = 0;
    FLAC__uint32 num_comments = 0;
    FLAC__StreamMetadata_VorbisComment_Entry vc_entry;

    lua_getfield(L,idx,"vorbis_comment");
    if(!lua_istable(L,-1)) {
        lua_pop(L,1);
        lua_pushliteral(L,"missing key: vorbis_comment");
        return 1;
    }

    lua_getfield(L,-1,"vendor_string");
    if(!lua_isnil(L,-1)) {
        entry = lua_tolstring(L,-1,&entrylen);
        if(entry != NULL) {
            vc_entry.entry = (FLAC__byte *)entry;
            vc_entry.length = (FLAC__uint32)entrylen;
            if(!FLAC__metadata_object_vorbiscomment_set_vendor_string(m,vc_entry,1)) {
                lua_pop(L,2);
                lua_pushliteral(L,"error setting vendor string");
                return 1;
            }
        }
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"comments");
    if(!lua_isnil(L,-1)) {
        num_comments = lua_rawlen(L,-1);
        if(!FLAC__metadata_object_vorbiscomment_resize_comments(m,num_comments)) {
            lua_pop(L,2);
            lua_pushliteral(L,"error resizing comments");
            return 1;
        }

        while(i<num_comments) {
            lua_rawgeti(L,-1,i+1);
            entry = lua_tolstring(L,-1,&entrylen);
            if(entry == NULL) {
                lua_pop(L,3);
                lua_pushliteral(L,"entry not a string");
                return 1;
            }
            vc_entry.entry = (FLAC__byte *)entry;
            vc_entry.length = (FLAC__uint32)entrylen;
            if(!FLAC__metadata_object_vorbiscomment_set_comment(m,i,vc_entry,1)) {
                lua_pop(L,3);
                lua_pushliteral(L,"error calling set_comment");
                return 1;
            }
            lua_pop(L,1);
            i++;
        }
    }
    lua_pop(L,2);
    assert(top == lua_gettop(L));
    return 0;
}

static int
luaflac_toflac_streammetadata_cue_sheet(lua_State *L, int idx, FLAC__StreamMetadata *m) {
    int top = lua_gettop(L);
    const char *str = NULL;
    size_t strlen = 0;
    unsigned num_tracks = 0;
    unsigned int i = 0;
    unsigned int j = 0;
    FLAC__byte num_indices = 0;
    FLAC__StreamMetadata_CueSheet_Track *track = NULL;

    lua_getfield(L,idx,"cue_sheet");
    if(!lua_istable(L,-1)) {
        lua_pop(L,1);
        lua_pushliteral(L,"missing key: cue_sheet");
        return 1;
    }

    lua_getfield(L,-1,"media_catalog_number");
    if(!lua_isnil(L,-1)) {
        str = lua_tolstring(L,-1,&strlen);
        if(strlen > 128) {
            lua_pop(L,2);
            lua_pushliteral(L,"media_catalog_number: length > 128");
            return 1;
        }
        memcpy(m->data.cue_sheet.media_catalog_number,str,strlen);
        m->data.cue_sheet.media_catalog_number[strlen] = 0;
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"lead_in");
    if(!lua_isnil(L,-1)) {
        m->data.cue_sheet.lead_in = luaflac_touint64(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"is_cd");
    if(!lua_isnil(L,-1)) {
        m->data.cue_sheet.is_cd = lua_toboolean(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"tracks");
    if(!lua_isnil(L,-1)) {
        num_tracks = lua_rawlen(L,-1);

        if(!FLAC__metadata_object_cuesheet_resize_tracks(m,num_tracks)) {
            lua_pop(L,2);
            lua_pushliteral(L,"error resizing tracks");
            return 1;
        }
        while(i<num_tracks) {
            lua_rawgeti(L,-1,i+1);
            track = FLAC__metadata_object_cuesheet_track_new();

            lua_getfield(L,-1,"offset");
            if(!lua_isnil(L,-1)) {
                track->offset = luaflac_touint64(L,-1);
            }
            lua_pop(L,1);

            lua_getfield(L,-1,"number");
            if(!lua_isnil(L,-1)) {
                track->number = lua_tointeger(L,-1);
            }
            lua_pop(L,1);

            lua_getfield(L,-1,"isrc");
            if(!lua_isnil(L,-1)) {
                str = lua_tolstring(L,-1,&strlen);
                if(strlen > 12) {
                    lua_pop(L,4);
                    lua_pushliteral(L,"incorrect isrc length");
                    return 1;
                }
                memcpy(track->isrc,str,strlen);
                track->isrc[strlen] = 0;
            }
            lua_pop(L,1);

            lua_getfield(L,-1,"type");
            if(!lua_isnil(L,-1)) {
                track->type = lua_tointeger(L,-1);
            }
            lua_pop(L,1);

            lua_getfield(L,-1,"pre_emphasis");
            if(!lua_isnil(L,-1)) {
                track->pre_emphasis = lua_tointeger(L,-1);
            }
            lua_pop(L,1);

            /* insert the track now since we'll have indexes to add */
            /* setting copy to "false" since this was dynamically allocated with new */
            if(!FLAC__metadata_object_cuesheet_set_track(m,i,track,0)) {
                lua_pop(L,3);
                lua_pushliteral(L,"error setting track");
                return 1;
            }

            lua_getfield(L,-1,"indices");
            if(!lua_isnil(L,-1)) {
                num_indices = lua_rawlen(L,-1);

                if(!FLAC__metadata_object_cuesheet_track_resize_indices(m,i,num_indices)) {
                    lua_pop(L,4);
                    lua_pushliteral(L,"error resizing track indexes");
                    return 1;
                }

                j=0;
                while(j<num_indices) {
                    lua_rawgeti(L,-1,j+1);

                    lua_getfield(L,-1,"offset");
                    if(!lua_isnil(L,-1)) {
                        m->data.cue_sheet.tracks[i].indices[j].offset = luaflac_touint64(L,-1);
                    } else {
                        m->data.cue_sheet.tracks[i].indices[j].offset = 0;
                    }
                    lua_pop(L,1);

                    lua_getfield(L,-1,"number");
                    if(!lua_isnil(L,-1)) {
                        m->data.cue_sheet.tracks[i].indices[j].number = lua_tointeger(L,-1);
                    } else {
                        m->data.cue_sheet.tracks[i].indices[j].number = 0;
                    }
                    lua_pop(L,1);

                    lua_pop(L,1); /* current index (rawgeti) */
                    j++;
                }
            }
            lua_pop(L,1); /* indices */

            lua_pop(L,1); /* current track */
            i++;
        }
    }

    lua_pop(L,2); /* tracks,cue_sheet */
    assert(top == lua_gettop(L));
    return 0;
}

static int
luaflac_toflac_streammetadata_picture(lua_State *L, int idx, FLAC__StreamMetadata *m) {
    int top = lua_gettop(L);
    const char *str = NULL;
    size_t str_len = 0;

    lua_getfield(L,idx,"picture");
    if(!lua_istable(L,-1)) {
        lua_pop(L,1);
        lua_pushliteral(L,"missing key: picture");
        return 1;
    }

    lua_getfield(L,-1,"type");
    if(!lua_isnil(L,-1)) {
        m->data.picture.type = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"width");
    if(!lua_isnil(L,-1)) {
        m->data.picture.width = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"height");
    if(!lua_isnil(L,-1)) {
        m->data.picture.height = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"depth");
    if(!lua_isnil(L,-1)) {
        m->data.picture.depth = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"colors");
    if(!lua_isnil(L,-1)) {
        m->data.picture.colors = lua_tointeger(L,-1);
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"mime_type");
    if(!lua_isnil(L,-1)) {
        str = lua_tostring(L,-1);
        if(!FLAC__metadata_object_picture_set_mime_type(m,(char *)str,1)) {
            lua_pop(L,2);
            lua_pushliteral(L,"error setting mime_type");
            return 1;
        }
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"description");
    if(!lua_isnil(L,-1)) {
        str = lua_tostring(L,-1);
        if(!FLAC__metadata_object_picture_set_description(m,(FLAC__byte *)str,1)) {
            lua_pop(L,2);
            lua_pushliteral(L,"error setting description");
            return 1;
        }
    }
    lua_pop(L,1);

    lua_getfield(L,-1,"data");
    if(!lua_isnil(L,-1)) {
        str = lua_tolstring(L,-1,&str_len);
        if(!FLAC__metadata_object_picture_set_data(m,(FLAC__byte *)str,str_len,1)) {
            lua_pop(L,2);
            lua_pushliteral(L,"error setting data");
            return 1;
        }
    }
    lua_pop(L,1);

    lua_pop(L,1);
    assert(top == lua_gettop(L));
    return 0;
}

LUAFLAC_PRIVATE
FLAC__StreamMetadata *
luaflac_toflac_streammetadata(lua_State *L, int idx) {
    FLAC__StreamMetadata *m = NULL;
    if(!lua_istable(L,idx)) {
        luaL_error(L,"invalid table");
        return NULL;
    }

    lua_getfield(L,idx,"type");
    m = FLAC__metadata_object_new(lua_tointeger(L,-1));
    if(m == NULL) {
        luaL_error(L,"memory error");
        return NULL;
    }
    lua_pop(L,1);

    lua_getfield(L,idx,"is_last");
    if(!lua_isnil(L,-1)) {
        m->is_last = lua_toboolean(L,-1);
    }
    lua_pop(L,1);

    switch(m->type) {
        case FLAC__METADATA_TYPE_STREAMINFO: {
            if(luaflac_toflac_streammetadata_streaminfo(L,idx,m)) {
                goto luaflac_toflac_streammetadata_error;
            }
            break;
        }
        case FLAC__METADATA_TYPE_PADDING: {
            if(luaflac_toflac_streammetadata_padding(L,idx,m)) {
                goto luaflac_toflac_streammetadata_error;
            }
            break;
        }
        case FLAC__METADATA_TYPE_APPLICATION: {
            if(luaflac_toflac_streammetadata_application(L,idx,m)) {
                goto luaflac_toflac_streammetadata_error;
            }
            break;
        }
        case FLAC__METADATA_TYPE_SEEKTABLE: {
            if(luaflac_toflac_streammetadata_seek_table(L,idx,m)) {
                goto luaflac_toflac_streammetadata_error;
            }
            break;
        }
        case FLAC__METADATA_TYPE_VORBIS_COMMENT: {
            if(luaflac_toflac_streammetadata_vorbis_comment(L,idx,m)) {
                goto luaflac_toflac_streammetadata_error;
            }
            break;
        }
        case FLAC__METADATA_TYPE_CUESHEET: {
            if(luaflac_toflac_streammetadata_cue_sheet(L,idx,m)) {
                goto luaflac_toflac_streammetadata_error;
            }
            break;
        }
        case FLAC__METADATA_TYPE_PICTURE: {
            if(luaflac_toflac_streammetadata_picture(L,idx,m)) {
                goto luaflac_toflac_streammetadata_error;
            }
            break;
        }
        default: {
            lua_pushliteral(L,"unknown metadata type");
            goto luaflac_toflac_streammetadata_error;
        }
    }

    goto luaflac_toflac_streammetadata_complete;

    luaflac_toflac_streammetadata_error:
    FLAC__metadata_object_delete(m);
    m = NULL;
    lua_error(L);

    luaflac_toflac_streammetadata_complete:

    return m;
}

/* methods for converting a FLAC__StreamMetadata object to a lua table */

static void
luaflac_pushstreammetadata_streaminfo(lua_State *L, const FLAC__StreamMetadata *m) {
    lua_newtable(L);
    lua_pushinteger(L,m->data.stream_info.min_blocksize);
    lua_setfield(L,-2,"min_blocksize");

    lua_pushinteger(L,m->data.stream_info.max_blocksize);
    lua_setfield(L,-2,"max_blocksize");

    lua_pushinteger(L,m->data.stream_info.min_framesize);
    lua_setfield(L,-2,"min_framesize");

    lua_pushinteger(L,m->data.stream_info.max_framesize);
    lua_setfield(L,-2,"max_framesize");

    lua_pushinteger(L,m->data.stream_info.sample_rate);
    lua_setfield(L,-2,"sample_rate");

    lua_pushinteger(L,m->data.stream_info.channels);
    lua_setfield(L,-2,"channels");

    lua_pushinteger(L,m->data.stream_info.bits_per_sample);
    lua_setfield(L,-2,"bits_per_sample");

    luaflac_pushuint64(L,m->data.stream_info.total_samples);
    lua_setfield(L,-2,"total_samples");

    lua_pushlstring(L,(const char *)m->data.stream_info.md5sum,16);
    lua_setfield(L,-2,"md5sum");

    lua_setfield(L,-2,"stream_info");
}

static void
luaflac_pushstreammetadata_padding(lua_State *L, const FLAC__StreamMetadata *m) {
    lua_newtable(L);
    lua_pushinteger(L,m->data.padding.dummy);
    lua_setfield(L,-2,"dummy");
    lua_setfield(L,-2,"padding");
}

static void
luaflac_pushstreammetadata_application(lua_State *L, const FLAC__StreamMetadata *m) {
    lua_newtable(L);

    lua_pushlstring(L,(const char *)m->data.application.id,4);
    lua_setfield(L,-2,"id");
    lua_pushlstring(L,(const char *)m->data.application.data,m->length);
    lua_setfield(L,-2,"data");

    lua_setfield(L,-2,"application");
}

static void
luaflac_pushstreammetadata_seek_table(lua_State *L, const FLAC__StreamMetadata *m) {
    unsigned int i = 0;
    lua_newtable(L);

    lua_pushinteger(L,m->data.seek_table.num_points);
    lua_setfield(L,-2,"num_points");

    i = 0;
    lua_newtable(L);
    while(i<m->data.seek_table.num_points) {
        lua_newtable(L);
        luaflac_pushuint64(L,m->data.seek_table.points[i].sample_number);
        lua_setfield(L,-2,"sample_number");
        luaflac_pushuint64(L,m->data.seek_table.points[i].stream_offset);
        lua_setfield(L,-2,"stream_offset");
        lua_pushinteger(L,m->data.seek_table.points[i].frame_samples);
        lua_setfield(L,-2,"frame_samples");
        lua_rawseti(L,-2,++i);
    }
    lua_setfield(L,-2,"points");

    lua_setfield(L,-2,"seek_table");
}

static void
luaflac_pushstreammetadata_vorbis_comment(lua_State *L, const FLAC__StreamMetadata *m) {
    unsigned int i = 0;
    lua_newtable(L);

    lua_pushlstring(L,(const char *)m->data.vorbis_comment.vendor_string.entry,
      m->data.vorbis_comment.vendor_string.length);
    lua_setfield(L,-2,"vendor_string");

    lua_pushinteger(L,m->data.vorbis_comment.num_comments);
    lua_setfield(L,-2,"num_comments");

    i = 0;
    lua_newtable(L);
    while(i<m->data.vorbis_comment.num_comments) {
        lua_pushlstring(L, (const char *)
          m->data.vorbis_comment.comments[i].entry,
          m->data.vorbis_comment.comments[i].length);
        lua_rawseti(L,-2,++i);
    }
    lua_setfield(L,-2,"comments");

    lua_setfield(L,-2,"vorbis_comment");
}

static void
luaflac_pushstreammetadata_cue_sheet(lua_State *L, const FLAC__StreamMetadata *m) {
    unsigned int i = 0;
    unsigned int j = 0;
    lua_newtable(L);

    lua_pushstring(L,m->data.cue_sheet.media_catalog_number);
    lua_setfield(L,-2,"media_catalog_number");
    lua_pushinteger(L,m->data.cue_sheet.lead_in);
    lua_setfield(L,-2,"lead_in");
    lua_pushboolean(L,m->data.cue_sheet.is_cd);
    lua_setfield(L,-2,"is_cd");
    lua_pushinteger(L,m->data.cue_sheet.num_tracks);
    lua_setfield(L,-2,"num_tracks");

    lua_newtable(L);
    while(i<m->data.cue_sheet.num_tracks) {
        lua_newtable(L);

        luaflac_pushuint64(L,m->data.cue_sheet.tracks[i].offset);
        lua_setfield(L,-2,"offset");

        lua_pushinteger(L,m->data.cue_sheet.tracks[i].number);
        lua_setfield(L,-2,"number");

        lua_pushstring(L,m->data.cue_sheet.tracks[i].isrc);
        lua_setfield(L,-2,"isrc");

        lua_pushinteger(L,m->data.cue_sheet.tracks[i].type);
        lua_setfield(L,-2,"type");

        lua_pushinteger(L,m->data.cue_sheet.tracks[i].pre_emphasis);
        lua_setfield(L,-2,"pre_emphasis");

        lua_pushinteger(L,m->data.cue_sheet.tracks[i].num_indices);
        lua_setfield(L,-2,"num_indices");

        j = 0;
        lua_newtable(L);
        while(j<m->data.cue_sheet.tracks[i].num_indices) {
            lua_newtable(L);
            luaflac_pushuint64(L,m->data.cue_sheet.tracks[i].indices[j].offset);
            lua_setfield(L,-2,"offset");
            lua_pushinteger(L,m->data.cue_sheet.tracks[i].indices[j].number);
            lua_setfield(L,-2,"number");
            lua_rawseti(L,-2,++j);
        }
        lua_setfield(L,-2,"indices");

        lua_rawseti(L,-2,++i);
    }
    lua_setfield(L,-2,"tracks");

    lua_setfield(L,-2,"cue_sheet");
}

static void
luaflac_pushstreammetadata_picture(lua_State *L, const FLAC__StreamMetadata *m) {
    lua_newtable(L);

    lua_pushinteger(L,m->data.picture.type);
    lua_setfield(L,-2,"type");
    lua_pushstring(L,m->data.picture.mime_type);
    lua_setfield(L,-2,"mime_type");
    lua_pushinteger(L,m->data.picture.width);
    lua_setfield(L,-2,"width");
    lua_pushinteger(L,m->data.picture.height);
    lua_setfield(L,-2,"height");
    lua_pushinteger(L,m->data.picture.depth);
    lua_setfield(L,-2,"depth");
    lua_pushinteger(L,m->data.picture.colors);
    lua_setfield(L,-2,"colors");
    lua_pushinteger(L,m->data.picture.data_length);
    lua_setfield(L,-2,"data_length");
    lua_pushlstring(L,(const char *)m->data.picture.data,m->data.picture.data_length);
    lua_setfield(L,-2,"data");

    lua_setfield(L,-2,"picture");
}

static void
luaflac_pushstreammetadata_undefined(lua_State *L, const FLAC__StreamMetadata *m) {
    lua_pushlstring(L,(const char *)&m->data,m->length);
    lua_setfield(L,-2,"data");
}

LUAFLAC_PRIVATE
int
luaflac_pushstreammetadata(lua_State *L, const FLAC__StreamMetadata *m) {
    int top;
    lua_newtable(L);
    top = lua_gettop(L);
    lua_pushinteger(L,m->type);
    lua_setfield(L,-2,"type");
    lua_pushboolean(L,m->is_last);
    lua_setfield(L,-2,"is_last");
    lua_pushinteger(L,m->length);
    lua_setfield(L,-2,"length");

    switch(m->type) {
        case FLAC__METADATA_TYPE_STREAMINFO: {
            luaflac_pushstreammetadata_streaminfo(L,m);
            break;
        }
        case FLAC__METADATA_TYPE_PADDING: {
            luaflac_pushstreammetadata_padding(L,m);
            break;
        }
        case FLAC__METADATA_TYPE_APPLICATION: {
            luaflac_pushstreammetadata_application(L,m);
            break;
        }
        case FLAC__METADATA_TYPE_SEEKTABLE: {
            luaflac_pushstreammetadata_seek_table(L,m);
            break;
        }
        case FLAC__METADATA_TYPE_VORBIS_COMMENT: {
            luaflac_pushstreammetadata_vorbis_comment(L,m);
            break;
        }
        case FLAC__METADATA_TYPE_CUESHEET: {
            luaflac_pushstreammetadata_cue_sheet(L,m);
            break;
        }
        case FLAC__METADATA_TYPE_PICTURE: {
            luaflac_pushstreammetadata_picture(L,m);
            break;
        }
        default: {
            luaflac_pushstreammetadata_undefined(L,m);
            break;
        }
    }
    assert(top == lua_gettop(L));
    return 1;
}

