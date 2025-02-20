cmake_minimum_required(VERSION 3.2)
SET(CMAKE_INSTALL_PREFIX ./lib)

project(SapphireEngine)

set(IMGUI_SUPPORTED TRUE)

find_package(SDL2 REQUIRED)
message(STATUS "SDL2 Version ${SDL2_VERSION}")
if (${SDL2_VERSION} VERSION_LESS "2.0.17")
  message(WARNING "SDL2 Version ${SDL2_VERSION} is less than 2.0.17. Disabling IMGUI support.")
  add_compile_definitions(IMGUI_SUPPORTED)
  set(IMGUI_SUPPORTED FALSE)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#might allow to remove additions from loguru *_additions
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")

if (IMGUI_SUPPORTED)
set(IMGUI_DIR ../ext/mac/imgui)

include_directories(
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
  ${IMGUI_DIR}/misc/cpp/
  /usr/include/SDL2/
)
endif()

include_directories(
  "ext/"
  "ext/loguru"
  "ext/toml11"
  "../include"
)

file(GLOB_RECURSE engine_SRC CONFIGURE_DEPENDS "../src/*.h" "../src/*.cpp")
file(GLOB_RECURSE engine_EXT CONFIGURE_DEPENDS "ext/*.h" "ext/*.hpp" "ext/*.cpp")

set(IMGUI_SRC
      ${IMGUI_DIR}/imgui.h
      ${IMGUI_DIR}/imgui.cpp
      ${IMGUI_DIR}/imgui_draw.cpp
      ${IMGUI_DIR}/imgui_tables.cpp
      ${IMGUI_DIR}/imgui_widgets.cpp
      ${IMGUI_DIR}/imgui_demo.cpp
      ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
      ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
      ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

add_library(${PROJECT_NAME} STATIC
  ${engine_SRC}
  ${engine_EXT}
  $<$<BOOL:${IMGUI_SUPPORTED}>:${IMGUI_SRC}>
)

target_link_libraries(${PROJECT_NAME}
  SDL2_mixer 
  SDL2_image 
  SDL2_ttf 
  SDL2_gfx 
  SDL2 
  opusfile
  vorbisfile
  vorbis
  ogg
  opus
  mp3lame
  freetype 
  mpdec 
  xmp
  jpeg 
  png 
  bz2 
  ${SDL2_LIBRARIES}
  pthread
  harfbuzz
  gcc
  z
  m
)
