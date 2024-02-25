#IMPORTANT. Copy headers from all frameworks into sdl2
cmake_minimum_required(VERSION 3.24)
project(game_test)

#has to be under macbuild folder
#add_subdirectory(../source/Nancy)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)
set(CMAKE_MACOSX_BUNDLE TRUE)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
if(!XCODE)
set(CMAKE_SKIP_BUILD_RPATH TRUE)
endif()

find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED CONFIG)
find_package(SDL2_mixer REQUIRED CONFIG)
find_package(SDL2_ttf REQUIRED CONFIG)

include_directories(../source/Mac/imgui/misc/cpp)
include_directories(../source/Mac/imgui)
include_directories(../source/Mac/imgui/backends)
include_directories(../source/Mac)
#include_directories(../source/Mac/SDL2)
include_directories(../source)

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

file(GLOB SOURCES_TOML CONFIGURE_DEPENDS "../source/toml11/*.h" "../source/toml11/*.cpp")
file(GLOB SOURCES CONFIGURE_DEPENDS "../source/*.h" "../source/*.cpp")
file(GLOB SOURCES_NANCY CONFIGURE_DEPENDS "../source/Nancy/*.h" "../source/Nancy/*.cpp")
file(GLOB SOURCES_NANCY_CHUNKS CONFIGURE_DEPENDS "../source/Nancy/HIFFChunks/*.h" "../source/Nancy/HIFFChunks/*.cpp")
file(GLOB SOURCES_NANCY_ACT CONFIGURE_DEPENDS "../source/Nancy/HIFFChunks/ACT/*.h" "../source/Nancy/HIFFChunks/ACT/*.cpp")
file(GLOB SOURCES_ENGINE CONFIGURE_DEPENDS "../source/Engine/*.h" "../source/Engine/*.cpp")
file(GLOB SOURCES_FF CONFIGURE_DEPENDS "../source/player-cpp-ffmpeg-sdl/*.h" "../source/player-cpp-ffmpeg-sdl/*.cpp")
file(GLOB SOURCES_IMGUI_STD CONFIGURE_DEPENDS "../source/Mac/imgui/misc/cpp/*.h" "../source/Mac/imgui/misc/cpp/*.cpp")
#file(GLOB SOURCES_IMGUI CONFIGURE_DEPENDS "../source/Mac/imgui/*.h" "../source/Mac/imgui/*.cpp")
file(GLOB SOURCES_GFX CONFIGURE_DEPENDS "../source/Mac/SDL2/*.h" "../source/Mac/SDL2/*.c")
file(GLOB IMGUI_LIB "../source/Mac/imgui/build/*.dylib")
file(GLOB SOURCES_HER CONFIGURE_DEPENDS "../source/oldhertools/*.h" "../source/oldhertools/*.cpp")
file(GLOB SOURCES_HELPER CONFIGURE_DEPENDS "../source/Mac/*.hpp" "../source/Mac/*.mm")
#add_executable(game_test main.cpp arial.ttf AppIcon.icns)
add_executable(game_test ${SOURCES_TOML} ${SOURCES_IMGUI_STD} ${SOURCES_GFX} ${SOURCES_HER} ${SOURCES_NANCY_CHUNKS} ${SOURCES_NANCY_ACT} ${SOURCES_NANCY} ${SOURCES_ENGINE} ${SOURCES_FF} ${SOURCES} ${SOURCES_HELPER})

target_link_libraries(game_test PRIVATE ${IMGUI_LIB})

target_link_libraries(game_test PRIVATE stdc++ "-framework Foundation" "-framework Cocoa" objc)
#target_link_libraries(game_test PRIVATE "-framework CoreFoundation")
target_link_libraries(game_test PRIVATE SDL2::SDL2)
target_link_libraries(game_test PRIVATE SDL2_image::SDL2_image)
target_link_libraries(game_test PRIVATE SDL2_mixer::SDL2_mixer)
target_link_libraries(game_test PRIVATE SDL2_ttf::SDL2_ttf)
target_link_libraries(game_test PRIVATE ${AVCODEC_LIBRARY})
target_link_libraries(game_test PRIVATE ${AVFORMAT_LIBRARY})
target_link_libraries(game_test PRIVATE ${AVUTIL_LIBRARY})
target_link_libraries(game_test PRIVATE ${AVFILTER_LIBRARY})
target_link_libraries(game_test PRIVATE ${SWSCALE_LIBRARY})
target_link_libraries(game_test PRIVATE ${SWRESAMPLE_LIBRARY})

set(RESOURCE_FILES) # these files go into .app/Contents/Resources/
#set(RESOURCE_FILES arial.ttf AppIcon.icns) # these files go into .app/Contents/Resources/
#set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)
set_target_properties(game_test PROPERTIES MACOSX_BUNDLE TRUE MACOSX_FRAMEWORK_IDENTIFIER SapphireForge.TestApp RESOURCE "${RESOURCE_FILES}")
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
target_link_libraries(game_test PRIVATE ${DYLIB_PATHS})
file(COPY ${DYLIB_PATHS} DESTINATION ${APP_BUNDLE_FRAMEWORKS_DIR})

if(!XCODE)
add_custom_command(TARGET ${PROJECT_NAME} 
    POST_BUILD COMMAND 
    install_name_tool -add_rpath "@executable_path/../Frameworks/"
    "${APP_BUNDLE_CONTENTS_DIR}/MacOS/${PROJECT_NAME}")
endif()


#bundle game resources
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/../romfs"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/../SapphireEngine"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources"
)

if(APPLE)
set(APPS "\${CMAKE_INSTALL_PREFIX}/game_test.app")
INSTALL(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\" \"${APP_BUNDLE_CONTENTS_DIR}/Frameworks\")
        " COMPONENT Runtime)

    set(CPACK_BINARY_DRAGNDROP ON)
    include(CPack)
endif()