# luaflac

Lua bindings to [libFLAC](https://xiph.org/flac/api/index.html)

MIT licensed (see file `LICENSE`).

Currently covers the stream encoder and decoder APIs, including parsing/setting metadata frames.

# Installation

## Building from source

You can build with luarocks or cmake.

# Table of Contents

* [Synopsis](#synopsis)
* [Implementation Notes](#implementation-notes)
  * [Metadata Blocks](#metadata-blocks)
  * [64-bit Values](#64-bit-values)
* [Decoder Functions](#decoder-functions)
* [Encoder Functions](#encoder-functions)

# Synopsis

```lua
-- demo program that opens an decoder for a file at arg[1]
-- encodes to a new file at arg[2]
local flac = require'luaflac'

if not arg[2] then
  print(string.format('Usage: %s infile outfile', arg[0]))
  os.exit(1)
end

local in_file = io.open(arg[1],'rb')
if not in_file then
  print(string.format('Error: unable to open %s for reading',arg[1]))
  os.exit(1)
end

local out_file = io.open(arg[2],'wb')
if not out_file then
  print(string.format('Error: unable to open %s for writing',arg[2]))
  os.exit(1)
end

-- create our encoder/decoder instances

local decoder = flac.FLAC__stream_decoder_new()
local encoder = flac.FLAC__stream_encoder_new()

-- our write callback for the encoder, buffer is a string of bytes
local function encoder_write_callback(userdata,buffer)
  return out_file:write(buffer)
end

-- our metadata callback for the decoder, we'll set some encoder
-- settings based on our STREAMINFO, and copy in the VORBIS_COMMENT
-- block (if it exists)
local function decoder_metadata_callback(userdata,metadata)
  -- copy any vorbis comments to our encoder instance
  if metadata.type == flac.FLAC__METADATA_TYPE_VORBIS_COMMENT then
    encoder:set_metadata({ metadata })
  elseif metadata.type == flac.FLAC__METADATA_TYPE_STREAMINFO then
    encoder:set_bits_per_sample(metadata.stream_info.bits_per_sample)
    encoder:set_channels(metadata.stream_info.channels)
    encoder:set_sample_rate(metadata.stream_info.sample_rate)
    encoder:set_total_samples_estimate(metadata.stream_info.total_samples)
  end
end

-- our read callback for the decoder, receives the number of bytes
-- of data to read, needs to return a string of bytes
local function decoder_read_callback(userdata, size)
  return in_file:read(size)
end

-- our write callback for the decoder, buffer is a multi-dimensional array,
-- 1-based, as per usual for Lua
-- example, buffer[1][1] = channel 1, sample 1
-- example, buffer[1][2] = channel 1, sample 2
-- example, buffer[2][1] = channel 2, sample 1
-- example, buffer[2][2] = channel 2, sample 2
-- each sample is an integer
local function decoder_write_callback(userdata,frame,buffer)

  -- encoder accepts a multi-dimensional array, so we can just
  -- pass the buffer as-is
  return encoder:process(buffer)
end

-- our decoder error callback, we'll print the code and just quit
local function decoder_error_callback(userdata, code)
  print(string.format('error: %d', code))
  os.exit(1)
end

-- by default, the decoder only calls our metadata callback with the STREAMINFO block
-- so let's enable vorbis comments as well.
decoder:set_metadata_respond(flac.FLAC__METADATA_TYPE_VORBIS_COMMENT)

-- init our decoder stream
assert(decoder:init_stream({
  read = decoder_read_callback,
  write = decoder_write_callback,
  metadata = decoder_metadata_callback,
  error = decoder_error_callback,
}) == flac.FLAC__STREAM_DECODER_INIT_STATUS_OK)

-- before initializing the encoder, have decoder process through
-- all the metadata (to ensure our metadata callback is called)
assert(decoder:process_until_end_of_metadata())

-- print out some basic info that was set in the decoder metadata callback
print('Output Audio Format:')
print(string.format('  bps: %s',tostring(encoder:get_bits_per_sample())))
print(string.format('  channels: %s',tostring(encoder:get_channels())))
print(string.format('  sample_rate: %s',tostring(encoder:get_sample_rate())))

-- we should have everything we need to init a stream
assert(encoder:init_stream({
  write = encoder_write_callback
}) == flac.FLAC__STREAM_ENCODER_INIT_STATUS_OK)

-- do the rest of the conversion
assert(decoder:process_until_end_of_stream())
assert(encoder:finish())

in_file:close()
out_file:close()

os.exit(0)
```

# Implementation Notes

## Metadata Blocks

`FLAC__StreamMetadata` structs are converted to/from Lua tables on-the-fly, I try to follow
the same naming conventions and structure from the C library.

Example, a `FLAC__StreamMetadata` `STREAMINFO` block could be represented in Lua as:

```lua
{
  type = 0,
  is_last = false,
  length = 34,
  stream_info = {
    bits_per_sample = 16,
    channels = 2,
    max_blocksize = 4096,
    max_framesize = 15001,
    md5sum = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
    min_blocksize = 4096,
    min_framesize = 1411,
    sample_rate = 44100,
    total_samples = <userdata>
  },
}
```

And a `FLAC__StreamMetadata VORBIS_COMMENT` block could be represented in Lua as:

```lua
{
  type = 4,
  is_last = false,
  length = 2507,
  vorbis_comment = {
    vendor_string = "libFLAC 1.2.1",
    num_comments = 3,
    comments = {
      "ARTIST=Some Artist",
      "ALBUM=Some Album",
      "TITLE=A Great Song",
    },
  },
}
```

When a function requires a metadata block, you don't need to set most
the length-related fields (`length`, `num_comments`, etc).

For example, a `VORBIS_COMMENT` block has
`num_comments` field for the number of vorbis comment entries.
When converted from/to Lua, vorbis comment entries are represented
as a Lua table, `num_comments` is ignored, and the length of the table
is used instead.

`libFLAC` will automatically set some fields as well (like `length` and `is_last`),
so when setting metadata you can do something like:

```lua
encoder:set_metadata({{
  type = 4,
  vorbis_comment = {
    comments = {
      "ARTIST=Some Artist",
      "ALBUM=Some Artist",
      "TITLE=A Great Song",
    },
  },
}})
```

## 64-bit values

libFLAC uses 64-bit, unsigned integers in a few places.


Lua 5.3 (and above) *may* have 64-bit integers (it's a compile-time option),
whereas Lua 5.2 (and below) do not.

`luaflac` has a 64bit userdata available. It supports comparison operations,
arithmetic operations, bitwise operations, and can be converted to a string.

It can be created from a Lua number or string.

Any function that accepts a 64-bit value as input will have the userdata
created on-the-fly, if you use a number or string. Same for field
entries on an input table.

The constructor is available as `flac.FLAC__uint64` and `flac.FLAC__int64`, example:

```lua
local i = luaflac.FLAC__int64('9223372036854775806') -- creates a new userdata
i = i + 1
print(i) -- prints "9223372036854775807", the max 64-bit signed int
```

# Decoder Functions

This section is a work-in-progress, for the most part you should be able to follow
the libFLAC documentation, I try to use the same function names and parameters.

The exceptions to that rule are:

* `*_init_stream`, `*_init_file`, `*_init_ogg_stream`, and `*_init_ogg_file` functions accept their parameters in a table.
* functions with an out-variable return the out-variable (example: `FLAC__stream_decoder_get_position` will return a `uint64` value, not
the boolean that the C library returns).

## FLAC\_\_stream_decoder\_new

**syntax:** `userdata state = flac.FLAC__stream_decoder_new()`

Returns a new stream decoder instance.

Instance has a metatable set allowing for object-oriented usage. Basically,
take any `FLAC__stream_decoder_*` function and remove the `FLAC__stream_decoder_` prefix,
example:

`state:get_channels()` is the same as `FLAC__stream_decoder_get_channels(state)`

## FLAC\_\_stream_decoder_init_file

**syntax:** `boolean success = FLAC__stream_decoder_init_file(userdata state, table params)`

Sets up a decoder instance to decode a FLAC file, `params` requires the following keys:

* `filename` - a lua string
* `write` - a callback for decoded data
* `error` - a callback for errors

Optional keys:

* `metadata` - a callback for metadata
* `userdata` - a value to pass to callbacks, always used as the first parameter.

## FLAC\_\_stream_decoder_init_stream

**syntax:** `boolean success = FLAC__stream_decoder_init_stream(userdata state, table params)`

Sets up a decoder instance to decode a FLAC stream, `params` requires the following keys:

* `read` - a callback for reading stream data
* `write` - a callback for decoded data
* `error` - a callback for errors

Optional keys:

* `metadata` - a callback for metadata
* `seek` - a callback to seek the stream.
* `tell` - a callback to get the absolute position of the stream (required if `seek` is given)
* `length` - a callback to get the length of the stream (required if `seek` is given)
* `eof` - a callback to determine if we've reached the end of the stream. (required if `seek` is given)
* `userdata` - a value to pass to callbacks, always used as the first parameter.

## FLAC\_\_stream_decoder_init_ogg_file

**syntax:** `boolean success = FLAC__stream_decoder_init_ogg_file(userdata state, table params)`

Sets up a decoder instance to decode a FLAC in Ogg file, `params` requires the following keys:

* `filename` - a lua string
* `write` - a callback for decoded data
* `error` - a callback for errors

Optional keys:

* `metadata` - a callback for metadata
* `userdata` - a value to pass to callbacks, always used as the first parameter.

## FLAC\_\_stream_decoder_init_ogg_stream

**syntax:** `boolean success = FLAC__stream_decoder_init_ogg_stream(userdata state, table params)`

Sets up a decoder instance to decode a FLAC in Ogg stream, `params` requires the following keys:

* `read` - a callback for reading stream data
* `write` - a callback for decoded data
* `error` - a callback for errors

Optional keys:

* `metadata` - a callback for metadata
* `seek` - a callback to seek the stream.
* `tell` - a callback to get the absolute position of the stream (required if `seek` is given)
* `length` - a callback to get the length of the stream (required if `seek` is given)
* `eof` - a callback to determine if we've reached the end of the stream. (required if `seek` is given)
* `userdata` - a value to pass to callbacks, always used as the first parameter.

# Encoder Functions

This section is a work-in-progress, for the most part you should be able to follow
the libFLAC documentation, I try to use the same function names and parameters.

The exceptions to that rule are:

* `*_init_stream`, `*_init_file`, `*_init_ogg_stream`, and `*_init_ogg_file` functions accept their parameters in a table.
* functions with multiple out-variables return a table of out-variables (example: `FLAC__stream_encoder_get_verify_decoder_error_stats` will
  return a table with entries named `absolute_sample`, `frame_number`, etc).

## FLAC\_\_stream\_encoder\_new

**syntax:** `userdata state = flac.FLAC__stream_encoder_new()`

Returns a new stream encoder instance.

Instance has a metatable set allowing for object-oriented usage. Basically,
take any `FLAC__stream_encoder\_*` function and remove the `FLAC__stream_encoder_` prefix,
example:

`state:set_channels(num)` is the same as `FLAC__stream_encoder_set_channels(state,num)`

## FLAC\_\_stream_encoder_init_file

**syntax:** `boolean success = FLAC__stream_encoder_init_file(userdata state, table params)`

Sets up a encoder instance to decode a FLAC file, `params` requires the following keys:

* `filename` - a lua string

Optional keys:

* `progress` - a callback, called periodically with information like number of samples encoded
* `userdata` - a value to pass to callbacks, always used as the first parameter.

## FLAC\_\_stream_encoder_init_stream

**syntax:** `boolean success = FLAC\_\_stream_encoder_init_stream(userdata state, table params)`

Sets up a encoder instance to decode a FLAC stream, `params` requires the following keys:

* `write` - a callback for writing encoded data

Optional keys:

* `metadata` - a callback for metadata, called at the end of encoding
* `seek` - a callback to seek the stream.
* `tell` - a callback to get the absolute position of the stream (required if `seek` is given)
* `userdata` - a value to pass to callbacks, always used as the first parameter.

## FLAC\_\_stream_encoder_init_ogg_file

**syntax:** `boolean success = FLAC\_\_stream_encoder_init_ogg_file(userdata state, table params)`

Sets up a encoder instance to decode a FLAC in Ogg file, `params` requires the following keys:

* `filename` - a lua string

Optional keys:

* `progress` - a callback, called periodically with information like number of samples encoded
* `userdata` - a value to pass to callbacks, always used as the first parameter.

## FLAC\_\_stream_encoder_init_ogg_stream

**syntax:** `boolean success = FLAC\_\_stream_encoder_init_ogg_stream(userdata state, table params)`

Sets up a encoder instance to decode a FLAC in Ogg stream, `params` requires the following keys:

* `write` - a callback for decoded data

Optional keys:

* `metadata` - a callback for metadata
* `read` - a callback for reading back stream data
* `seek` - a callback to seek the stream (required if `read` is given).
* `tell` - a callback to get the absolute position of the stream (required if `read` is given)
* `userdata` - a value to pass to callbacks, always used as the first parameter.
