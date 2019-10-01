#include "ContextMenu.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UI.h>

using namespace Urho3D;

namespace Geode
{
	ContextMenu::ContextMenu(Context* context) : Object(context)
	{
		UIElement* uiRoot = GetSubsystem<UI>()->GetRoot();

		flyMenu_ = uiRoot->CreateChild<FlyMenu>();
		flyMenu_->SetStyleAuto();
		flyMenu_->SetVisible(false);

		SubscribeToEvent(flyMenu_, E_FLYMENU_CLOSED, URHO3D_HANDLER(ContextMenu, HandleFlyMenuClosed));
	}

	ContextMenu::~ContextMenu()
	{
		flyMenu_->Remove();
	}

	FlyMenu* ContextMenu::Get()
	{
		flyMenu_->Reset();
		return flyMenu_;
	}

	void ContextMenu::Show(int x, int y)
	{
		flyMenu_->SetPosition(x, y);
		flyMenu_->SetEnabled(true);
		flyMenu_->ShowPopup(true);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void ContextMenu::HandleFlyMenuClosed(StringHash, VariantMap&)
	{
		flyMenu_->SetEnabled(false);
		flyMenu_->Reset();
	}
}
