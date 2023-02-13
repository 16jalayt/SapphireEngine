#include <Nancy/GUI.h>
#include <string>
#include <Engine/utils.h>
#include <Engine/Button.h>
#include <Engine/graphics.h>

std::shared_ptr<SDL_Texture> GUI::canvas;
//SDL_Rect GUI::canvasRect;

GUI::GUI()
{
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