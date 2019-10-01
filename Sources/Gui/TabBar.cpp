#include "TabBar.h"

#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Button.h>

using namespace Urho3D;

namespace Geode
{
	TabBar::TabBar(Context* context) : BorderImage(context),
		tabsClosable_(false),
		currentIndex_(0)
	{
		id_ = Rand();
	}

	void TabBar::RegisterObject(Context* context)
	{
		context->RegisterFactory<TabBar>();
		URHO3D_COPY_BASE_ATTRIBUTES(BorderImage);

		URHO3D_ACCESSOR_ATTRIBUTE("TabsClosable", GetTabsClosable, SetTabsClosable, bool, false, AM_FILE);
	}

	unsigned int TabBar::AddTab(const String& label, const String& toolTip)
	{
		auto newTab = CreateTab(label, toolTip);
		AddChild(newTab);

		tabs_.Push(newTab);
		auto indexedAt = tabs_.IndexOf(newTab);

		VariantMap eventData;
		eventData[TabAdded::P_INDEX] = indexedAt;
		SendEvent(E_TAB_ADDED, eventData);

		return indexedAt;
	}

	unsigned int TabBar::InsertTab(unsigned int index, const String& label, const String& toolTip)
	{
		auto newTab = CreateTab(label, toolTip);
		InsertChild(index, newTab);

		tabs_.Insert(index, newTab);
		auto indexedAt = tabs_.IndexOf(newTab);

		VariantMap eventData;
		eventData[TabAdded::P_INDEX] = indexedAt;
		SendEvent(E_TAB_ADDED, eventData);

		return indexedAt;
	}

	void TabBar::RemoveTab(unsigned int index)
	{
		if (index >= tabs_.Size())
		{
			return;
		}

		auto foundedTab = tabs_[index];

		UnsubscribeFromEvent(foundedTab, E_TOGGLED);
		tabs_.Remove(foundedTab);
		RemoveChild(foundedTab);

		VariantMap eventData;
		eventData[TabRemoved::P_INDEX] = index;
		SendEvent(E_TAB_REMOVED, eventData);
	}

	void TabBar::MoveTab(unsigned int from, unsigned int to)
	{
		if (from >= tabs_.Size() || to >= tabs_.Size())
		{
			return;
		}

		auto tab = tabs_[from];

		tabs_.Insert(to, tab);
		tabs_.Erase(from);

		InsertChild(to, tab);

		VariantMap eventData;
		eventData[TabMoved::P_INDEX_FROM] = from;
		eventData[TabMoved::P_INDEX_TO] = to;
		SendEvent(E_TAB_MOVED, eventData);
	}

	void TabBar::SelectTab(unsigned int index)
	{
		if (index >= tabs_.Size())
		{
			return;
		}

		auto foundedTab = tabs_[index];
		foundedTab->SetChecked(true);
	}

	void TabBar::SetTabVisible(unsigned int index, bool visible)
	{
		if (index >= tabs_.Size())
		{
			return;
		}

		auto foundedTab = tabs_[index];
		foundedTab->SetVisible(visible);
	}

	void TabBar::SetTabsClosable(bool tabsClosable)
	{
		if (tabsClosable_ == tabsClosable)
		{
			return;
		}

		if (tabsClosable)
		{
			for (auto tab : tabs_)
			{
				CreateCloseButton(tab);
			}
		}
		else
		{
			for (auto tab : tabs_)
			{
				tab->RemoveChild(tab->GetChild("CloseButton", false));
			}
		}

		tabsClosable_ = tabsClosable;
	}

	bool TabBar::GetTabVisible(unsigned int index) const
	{
		if (index >= tabs_.Size())
		{
			return false;
		}

		auto foundedTab = tabs_[index];
		return foundedTab->IsVisible();
	}

	bool TabBar::GetTabsClosable() const
	{
		return tabsClosable_;
	}

	bool TabBar::HasTab(unsigned int index) const
	{
		return index < tabs_.Size();
	}

	unsigned int TabBar::GetCurrentIndex() const
	{
		return currentIndex_;
	}

	SharedPtr<Radio> TabBar::CreateTab(const String& label, const String& toolTip)
	{
		auto tab = MakeShared<Radio>(context_);
		tab->SetStyle("TabBarTabItem");
		tab->SetGroupName("TabBar_" + String(id_));

		auto text = tab->CreateChild<Text>("Text");
		text->SetStyleAuto();
		text->SetText(label);
		text->SetVerticalAlignment(VerticalAlignment::VA_CENTER);
		text->SetHorizontalAlignment(HorizontalAlignment::HA_CENTER);

		if (toolTip != String::EMPTY)
		{
			CreateToolTip(tab, toolTip, { 0, tab->GetHeight() });
		}

		if (tabsClosable_)
		{
			CreateCloseButton(tab);
		}

		SubscribeToEvent(tab, E_TOGGLED, URHO3D_HANDLER(TabBar, HandleTabToggled));

		return tab;
	}

	void TabBar::CreateToolTip(UIElement* parent, String text, IntVector2 offset)
	{
		auto toolTip = parent->CreateChild<ToolTip>("ToolTip");
		toolTip->SetStyleAuto();
		toolTip->SetPosition(toolTip->GetPosition() + offset);

		auto toolTipText = toolTip->CreateChild<Text>("Text");
		toolTipText->SetStyleAuto();
		toolTipText->SetText(text);
	}

	void TabBar::CreateCloseButton(UIElement* parent)
	{
		auto closeButton = parent->CreateChild<Button>("CloseButton");
		closeButton->SetStyle("TabBarTabItemCloseButton");

		SubscribeToEvent(closeButton, E_RELEASED, URHO3D_HANDLER(TabBar, HandleTabCloseButtonReleased));
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void TabBar::HandleTabCloseButtonReleased(StringHash, VariantMap& eventData)
	{
		auto closeButton = static_cast<Button*>(eventData[Released::P_ELEMENT].GetPtr());
		auto tab = static_cast<Radio*>(closeButton->GetParent());
		auto index = tabs_.IndexOf(tab);

		VariantMap sendEventData;
		sendEventData[TabCloseReleased::P_INDEX] = index;
		SendEvent(E_TAB_CLOSE_RELEASED, sendEventData);

		RemoveTab(index);
	}

	void TabBar::HandleTabToggled(StringHash eventType, VariantMap& eventData)
	{
		auto tab = static_cast<Radio*>(eventData[Toggled::P_ELEMENT].GetPtr());
		auto state = eventData[Toggled::P_STATE].GetBool();

		if (state)
		{
			currentIndex_ = tabs_.IndexOf(tab);

			VariantMap sendEventData;
			sendEventData[TabSelectionChanged::P_INDEX] = currentIndex_;
			SendEvent(E_TAB_SELECTION_CHANGED, sendEventData);
		}
	}
}
