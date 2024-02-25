//TODO: update convention for engine includes
#include "Engine/GUI.h"
#include <string>

#ifndef __SWITCH__
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include "GuiStyles.h"
#endif

#include "Engine/utils.h"
#include "Engine/Button.h"
#include "Engine/graphics.h"
#include "Nancy/Loader.h"
#include "Config.h"

std::shared_ptr<SDL_Texture> GUI::canvas;
SDL_Rect GUI::canvasRect;

GUI::GUI()
{
	//For now just patch out imgui from switch port
#ifndef __SWITCH__
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.IniFilename = NULL;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(Graphics::window.get(), Graphics::renderer.get());
	ImGui_ImplSDLRenderer2_Init(Graphics::renderer.get());

	//StyleClean();
	StyleCleanBlue();
	//StyleSteam();
#endif
}

GUI::~GUI()
{
#ifndef __SWITCH__
	// Cleanup IMGUI
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
#endif
}

void GUI::Draw()
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

#ifndef __SWITCH__
	//TODO:imgui cursor position doesn't work fullscreen
	// Start the Dear ImGui frame
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	bool show_demo_window = false;
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	drawCheatSheet();

	// Rendering
	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
#endif
}

void GUI::EventProc(SDL_Event event)
{
	if (this != NULL)
	{
		for (auto& button : buttons) {
			button->Event(event);
		}
	}
}

void GUI::AddSprite(Sprite_ptr sprite)
{
	statics.push_back(sprite);
}

void GUI::AddButton(Button_ptr button)
{
	buttons.push_back(button);
}

void GUI::AddRect(GUI_Rect rect)
{
	rects.push_back(rect);
}

#ifndef __SWITCH__
void GUI::drawCheatSheet()
{
	//TODO: key combo to toggle
	if (cheatSheetOpen)
	{
		//Forces to be immoveable
		//ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(700, 500));

		//Todo: temp
		//ImGui::SetNextWindowCollapsed(true);

		// Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Begin("Cheet Sheet", &cheatSheetOpen,
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		//ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

		//ImGui::SameLine();
		//ImGui::Separator();
		//HelpMarker("Using TableNextRow() + calling TableSetColumnIndex() _before_ each cell, in a loop.");
		if (ImGui::BeginTabBar("MyTabBar"))
		{
			if (ImGui::BeginTabItem("General"))
			{
				std::string inputScene = currentScene->sceneName;
				//TODO:redo flags
				if (ImGui::InputText("Scene Num", &inputScene, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					//printf("changed!\n");
					//std::string sceneName = std::to_string(sceneNum);
					//sceneChangeName = inputScene;
					//sceneNum already set
					//sceneNum = changeTo;
					//sceneChangeFlag = true;
					Loader::loadScene(inputScene);
				}
				//ToggleButtonV2("hi", &toggle);
				if (ImGui::Button("Reload scene"))
					ReloadScene();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Inventory"))
			{
				//ImGui::Checkbox("test", &check);
				if (ImGui::Button("Click Me"))
					printf("Ow! Stop it!\n");
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Events"))
			{
				if (ImGui::BeginTable("table1", 4))
				{
					for (int i = 0; i < 550; i++)
					{
						ImGui::TableNextColumn();
						ImGui::Checkbox((std::to_string(i + 1000) + ":" + flagDesc[i]).c_str(), &flags[i]);
						if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) // With a delay
							ImGui::SetTooltip(flagDesc[i].c_str());
					}
					ImGui::EndTable();
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Editor"))
			{
				//Begin editing button
				// toggle for button debug
				//allow drag of sprites
				//show xy/props of selected sprite
				ToggleButtonV2("isEditing", &isEditing);
				ImGui::SameLine();
				ImGui::Text("Edit mode");

				ToggleButtonV2("debugHot", &Config::debugHot);
				ImGui::SameLine();
				ImGui::Text("Show debug hotzones");

				ImGui::EndTabItem();
			}
			//TODO: new tab with map like Humongous
			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}

//https://github.com/ocornut/imgui/issues/1537#issuecomment-355569554
//Animates and green
void GUI::ToggleButton(const char* str_id, bool* v)
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
void GUI::ToggleButtonV2(const char* str_id, bool* v)
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
void GUI::ShowHelpMarker(const char* desc) {
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