cmake_minimum_required(VERSION 3.24)
project(SappireEngine)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)
set(CMAKE_MACOSX_BUNDLE TRUE)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED CONFIG)
find_package(SDL2_mixer REQUIRED CONFIG)
find_package(SDL2_ttf REQUIRED CONFIG)

include_directories(../ext/mac/imgui/misc/cpp)
include_directories(../ext/mac/imgui)
include_directories(../ext/mac/imgui/backends)
include_directories(../mac)
include_directories(../ext)
include_directories(..)
include_directories(../ext/mac)
#include_directories(../ext/mac/SDL2)
include_directories(../ext/loguru-master)
include_directories(../ext/toml11)
include_directories(../src)
include_directories(../include)

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVFILTER_INCLUDE_DIR libavfilter/avfilter.h)
find_library(AVFILTER_LIBRARY avfilter)

find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h)
find_library(SWSCALE_LIBRARY swscale)

find_path(SWRESAMPLE_INCLUDE_DIR libswresample/swresample.h)
find_library(SWRESAMPLE_LIBRARY swresample)

include_directories(${AVCODEC_INCLUDE_DIR})
include_directories(${AVFORMAT_INCLUDE_DIR})
include_directories(${AVUTIL_INCLUDE_DIR})
include_directories(${AVDEVICE_INCLUDE_DIR})

file(GLOB SOURCES_TOML CONFIGURE_DEPENDS "../ext/toml11/*.h" "../ext/toml11/*.cpp")
file(GLOB SOURCES CONFIGURE_DEPENDS "../include/*.h" "../include/Engine/*.h" "../src/*.cpp")
file(GLOB SOURCES_IMGUI_STD CONFIGURE_DEPENDS "../ext/mac/imgui/misc/cpp/*.h" "../ext/mac/imgui/misc/cpp/*.cpp")
file(GLOB SOURCES_IMGUI CONFIGURE_DEPENDS "../ext/mac/imgui/*.h" "../ext/mac/imgui/*.cpp")
file(GLOB SOURCES_GFX CONFIGURE_DEPENDS "../ext/mac/SDL2/*.h" "../ext/mac/SDL2/*.c")
file(GLOB IMGUI_LIB "../ext/mac/imgui/build/*.dylib")
file(GLOB SOURCES_HELPER CONFIGURE_DEPENDS "../mac/*.hpp" "../mac/*.mm")
file(GLOB SOURCES_LOG CONFIGURE_DEPENDS "../ext/loguru-master/*.hpp" "../ext/loguru-master/*.cpp")

add_library(SappireEngine SHARED ${SOURCES_TOML} ${SOURCES_IMGUI_STD} ${SOURCES_IMGUI} ${SOURCES_GFX} ${SOURCES_HELPER} ${SOURCES_LOG} ${SOURCES})

target_link_libraries(SappireEngine PRIVATE ${IMGUI_LIB})

target_link_libraries(SappireEngine PRIVATE stdc++ "-framework Foundation" "-framework Cocoa" objc)
#target_link_libraries(SappireEngine PRIVATE "-framework CoreFoundation")
target_link_libraries(SappireEngine PRIVATE SDL2::SDL2)
target_link_libraries(SappireEngine PRIVATE SDL2_image::SDL2_image)
target_link_libraries(SappireEngine PRIVATE SDL2_mixer::SDL2_mixer)
target_link_libraries(SappireEngine PRIVATE SDL2_ttf::SDL2_ttf)
target_link_libraries(SappireEngine PRIVATE ${AVCODEC_LIBRARY})
target_link_libraries(SappireEngine PRIVATE ${AVFORMAT_LIBRARY})
target_link_libraries(SappireEngine PRIVATE ${AVUTIL_LIBRARY})
target_link_libraries(SappireEngine PRIVATE ${AVFILTER_LIBRARY})
target_link_libraries(SappireEngine PRIVATE ${SWSCALE_LIBRARY})
target_link_libraries(SappireEngine PRIVATE ${SWRESAMPLE_LIBRARY})

set(RESOURCE_FILES) # these files go into .app/Contents/Resources/
#set(RESOURCE_FILES arial.ttf AppIcon.icns) # these files go into .app/Contents/Resources/
#set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)
set_target_properties(SappireEngine PROPERTIES MACOSX_BUNDLE TRUE MACOSX_FRAMEWORK_IDENTIFIER SapphireForge.TestApp RESOURCE "${RESOURCE_FILES}")
# <lastname>.<firstname>.<appname> or <domain>.<appname>
# (domain formatted as com.example instead of example.com)

#copy frameworks
set(OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
message(PROJECT_SOURCE_DIR="${OUTPUT_DIRECTORY}")
set(EXECUTABLE_OUTPUT_PATH "${OUTPUT_DIRECTORY}")
set(CMAKE_EXECUTABLE_OUTPUT_PATH "${OUTPUT_DIRECTORY}")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
SET( ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
SET( ARCHIVE_OUTPUT_DIRECTORY_<CONFIG> "${OUTPUT_DIRECTORY}")

set(APP_BUNDLE_CONTENTS_DIR "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.app/Contents")
set(APP_BUNDLE_FRAMEWORKS_DIR "${APP_BUNDLE_CONTENTS_DIR}/Frameworks")
set(FRAMEWORKS_DIR "${CMAKE_SOURCE_DIR}/Frameworks")

file(GLOB DYLIB_PATHS "${FRAMEWORKS_DIR}/SDL2.framework" "${FRAMEWORKS_DIR}/SDL2_image.framework" "${FRAMEWORKS_DIR}/SDL2_mixer.framework" "${FRAMEWORKS_DIR}/SDL2_ttf.framework" "${FRAMEWORKS_DIR}/libimgui_backend_sdl.dylib" "${FRAMEWORKS_DIR}/libimgui.dylib")
target_link_libraries(SappireEngine PRIVATE ${DYLIB_PATHS})
file(COPY ${DYLIB_PATHS} DESTINATION ${APP_BUNDLE_FRAMEWORKS_DIR})