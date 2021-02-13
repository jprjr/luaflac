#include "luaflac-internal.h"
#include <FLAC/format.h>

LUAFLAC_PUBLIC
int luaopen_luaflac_format(lua_State *L) {
    lua_newtable(L);

    luaflac_push_const(FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE);
    luaflac_push_const(FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2);
    luaflac_push_const(FLAC__SUBFRAME_TYPE_CONSTANT);
    luaflac_push_const(FLAC__SUBFRAME_TYPE_VERBATIM);
    luaflac_push_const(FLAC__SUBFRAME_TYPE_FIXED);
    luaflac_push_const(FLAC__SUBFRAME_TYPE_LPC);
    luaflac_push_const(FLAC__CHANNEL_ASSIGNMENT_INDEPENDENT);
    luaflac_push_const(FLAC__CHANNEL_ASSIGNMENT_LEFT_SIDE);
    luaflac_push_const(FLAC__CHANNEL_ASSIGNMENT_RIGHT_SIDE);
    luaflac_push_const(FLAC__CHANNEL_ASSIGNMENT_MID_SIDE);
    luaflac_push_const(FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER);
    luaflac_push_const(FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER);
    luaflac_push_const(FLAC__METADATA_TYPE_STREAMINFO);
    luaflac_push_const(FLAC__METADATA_TYPE_PADDING);
    luaflac_push_const(FLAC__METADATA_TYPE_APPLICATION);
    luaflac_push_const(FLAC__METADATA_TYPE_SEEKTABLE);
    luaflac_push_const(FLAC__METADATA_TYPE_VORBIS_COMMENT);
    luaflac_push_const(FLAC__METADATA_TYPE_CUESHEET);
    luaflac_push_const(FLAC__METADATA_TYPE_PICTURE);
    luaflac_push_const(FLAC__METADATA_TYPE_UNDEFINED);
    luaflac_push_const(FLAC__MAX_METADATA_TYPE);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_OTHER);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON_STANDARD);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_FRONT_COVER);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_BACK_COVER);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_LEAFLET_PAGE);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_MEDIA);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_LEAD_ARTIST);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_ARTIST);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_CONDUCTOR);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_BAND);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_COMPOSER);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_LYRICIST);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_RECORDING_LOCATION);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_DURING_RECORDING);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_DURING_PERFORMANCE);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_VIDEO_SCREEN_CAPTURE);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_FISH);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_ILLUSTRATION);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_BAND_LOGOTYPE);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_PUBLISHER_LOGOTYPE);
    luaflac_push_const(FLAC__STREAM_METADATA_PICTURE_TYPE_UNDEFINED);

    return 1;
}
