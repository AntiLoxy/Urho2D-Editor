#include "PanelView.h"

#include <Urho3D/UI/UI.h>

using namespace Urho3D;

namespace Geode
{
	PanelView::PanelView(Context* context, UIElement* parent) : IView(context, "UI/PanelView.xml", parent)
	{
		currentWindowView_ = nullptr;

		tabBar_ = elRoot_->GetChildDynamicCast<TabBar>("TabBar", true);
		assert(tabBar_);

		scrollView_ = elRoot_->GetChildDynamicCast<ScrollView>("ScrollView", true);
		assert(scrollView_);

		scrollContent_ = MakeShared<UIElement>(context_);
		scrollContent_->SetDefaultStyle(elRoot_->GetDefaultStyle());
		scrollContent_->SetLayoutMode(LayoutMode::LM_VERTICAL);
		scrollContent_->SetLayoutBorder(IntRect(6, 6, 6, 6));
		scrollContent_->SetMinWidth(scrollView_->GetWidth() - 4);
		scrollView_->SetContentElement(scrollContent_);

		SubscribeToEvent(tabBar_, E_TAB_SELECTION_CHANGED, URHO3D_HANDLER(PanelView, HandleTabSelectionChanged));
		SubscribeToEvent(tabBar_, E_TAB_REMOVED, URHO3D_HANDLER(PanelView, HandleTabRemoved));
	}

	bool PanelView::RegisterWindowView(SharedPtr<IWindowView> windowView)
	{
		if (IsRegisteredWindowView(windowView))
		{
			return false;
		}

		registeredWindowViews_.Push(windowView);

		SubscribeToEvent(windowView, E_WINDOWVIEW_CLOSED, URHO3D_HANDLER(PanelView, HandleWindowViewClosed));
		SubscribeToEvent(windowView, E_WINDOWVIEW_OPENED, URHO3D_HANDLER(PanelView, HandleWindowViewOpened));
		SubscribeToEvent(windowView, E_WINDOWVIEW_DRAGEND, URHO3D_HANDLER(PanelView, HandleWindowViewDragEnd));
		
		return true;
	}

	void PanelView::UnregisterWindowView(IWindowView* windowView)
	{
		auto index = IndexOfRegisteredWindowView(windowView);

		if (index == registeredWindowViews_.Size())
		{
			return;
		}

		registeredWindowViews_.Erase(index);
		UnsubscribeFromEvent(windowView, E_WINDOWVIEW_DRAGEND);
	}

	unsigned int PanelView::IndexOfRegisteredWindowView(IWindowView* windowView)
	{
		for (unsigned int i = 0; i < registeredWindowViews_.Size(); i++)
		{
			auto registeredWindowView = registeredWindowViews_[i];

			if (registeredWindowView.Get() == windowView)
			{
				return i;
			}
		}

		return registeredWindowViews_.Size();
	}

	bool PanelView::IsRegisteredWindowView(IWindowView* windowView)
	{
		return IndexOfRegisteredWindowView(windowView) != registeredWindowViews_.Size();
	}

	bool PanelView::AddTab(IWindowView* windowView, bool autoSelect)
	{
		if (windowView == nullptr)
		{
			return false;
		}

		if (!IsRegisteredWindowView(windowView))
		{
			return false;
		}

		if (windowViews_.Contains(windowView))
		{
			return false;
		}

		windowView->SetMode(IWindowView::Mode::DOCKED);
		windowView->GetElementRoot()->SetVisible(false);
		windowViews_.Push(windowView);

		auto indexedAt = tabBar_->AddTab(windowView->GetTitle());

		if (autoSelect)
		{
			SelectTab(indexedAt);
		}

		return indexedAt;
	}

	void PanelView::RemoveTab(unsigned int index)
	{
		tabBar_->RemoveTab(index);
	}

	void PanelView::SelectTab(unsigned int index)
	{
		tabBar_->SelectTab(index);
	}

	void PanelView::SetContent(UIElement* element)
	{
		scrollContent_->RemoveAllChildren();
		scrollContent_->AddChild(element);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void PanelView::HandleTabRemoved(StringHash, VariantMap& eventData)
	{
		auto index = eventData[TabRemoved::P_INDEX].GetInt();
		auto windowView = windowViews_[index];

		windowView->SetMode(IWindowView::Mode::NORMAL);
		windowViews_.Remove(windowView);

		if (currentWindowView_ == windowView)
		{
			currentWindowView_ = nullptr;

			if (tabBar_->HasTab(index))
			{
				SelectTab(index);
			}
			else
			{
				SelectTab(index - 1);
			}
		}
	}

	void PanelView::HandleTabSelectionChanged(StringHash, VariantMap& eventData)
	{
		auto index = eventData[TabSelectionChanged::P_INDEX].GetInt();
		auto windowView = windowViews_[index];
		auto windowViewRoot = windowView->GetElementRoot();
		
		if (currentWindowView_ != nullptr)
		{
			currentWindowView_->GetElementRoot()->SetVisible(false);
		}

		windowViewRoot->SetVisible(true);
		SetContent(windowViewRoot);
		currentWindowView_ = windowView;
	}

	void PanelView::HandleWindowViewClosed(StringHash, VariantMap& eventData)
	{
		auto closedWindowView = static_cast<IWindowView*>(eventData[WindowViewClosed::P_ELEMENT].GetPtr());
		auto index = windowViews_.IndexOf(closedWindowView);

		if (index == windowViews_.Size())
		{
			return;
		}

		tabBar_->SetTabVisible(index, false);

		if (currentWindowView_ == closedWindowView)
		{
			if (tabBar_->HasTab(index))
			{
				SelectTab(index);
			}
			else
			{
				SelectTab(index - 1);
			}
		}
	}

	void PanelView::HandleWindowViewOpened(StringHash, VariantMap& eventData)
	{
		auto openedWindowView = static_cast<IWindowView*>(eventData[WindowViewOpened::P_ELEMENT].GetPtr());
		auto index = windowViews_.IndexOf(openedWindowView);

		if (index == windowViews_.Size())
		{
			return;
		}

		tabBar_->SetTabVisible(index, true);
	}

	void PanelView::HandleWindowViewDragEnd(StringHash, VariantMap& eventData)
	{
		auto windowView = static_cast<IWindowView*>(eventData[WindowViewDragEnd::P_ELEMENT].GetPtr());

		auto ui = GetSubsystem<UI>();
		auto cursorPosition = ui->GetCursorPosition();

		if (elRoot_->IsInside(cursorPosition, true))
		{
			AddTab(windowView, true);
		}
	}
}