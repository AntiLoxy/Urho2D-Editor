#include "ReactiveForm.h"

#include <Urho3D/Core/CoreEvents.h>

using namespace Urho3D;

namespace Geode
{
	AttributeField::AttributeField(Context* context) : UIElement(context), activated_(true), bidirectionnalBinding_(false), serializable_(nullptr), control_(nullptr)
	{
		label_ = CreateChild<Text>("AF_Label");
		label_->SetInternal(true);

		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(AttributeField, HandleUpdate));
	}

	void AttributeField::RegisterObject(Context* context)
	{
		context->RegisterFactory<AttributeField>();
		URHO3D_COPY_BASE_ATTRIBUTES(UIElement);
	}

	String AttributeField::GetAttributeName()
	{
		return attributeName_;
	}

	IControlBase* AttributeField::GetControl()
	{
		return control_;
	}

	void AttributeField::SetAttribute(Serializable* serializable, const String& attributeName)
	{
		if (control_)
		{
			UnsubscribeFromEvent(control_, E_CONTROLCHANGED);
			control_->Remove();
			control_ = nullptr;
		}

		if (serializable != nullptr)
		{
			auto value = serializable->GetAttribute(attributeName);
			auto type = value.GetType();

			if (type == VAR_FLOAT)
			{
				auto control = CreateChild<FloatControl>("AF_Control");
				control->SetValue(value.GetFloat());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_QUATERNION)
			{
				auto control = CreateChild<QuaternionControl>("AF_Control");
				control->SetValue(value.GetQuaternion());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_VECTOR2)
			{
				auto control = CreateChild<Vector2Control>("AF_Control");
				control->SetValue(value.GetVector2());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_VECTOR3)
			{
				auto control = CreateChild<Vector3Control>("AF_Control");
				control->SetValue(value.GetVector3());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_VECTOR4)
			{
				auto control = CreateChild<Vector4Control>("AF_Control");
				control->SetValue(value.GetVector4());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_RECT)
			{
				auto control = CreateChild<RectControl>("AF_Control");
				control->SetValue(value.GetRect());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_COLOR)
			{
				auto control = CreateChild<ColorControl>("AF_Control");
				control->SetValue(value.GetColor());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_INT)
			{
				auto control = CreateChild<IntegerControl>("AF_Control");
				control->SetValue(value.GetInt());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_INTVECTOR2)
			{
				auto control = CreateChild<IntVector2Control>("AF_Control");
				control->SetValue(value.GetIntVector2());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_INTVECTOR3)
			{
				auto control = CreateChild<IntVector3Control>("AF_Control");
				control->SetValue(value.GetIntVector3());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_INTRECT)
			{
				auto control = CreateChild<IntRectControl>("AF_Control");
				control->SetValue(value.GetIntRect());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_BOOL)
			{
				auto control = CreateChild<CheckboxControl>("AF_Control");
				control->SetValue(value.GetBool());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_STRING)
			{
				auto control = CreateChild<StringControl>("AF_Control");
				control->SetValue(value.GetString());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_STRINGVECTOR)
			{
				auto control = CreateChild<StringListControl>("AF_Control");
				control->SetValue(value.GetStringVector());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_RESOURCEREF)
			{
				auto control = CreateChild<ResourceRefControl>("AF_Control");
				control->SetValue(value.GetResourceRef());
				control->SetStyleAuto();
				control_ = control;
			}
			else if (type == VAR_VARIANTMAP)
			{
				auto control = CreateChild<VariantMapControl>("AF_Control");
				control->SetValue(value.GetVariantMap());
				control->SetStyleAuto();
				control_ = control;
			}
		}

		if (control_ != nullptr)
		{
			SubscribeToEvent(control_, E_CONTROLCHANGED, URHO3D_HANDLER(AttributeField, HandleControlChanged));
		}

		serializable_ = serializable;
		attributeName_ = attributeName;
	}

	void AttributeField::SetLabel(const String& label)
	{
		label_->SetText(label);
	}
	
	void AttributeField::SetBidirectionnalBinding(bool enable)
	{
		bidirectionnalBinding_ = enable;
	}

	void AttributeField::Activate()
	{
		activated_ = true;
		SetVisible(true);
		control_->SetBlockEvents(false);
	}

	void AttributeField::Desactivate()
	{
		activated_ = false;
		SetVisible(false);
		control_->SetBlockEvents(true);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void AttributeField::HandleControlChanged(StringHash, VariantMap& eventData)
	{
		if (activated_ == false || serializable_ == nullptr || control_ == nullptr)
		{
			return;
		}

		auto controlOldValue = eventData[ControlChanged::P_OLDVALUE];
		auto controlValue = eventData[ControlChanged::P_VALUE];
		auto controlValidated = eventData[ControlChanged::P_VALIDATED];

		serializable_->SetAttribute(attributeName_, controlValue);

		VariantMap sendEventData;
		sendEventData[AttributeFieldDataChanged::P_ELEMENT] = this;
		sendEventData[AttributeFieldDataChanged::P_ATTRIBUTE_NAME] = attributeName_;
		sendEventData[AttributeFieldDataChanged::P_OLDVALUE] = controlOldValue;
		sendEventData[AttributeFieldDataChanged::P_VALUE] = controlValue;
		sendEventData[AttributeFieldDataChanged::P_VALIDATED] = controlValidated;
		SendEvent(E_ATTRIBUTEFIELD_DATACHANGED, sendEventData);
	}

	void AttributeField::HandleUpdate(StringHash, VariantMap&)
	{
		if (activated_ == false || bidirectionnalBinding_ == false || serializable_ == nullptr || control_ == nullptr || control_->IsEditing())
		{
			return;
		}

		control_->SetBlockEvents(true);

		auto value = serializable_->GetAttribute(attributeName_);
		auto type = value.GetType();

		if (type == VAR_FLOAT)
		{
			auto control = static_cast<FloatControl*>(control_);
			control->SetValue(value.GetFloat());
		}
		else if (type == VAR_QUATERNION)
		{
			auto control = static_cast<QuaternionControl*>(control_);
			control->SetValue(value.GetQuaternion());
		}
		else if (type == VAR_VECTOR2)
		{
			auto control = static_cast<Vector2Control*>(control_);
			control->SetValue(value.GetVector2());
		}
		else if (type == VAR_VECTOR3)
		{
			auto control = static_cast<Vector3Control*>(control_);
			control->SetValue(value.GetVector3());
		}
		else if (type == VAR_VECTOR4)
		{
			auto control = static_cast<Vector4Control*>(control_);
			control->SetValue(value.GetVector4());
		}
		else if (type == VAR_RECT)
		{
			auto control = static_cast<RectControl*>(control_);
			control->SetValue(value.GetRect());
		}
		else if (type == VAR_COLOR)
		{
			auto control = static_cast<ColorControl*>(control_);
			control->SetValue(value.GetColor());
		}
		else if (type == VAR_INT)
		{
			auto control = static_cast<IntegerControl*>(control_);
			control->SetValue(value.GetInt());
		}
		else if (type == VAR_INTVECTOR2)
		{
			auto control = static_cast<IntVector2Control*>(control_);
			control->SetValue(value.GetIntVector2());
		}
		else if (type == VAR_INTVECTOR3)
		{
			auto control = static_cast<IntVector3Control*>(control_);
			control->SetValue(value.GetIntVector3());
		}
		else if (type == VAR_INTRECT)
		{
			auto control = static_cast<IntRectControl*>(control_);
			control->SetValue(value.GetIntRect());
		}
		else if (type == VAR_BOOL)
		{
			auto control = static_cast<CheckboxControl*>(control_);
			control->SetValue(value.GetBool());
		}
		else if (type == VAR_STRING)
		{
			auto control = static_cast<StringControl*>(control_);
			control->SetValue(value.GetString());
		}
		else if (type == VAR_STRINGVECTOR)
		{
			auto control = static_cast<StringListControl*>(control_);
			control->SetValue(value.GetStringVector());
		}
		else if (type == VAR_RESOURCEREF)
		{
			auto control = static_cast<ResourceRefControl*>(control_);
			control->SetValue(value.GetResourceRef());
		}
		else if (type == VAR_VARIANTMAP)
		{
			auto control = static_cast<VariantMapControl*>(control_);
			control->SetValue(value.GetVariantMap());
		}

		control_->SetBlockEvents(false);
	}
}

namespace Geode
{
	ReactiveForm::ReactiveForm(Context* context) : UIElement(context)
	{}

	void ReactiveForm::RegisterObject(Context* context)
	{
		context->RegisterFactory<ReactiveForm>();
		URHO3D_COPY_BASE_ATTRIBUTES(UIElement);

		AttributeField::RegisterObject(context);
	}

	AttributeField* ReactiveForm::GetAttributeField(const String& attributeName)
	{
		for (auto attrField : attrFields_)
		{
			if (attrField->GetAttributeName() == attributeName)
			{
				return attrField;
			}
		}

		return nullptr;
	}

	bool ReactiveForm::IsValid()
	{
		for (auto attrField : attrFields_)
		{
			if (attrField->GetControl() != nullptr && !attrField->GetControl()->IsValid())
			{
				return false;
			}
		}

		return true;
	}

	AttributeField* ReactiveForm::AddAttributeField(Serializable* serializable, String attributeName, String label)
	{
		auto attrField = CreateChild<AttributeField>("RF_AttributeField" + attributeName);
		attrField->SetStyleAuto();
		attrField->SetAttribute(serializable, attributeName);
		attrField->SetLabel(label);

		attrFields_.Push(attrField);

		SubscribeToEvent(attrField, E_ATTRIBUTEFIELD_DATACHANGED, URHO3D_HANDLER(ReactiveForm, HandleAttributeFieldDataChanged));

		return attrField;
	}

	void ReactiveForm::Clear()
	{
		attrFields_.Clear();
		RemoveAllChildren();
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void ReactiveForm::HandleAttributeFieldDataChanged(StringHash, VariantMap& eventData)
	{
		eventData[ReactiveFormDataChanged::P_ELEMENT] = this;
		SendEvent(E_REACTIVEFORM_DATACHANGED, eventData);
	}
}