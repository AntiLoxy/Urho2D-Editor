#include "IView.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

namespace Geode
{
	IView::IView(Context* context, String layoutFile, UIElement* parent) : Object(context)
	{
		auto ui = GetSubsystem<UI>();
		auto cache = GetSubsystem<ResourceCache>();

		uiRoot_ = ui->GetRoot();

		if (layoutFile == String::EMPTY)
		{
			elRoot_ = MakeShared<UIElement>(context);
		}
		else
		{
			elRoot_ = ui->LoadLayout(cache->GetResource<XMLFile>(layoutFile));
			assert(elRoot_);

			name_ = elRoot_->GetName();
		}

		if (parent == nullptr)
		{
			uiRoot_->AddChild(elRoot_);
		}
		else
		{
			parent->AddChild(elRoot_);
		}
	}

	IView::~IView()
	{
		elRoot_->Remove();
	}

	void IView::SetName(String name)
	{
		name_ = name;
	}

	SharedPtr<UIElement> IView::GetElementRoot()
	{
		return elRoot_;
	}

	String IView::GetName()
	{
		return name_;
	}
}