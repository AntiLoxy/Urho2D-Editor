#include "IWindowView.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>

using namespace Urho3D;

namespace Geode
{
	IWindowView::IWindowView(Context* context, String layoutFile) : IView(context, layoutFile), mode_(Mode::NORMAL), opened_(true)
	{
		assert(elRoot_->IsInstanceOf<Window>());
		win_ = static_cast<Window*>(elRoot_.Get());

		header_ = win_->GetChildDynamicCast<UIElement>("Header", true);
		assert(header_);

		title_ = win_->GetChildDynamicCast<Text>("Title", true);
		assert(title_);

		closeButton_ = win_->GetChildDynamicCast<Button>("CloseButton", true);
		assert(closeButton_);

		body_ = win_->GetChildDynamicCast<UIElement>("Body", true);
		assert(body_);

		win_->BringToFront();

		SubscribeToEvent(win_, E_DRAGBEGIN, URHO3D_HANDLER(IWindowView, HandleDragBegin));
		SubscribeToEvent(win_, E_DRAGMOVE, URHO3D_HANDLER(IWindowView, HandleDragMove));
		SubscribeToEvent(win_, E_DRAGEND, URHO3D_HANDLER(IWindowView, HandleDragEnd));
		SubscribeToEvent(closeButton_, E_RELEASED, URHO3D_HANDLER(IWindowView, HandleCloseButton));
	}

	const String& IWindowView::GetTitle() const
	{
		return title_->GetText();
	}

	bool IWindowView::IsOpened()
	{
		return opened_;
	}

	void IWindowView::SetMode(Mode mode)
	{
		if (mode_ == mode)
		{
			return;
		}

		auto ui = GetSubsystem<UI>();

		if (mode == Mode::NORMAL)
		{
			ui->GetRoot()->AddChild(GetElementRoot());
			header_->SetVisible(true);
			win_->SetMovable(true);
		}
		else if (mode == Mode::DOCKED)
		{
			ui->GetRoot()->RemoveChild(GetElementRoot());
			header_->SetVisible(false);
			win_->SetMovable(false);
		}

		mode_ = mode;
	}

	void IWindowView::SetTitle(String title)
	{
		title_->SetText(title);
	}

	void IWindowView::Open()
	{
		win_->SetVisible(true);
		opened_ = true;

		VariantMap eventData;
		eventData[WindowViewOpened::P_ELEMENT] = this;
		SendEvent(E_WINDOWVIEW_OPENED, eventData);

		OnOpened();
	}

	void IWindowView::Close()
	{
		win_->SetVisible(false);
		opened_ = false;

		VariantMap eventData;
		eventData[WindowViewClosed::P_ELEMENT] = this;
		SendEvent(E_WINDOWVIEW_CLOSED, eventData);

		OnClosed();
	}

	void IWindowView::OnClosed()
	{
	}

	void IWindowView::OnOpened()
	{
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void IWindowView::HandleCloseButton(StringHash, VariantMap&)
	{
		Close();
	}

	void IWindowView::HandleDragBegin(StringHash, VariantMap& eventData)
	{
		eventData[WindowViewDragBegin::P_ELEMENT] = this;
		SendEvent(E_WINDOWVIEW_DRAGBEGIN, eventData);
	}

	void IWindowView::HandleDragMove(StringHash, VariantMap& eventData)
	{
		eventData[WindowViewDragMove::P_ELEMENT] = this;
		SendEvent(E_WINDOWVIEW_DRAGMOVE, eventData);
	}

	void IWindowView::HandleDragEnd(StringHash, VariantMap& eventData)
	{
		eventData[WindowViewDragEnd::P_ELEMENT] = this;
		SendEvent(E_WINDOWVIEW_DRAGEND, eventData);
	}
}
