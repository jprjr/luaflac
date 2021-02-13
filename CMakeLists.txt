cmake_minimum_required(VERSION 3.1)
project(luaflac)

option(BUILD_SHARED_LIBS "Build modules as shared libraries" ON)

find_package(PkgConfig)
include(FindPackageHandleStandardArgs)

if(LUA_VERSION)
  find_package(Lua ${LUA_VERSION} EXACT REQUIRED)
else()
  find_package(Lua REQUIRED)
  set(LUA_VERSION "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}" CACHE STRING "Lua version to use")
endif()

pkg_check_modules(PC_FLAC QUIET libFLAC)
find_path(FLAC_INCLUDE_DIR FLAC/all.h
  HINTS ${PC_FLAC_INCLUDEDIR} ${PC_FLAC_INCLUDE_DIRS}
  PATH_SUFFIXES libFLAC)
find_library(FLAC_LIBRARY NAMES FLAC libFLAC
  HINTS ${PC_FLAC_LIBDIR} ${PC_FLAC_LIBRARY_DIRS})
find_package_handle_standard_args(LibFlac DEFAULT_MSG FLAC_LIBRARY FLAC_INCLUDE_DIR)
set(FLAC_LIBRARIES ${FLAC_LIBRARY})
set(FLAC_INCLUDE_DIRS ${FLAC_INCLUDE_DIR})

set(CMODULE_INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib/lua/${LUA_VERSION}")
set(LUAMODULE_INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/share/lua/${LUA_VERSION}")

configure_file(
  "src/luaflac/version.lua"
  "${CMAKE_BINARY_DIR}/luaflac/version.lua"
  COPYONLY
)

install(FILES "src/luaflac/version.lua"
  DESTINATION "${LUAMODULE_INSTALL_LIB_DIR}/"
)

set(luaflac_sources)
list(APPEND luaflac_sources "csrc/luaflac.c")
list(APPEND luaflac_sources "csrc/luaflac_int64.c")
list(APPEND luaflac_sources "csrc/luaflac_internal.c")
list(APPEND luaflac_sources "csrc/luaflac_no_ogg.c")
list(APPEND luaflac_sources "csrc/luaflac_export.c")
list(APPEND luaflac_sources "csrc/luaflac_format.c")
list(APPEND luaflac_sources "csrc/luaflac_metadata.c")
list(APPEND luaflac_sources "csrc/luaflac_stream_decoder.c")
list(APPEND luaflac_sources "csrc/luaflac_stream_encoder.c")

add_library(luaflac ${luaflac_sources})


target_link_libraries(luaflac PRIVATE ${FLAC_LIBRARIES})
target_link_directories(luaflac PRIVATE ${FLAC_LIBRARY_DIRS})
if(WIN32)
    target_link_libraries(luaflac PRIVATE ${LUA_LIBRARIES})
endif()
target_include_directories(luaflac PRIVATE ${FLAC_INCLUDEDIR})
target_include_directories(luaflac PRIVATE ${LUA_INCLUDE_DIR})

if(APPLE)
    set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -undefined dynamic_lookup")
    if(BUILD_SHARED_LIBS)
        set_target_properties(luaflac PROPERTIES SUFFIX ".so")
    endif()
endif()

set_target_properties(luaflac PROPERTIES PREFIX "")
set_target_properties(luaflac PROPERTIES OUTPUT_NAME "luaflac")
set_target_properties(luaflac PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(luaflac PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(luaflac PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(luaflac PROPERTIES C_VISIBILITY_PRESET hidden)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
  set_target_properties(luaflac PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}")
  set_target_properties(luaflac PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}")
  set_target_properties(luaflac PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}")
endforeach()

install(TARGETS luaflac
  LIBRARY DESTINATION "${CMODULE_INSTALL_LIB_DIR}"
  RUNTIME DESTINATION "${CMODULE_INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${CMODULE_INSTALL_LIB_DIR}"
)
