//TODO: update convention for engine includes
#include "Engine/GUIEngine.h"
#include <string>

#if !defined(__SWITCH__) && !defined(__APPLE__)
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include "Engine/GuiStyles.h"
#endif

#include "Engine/Utils.h"
#include "Engine/Config.h"

std::shared_ptr<SDL_Texture> GUIEngine::canvas;
SDL_Rect GUIEngine::canvasRect;

GUIEngine::GUIEngine()
{
	//For now just patch out imgui from switch port
#if !defined(__SWITCH__) && !defined(__APPLE__)
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	imCtx = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.IniFilename = NULL;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(Graphics::window.get(), Graphics::renderer.get());
	ImGui_ImplSDLRenderer2_Init(Graphics::renderer.get());

	//StyleClean();
	StyleCleanBlue();
	//StyleSteam();
#endif
}

GUIEngine::~GUIEngine()
{
#if !defined(__SWITCH__) && !defined(__APPLE__)
	// Cleanup IMGUI
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
#endif
}

void GUIEngine::Draw()
{
	if (this != NULL)
	{
		for (auto& rect : rects) {
			SDL_SetRenderDrawColor(Graphics::renderer.get(), rect.r, rect.g, rect.b, 0xFF);
			SDL_RenderFillRect(Graphics::renderer.get(), &rect.rect);
		}

		for (auto& sprite : statics) {
			sprite->Draw();
		}

		for (auto& button : buttons) {
			button->Draw();
		}
	}

	if (canvas)
		SDL_RenderCopy(Graphics::renderer.get(), canvas.get(), NULL, &canvasRect);
}

void GUIEngine::EventProc(SDL_Event event)
{
#if !defined(__SWITCH__) && !defined(__APPLE__)
	ImGui_ImplSDL2_ProcessEvent(&event);
#endif

	if (this != NULL)
	{
		for (auto& button : buttons) {
			button->Event(event);
		}
	}
}

void GUIEngine::AddSprite(Sprite_ptr sprite)
{
	statics.push_back(sprite);
}

void GUIEngine::AddButton(Button_ptr button)
{
	buttons.push_back(button);
}

void GUIEngine::AddRect(GUI_Rect rect)
{
	rects.push_back(rect);
}

#if !defined(__SWITCH__) && !defined(__APPLE__)

//https://github.com/ocornut/imgui/issues/1537#issuecomment-355569554
//Animates and green
void GUIEngine::ToggleButton(const char* str_id, bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.08f;
	if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
	else
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

//Does not animate and blue
void GUIEngine::ToggleButtonV2(const char* str_id, bool* v)
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked()) *v = !*v;
	ImGuiContext& gg = *GImGui;
	float ANIM_SPEED = 0.085f;
	if (gg.LastActiveId == gg.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
		float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
	if (ImGui::IsItemHovered())
		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonActive] : ImVec4(0.78f, 0.78f, 0.78f, 1.0f)), height * 0.5f);
	else
		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_Button] : ImVec4(0.85f, 0.85f, 0.85f, 1.0f)), height * 0.50f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + (*v ? 1 : 0) * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

//From imgui_demo.cpp
void GUIEngine::ShowHelpMarker(const char* desc) {
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
#endif