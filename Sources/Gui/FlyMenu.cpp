#include "FlyMenu.h"

#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>

using namespace Urho3D;

namespace Geode
{
	FlyMenu::FlyMenu(Context* context) : Menu(context)
	{
		SetEnabled(true);
		focusMode_ = FM_FOCUSABLE;

		auto popup = new Window(context);
		popup->SetInternal(true);
		SetPopup(popup);
	}

	FlyMenu::~FlyMenu()
	{
		popup_->Remove();
	}

	void FlyMenu::RegisterObject(Context* context)
	{
		context->RegisterFactory<FlyMenu>();
		URHO3D_COPY_BASE_ATTRIBUTES(Menu);
	}

	Button* FlyMenu::AddButton(const String& text)
	{
		auto btn = popup_->CreateChild<Button>("FM_Button");
		btn->SetStyle("FlyMenuButton");

		auto label = btn->CreateChild<Text>("FM_ButtonText");
		label->SetText(text);
		label->SetStyle("FlyMenuButtonText");

		SubscribeToEvent(btn, E_CLICKEND, URHO3D_HANDLER(FlyMenu, HandleItemClickEnd));

		return btn;
	}

	CheckBox* FlyMenu::AddToggle(const String& text)
	{
		auto toggle = popup_->CreateChild<CheckBox>("FM_Toggle");
		toggle->SetStyle("FlyMenuToggle");

		auto label = toggle->CreateChild<Text>("FM_ToggleText");
		label->SetText(text);
		label->SetStyle("FlyMenuToggleText");

		SubscribeToEvent(toggle, E_CLICKEND, URHO3D_HANDLER(FlyMenu, HandleItemClickEnd));

		return toggle;
	}

	FlyMenu* FlyMenu::AddSubMenu(const String& text)
	{
		auto menu = popup_->CreateChild<FlyMenu>("FM_SubMenu");
		menu->SetStyle("FlyMenuSubMenu");
		menu->SetPopupOffset(menu->GetWidth(), 0);

		auto label = menu->CreateChild<Text>("FM_SubMenuText");
		label->SetText(text);
		label->SetStyle("FlyMenuSubMenuText");

		auto caret = menu->CreateChild<BorderImage>("FM_SubMenuCaret");
		caret->SetStyle("FlyMenuSubMenuCaret");

		return menu;
	}

	UIElement* FlyMenu::AddSeparator()
	{
		auto spacer = popup_->CreateChild<UIElement>("FM_Separator");
		spacer->SetStyle("FlyMenuSeparator");

		return spacer;
	}

	void FlyMenu::Reset()
	{
		popup_->RemoveAllChildren();
	}

	void FlyMenu::OnShowPopup()
	{
		SetFocus(true);
	}

	void FlyMenu::OnHidePopup()
	{
		SetFocus(false);

		VariantMap sendEventData;
		sendEventData[FlyMenuClosed::P_ELEMENT] = this;
		SendEvent(E_FLYMENU_CLOSED, sendEventData);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void FlyMenu::HandleItemClickEnd(StringHash, VariantMap& eventData)
	{
		auto item = static_cast<UIElement*>(eventData["Element"].GetPtr());

		if (item != nullptr)
		{
			ShowPopup(false);

			VariantMap sendEventData;
			sendEventData[FlyMenuItemClicked::P_ELEMENT] = this;
			sendEventData[FlyMenuItemClicked::P_ITEM] = item;
			SendEvent(E_FLYMENU_ITEM_CLICKED, sendEventData);
		}
	}
}
