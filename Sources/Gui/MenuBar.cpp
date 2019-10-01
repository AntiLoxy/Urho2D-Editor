#include "MenuBar.h"

#include <Urho3D/UI/Text.h>

using namespace Urho3D;

namespace Geode
{
	MenuBar::MenuBar(Context* context) : BorderImage(context)
	{}

	void MenuBar::RegisterObject(Context* context)
	{
		context->RegisterFactory<MenuBar>();
		URHO3D_COPY_BASE_ATTRIBUTES(BorderImage);
	}

	FlyMenu* MenuBar::AddMenu(const String& text)
	{
		FlyMenu* menu = CreateChild<FlyMenu>("MB_FlyMenu");
		menu->SetStyle("MenuBarItem");

		Text* label = menu->CreateChild<Text>("MB_FlyMenuLabel");
		label->SetText(text);
		label->SetStyle("MenuBarItemText");

		menu->SetFixedWidth(label->GetWidth() + 20);

		return menu;
	}
}
