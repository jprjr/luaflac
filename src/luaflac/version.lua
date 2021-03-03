local VERSION_MAJOR = 3
local VERSION_MINOR = 0
local VERSION_PATCH = 0

return {
  _VERSION_MAJOR = VERSION_MAJOR,
  _VERSION_MINOR = VERSION_MINOR,
  _VERSION_PATCH = VERSION_PATCH,
  _VERSION = string.format('%d.%d.%d',
    VERSION_MAJOR,
    VERSION_MINOR,
    VERSION_PATCH),
}
