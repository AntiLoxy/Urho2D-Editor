#include "ToolBar.h"
#include "Helper.h"

#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/Text.h>

using namespace Urho3D;

namespace Geode
{
	ToolBar::ToolBar(Context* context) : BorderImage(context)
	{}

	void ToolBar::RegisterObject(Context* context)
	{
		context->RegisterFactory<ToolBar>();
		URHO3D_COPY_BASE_ATTRIBUTES(BorderImage);
	}

	Button* ToolBar::AddButton(String name, String labelText)
	{
		auto btn = CreateChild<Button>(name);
		btn->SetStyle("ToolBarButton");

		auto label = btn->CreateChild<Text>("Label");
		label->SetStyle("ToolBarButtonLabel");
		label->SetText(labelText);

		btn->SetFixedWidth(label->GetWidth() + 20);

		return btn;
	}

	Button* ToolBar::AddIconButton(String name, String iconName, String tooltipText)
	{
		auto btn = CreateChild<Button>(name);
		btn->SetStyle("ToolBarIconButton");

		Helper::CreateIcon(btn, iconName, HA_CENTER, VA_CENTER);
		Helper::CreateToolTip(btn, tooltipText, { 0, btn->GetHeight() });

		return btn;
	}

	Radio* ToolBar::AddIconRadio(String name, String rgn, String iconName, String tooltipText)
	{
		auto radio = CreateChild<Radio>(name);
		radio->SetStyle("ToolBarIconRadio");
		radio->SetGroupName(rgn);

		Helper::CreateIcon(radio, iconName, HA_CENTER, VA_CENTER);
		Helper::CreateToolTip(radio, tooltipText, { 0, radio->GetHeight() });

		return radio;
	}

	CheckBox* ToolBar::AddIconToggle(String name, String iconName, String tooltipText)
	{
		auto toggle = CreateChild<CheckBox>(name);
		toggle->SetStyle("ToolBarIconToggle");

		Helper::CreateIcon(toggle, iconName, HA_CENTER, VA_CENTER);
		Helper::CreateToolTip(toggle, tooltipText, { 0, toggle->GetHeight() });

		return toggle;
	}

	UIElement* ToolBar::AddSeparator()
	{
		auto spacer = CreateChild<UIElement>("TB_Separator");
		spacer->SetStyle("ToolBarSeparator");

		return spacer;
	}
}
