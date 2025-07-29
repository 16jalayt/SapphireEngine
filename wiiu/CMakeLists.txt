cmake_minimum_required(VERSION 3.2)
SET(CMAKE_INSTALL_PREFIX ./lib)
set(IMGUI_SUPPORTED FALSE)

if(NOT DEFINED ENV{DEVKITPRO})
    message( FATAL_ERROR "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif()

project(SapphireEngine)


find_package(SDL2 REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(
  "ext/"
  "ext/loguru"
  "ext/toml11"
  "../include"
)

file(GLOB_RECURSE engine_SRC CONFIGURE_DEPENDS "../src/*.h" "../src/*.cpp")
file(GLOB_RECURSE engine_EXT CONFIGURE_DEPENDS "ext/*.h" "ext/*.hpp" "ext/*.cpp")

add_library(${PROJECT_NAME} STATIC
  ${engine_SRC}
  ${engine_EXT}
)

target_link_libraries(${PROJECT_NAME}
  SDL2_mixer 
  SDL2_image 
  SDL2_ttf 
  SDL2_gfx 
  SDL2 
  webpdemux
  webp
  opusfile
  vorbisfile
  vorbis
  ogg
  mpg123
  FLAC
  mikmod
  modplug
  opus
  swresample 
  mp3lame
  avutil 
  freetype 
  mpdec 
  xmp
  jpeg 
  png 
  bz2 
  ${SDL2_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  pthread
  intl
  fribidi
  harfbuzz
  gcc
  z
  m
)

#Cant find what actual platform define is
add_compile_definitions(__WIIU__)


wut_create_rpx(helloworld)