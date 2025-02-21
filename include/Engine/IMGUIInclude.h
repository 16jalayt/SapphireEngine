#pragma once

#if !SDL_VERSION_ATLEAST(2,0,17)
#warning This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function. Disabling debug UI.
  #if !defined(NO_IMGUI)
    #define NO_IMGUI
  #endif
#endif

#if defined(__SWITCH__)  || defined(__APPLE__)|| defined(__VITA__)
  #if !defined(NO_IMGUI)
    #define NO_IMGUI
  #endif
#endif

#if !defined(NO_IMGUI)
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#endif