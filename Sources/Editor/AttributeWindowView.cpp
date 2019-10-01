#include "AttributeWindowView.h"
#include "../Gui/GuiEvents.h"

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Core/CoreEvents.h>

using namespace Urho3D;

namespace Geode
{
	AttributeWindowView::AttributeWindowView(Context* context, Geode::CommandHistory::Ptr commandHistory, Geode::EditorScene::Ptr editorScene) : IWindowView(context, "UI/AttributeWindowView.xml")
	{
		commandHistory_ = commandHistory;
		editorScene_ = editorScene;
		selectedSerializable_ = nullptr;

		attributesForm_ = elRoot_->GetChildDynamicCast<ReactiveForm>("AttributesForm", true);
		assert(attributesForm_);

		SetSelectedSerializable(editorScene_->GetSelectedObjectAs<Serializable>());

		SubscribeToEvent(editorScene, E_SELECTEDOBJECTCHANGED, URHO3D_HANDLER(AttributeWindowView, HandleSelectedObjectChanged));
		SubscribeToEvent(attributesForm_, E_REACTIVEFORM_DATACHANGED, URHO3D_HANDLER(AttributeWindowView, HandleReactiveFormDataChanged));
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void AttributeWindowView::HandleSelectedObjectChanged(StringHash, VariantMap& eventData)
	{
		SetSelectedSerializable(editorScene_->GetSelectedObjectAs<Serializable>());
	}

	void AttributeWindowView::HandleReactiveFormDataChanged(StringHash, VariantMap& eventData)
	{
		auto attributeName = eventData[ReactiveFormDataChanged::P_ATTRIBUTE_NAME].GetString();
		auto attributeOldValue = eventData[ReactiveFormDataChanged::P_OLDVALUE];
		auto attributeValue = eventData[ReactiveFormDataChanged::P_VALUE];

		CommandInvoker(MakeShared<ChangeAttributeSerializableCommand>(editorScene_, attributeName, attributeOldValue, attributeValue), commandHistory_).Exec();
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void AttributeWindowView::SetSelectedSerializable(Serializable* selectedSerializable)
	{
		if (selectedSerializable != nullptr)
		{
			auto attributes = selectedSerializable->GetAttributes();
			attributesForm_->Clear();

			for (auto i = 0; i < attributes->Size(); i++)
			{
				auto attribute = attributes->At(i);

				if (attribute.mode_ & AM_FILE)
				{
					auto field = attributesForm_->AddAttributeField(selectedSerializable, attribute.name_, attribute.name_);
					field->SetBidirectionnalBinding(true);
				}
			}

			attributesForm_->SetVisible(true);
		}
		else
		{
			attributesForm_->Clear();
			attributesForm_->SetVisible(false);
		}

		selectedSerializable_ = selectedSerializable;
	}
}
