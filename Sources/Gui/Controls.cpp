#include "Controls.h"
#include "Helper.h"

#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/UI.h>

#define URHO3D_HASH_DEBUG

using namespace Urho3D;
using namespace std;

static const int WHEEL_WAY_UP = 1;
static const int WHEEL_WAY_DOWN = -1;

static const Vector<char> INTEGER_CONTROL_ALLOWED_CHARS = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-' };
static const Vector<char> FLOAT_CONTROL_ALLOWED_CHARS = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-' };
static const Vector<String> VARIANT_CONTROL_TYPE_NAMES = {"None", "Integer", "Bool", "Float", "Vector2", "Vector3", "Vector4", "Quaternion", "Color", "String", "Buffer", "VoidPtr", "ResourceRef", "ResourceRefList", "VariantVector", "VariantMap", "IntRect", "IntVector2", "Ptr", "Matrix3", "Matrix3x4", "Matrix4", "Double", "StringVector", "Rect", "IntVector3", "Int64", "CustomHeap", "CustomStack"};
static const Vector<VariantType> VARIANT_CONTROL_DEFAULT_ALLOWED_TYPES = { VAR_FLOAT, VAR_QUATERNION, VAR_VECTOR2, VAR_VECTOR3, VAR_VECTOR4, VAR_RECT, VAR_COLOR,  VAR_INT,  VAR_INTVECTOR2, VAR_INTVECTOR3, VAR_INTRECT, VAR_BOOL, VAR_STRING, VAR_STRINGVECTOR, VAR_RESOURCEREF};

static const String VAR_SELECT_OPTION_VALUE = "VAR_SELECT_OPTION_VALUE";
static const String VAR_VARIANTMAP_ITEM_KEY = "VAR_VARIANTMAP_ITEM_KEY";

namespace Geode
{
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  ICONTROL BASE
	///////////////////////////////////////////////////////////////////////////////////////////////////
	IControlBase::IControlBase(Context* context) : UIElement(context)
	{
		SetLayoutMode(LayoutMode::LM_HORIZONTAL);
	}

	bool IControlBase::IsValid()
	{
		return true;
	}

	bool IControlBase::IsEditing()
	{
		return HasFocus() && IsEditable();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  FLOAT CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	FloatControl::FloatControl(Context* context) : IControl<float>(context),
		sensibility_(0.1f),
		min_(1),
		max_(524288)
	{
		lineEdit_ = CreateChild<LineEdit>("FC_LineEdit");
		lineEdit_->SetInternal(true);

		SubscribeToEvent(E_MOUSEWHEEL, URHO3D_HANDLER(FloatControl, HandleMouseWheel));
		SubscribeToEvent(lineEdit_, E_TEXTCHANGED, URHO3D_HANDLER(FloatControl, HandleTextChanged));
		SubscribeToEvent(lineEdit_, E_TEXTENTRY, URHO3D_HANDLER(FloatControl, HandleTextEntry));
	}

	void FloatControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<FloatControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<float>);
	}

	float FloatControl::GetSensibility()
	{
		return sensibility_;
	}

	float FloatControl::GetMin()
	{
		return min_;
	}

	float FloatControl::GetMax()
	{
		return max_;
	}

	bool FloatControl::IsEditing()
	{
		return lineEdit_->HasFocus() && IsEditable();
	}

	void FloatControl::SetValue(const float& value)
	{
		lineEdit_->SetBlockEvents(true);
		
		lineEdit_->SetText(String(value));
		lineEdit_->SetCursorPosition(0);
		IControl::SetValue(value);

		lineEdit_->SetBlockEvents(false);
	}

	void FloatControl::SetSensibility(float sensibility)
	{
		sensibility_ = sensibility;
	}

	void FloatControl::SetMin(float min)
	{
		if (min < max_)
		{
			min_ = min;
		}
	}

	void FloatControl::SetMax(float max)
	{
		if (max > min_)
		{
			max_ = max;
		}
	}

	void FloatControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		lineEdit_->SetEditable(editable);
	}

	void FloatControl::Increase()
	{
		float increaseValue = GetValue() + sensibility_;

		if (Validate(increaseValue))
		{
			SetValue(increaseValue);
		}
		else
		{
			SetValue(min_);
		}
	}

	void FloatControl::Decrease()
	{
		float decreaseValue = GetValue() - sensibility_;

		if (Validate(decreaseValue))
		{
			SetValue(decreaseValue);
		}
		else
		{
			SetValue(min_);
		}
	}

	String FloatControl::Sanitize(String text)
	{
		for (unsigned int i = 0; i < text.Length(); i++)
		{
			if (!IsValidCharacter(text[i]))
			{
				text.Erase(i);
			}
		}

		return text;
	}

	bool FloatControl::IsValidCharacter(char character)
	{
		return FLOAT_CONTROL_ALLOWED_CHARS.Find(character) != FLOAT_CONTROL_ALLOWED_CHARS.End();
	}

	bool FloatControl::Validate(const float& value)
	{
		return (value >= min_ && value <= max_);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void FloatControl::HandleMouseWheel(StringHash, VariantMap& eventData)
	{
		int wheelWay = eventData[MouseWheel::P_WHEEL].GetInt();

		if (lineEdit_->HasFocus() && IsEditable())
		{
			if (wheelWay == WHEEL_WAY_UP)
			{
				Increase();
			}
			if (wheelWay == WHEEL_WAY_DOWN)
			{
				Decrease();
			}
		}
	}

	void FloatControl::HandleTextChanged(StringHash, VariantMap&)
	{
		String text = Sanitize(lineEdit_->GetText());
		float value = Variant(VariantType::VAR_FLOAT, text).GetFloat();

		IControl::SetValue(value);
	}

	void FloatControl::HandleTextEntry(StringHash, VariantMap& eventData)
	{
		String character = eventData[TextChanged::P_TEXT].GetString();

		if (!IsValidCharacter(character[0])) // delete char if is not a valid char
		{
			eventData[TextChanged::P_TEXT] = "";
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  QUATERNION CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	QuaternionControl::QuaternionControl(Context* context) : IControl<Quaternion>(context)
	{
		angleControl_ = CreateChild<FloatControl>("QC_AngleControl");
		angleControl_->SetInternal(true);
		angleControl_->SetMin(0);
		angleControl_->SetMax(360);

		SubscribeToEvent(angleControl_, E_CONTROLCHANGED, URHO3D_HANDLER(QuaternionControl, HandleAngleControlChanged));
	}

	void QuaternionControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<QuaternionControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<Quaternion>);
	}

	bool QuaternionControl::IsEditing()
	{
		return angleControl_->IsEditing() && IsEditable();
	}

	void QuaternionControl::SetValue(const Quaternion& value)
	{
		angleControl_->SetBlockEvents(true);

		angleControl_->SetValue(value.Angle());
		IControl::SetValue(value);

		angleControl_->SetBlockEvents(false);
	}

	void QuaternionControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		angleControl_->SetEditable(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void QuaternionControl::HandleAngleControlChanged(StringHash, VariantMap&)
	{
		auto angle = angleControl_->GetValue();
		IControl::SetValue(Quaternion(angle));
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  VECTOR2 CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	Vector2Control::Vector2Control(Context* context) : IControl<Vector2>(context)
	{
		xControl_ = CreateChild<FloatControl>("VC_XControl");
		xControl_->SetInternal(true);

		yControl_ = CreateChild<FloatControl>("VC_YControl");
		yControl_->SetInternal(true);

		SubscribeToEvent(xControl_, E_CONTROLCHANGED, URHO3D_HANDLER(Vector2Control, HandleXControlChanged));
		SubscribeToEvent(yControl_, E_CONTROLCHANGED, URHO3D_HANDLER(Vector2Control, HandleYControlChanged));
	}

	void Vector2Control::RegisterObject(Context* context)
	{
		context->RegisterFactory<Vector2Control>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<Vector2>);
	}

	bool Vector2Control::IsEditing()
	{
		return (xControl_->IsEditing() || yControl_->IsEditing()) && IsEditable();
	}

	void Vector2Control::SetValue(const Vector2& value)
	{
		xControl_->SetBlockEvents(true);
		yControl_->SetBlockEvents(true);

		xControl_->SetValue(value.x_);
		yControl_->SetValue(value.y_);
		IControl::SetValue(value);

		xControl_->SetBlockEvents(false);
		yControl_->SetBlockEvents(false);
	}

	void Vector2Control::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		xControl_->SetEditable(editable);
		yControl_->SetEditable(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void Vector2Control::HandleXControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.x_ = xControl_->GetValue();

		IControl::SetValue(vec);
	}

	void Vector2Control::HandleYControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.y_ = yControl_->GetValue();

		IControl::SetValue(vec);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  VECTOR3 CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	Vector3Control::Vector3Control(Context* context) : IControl<Vector3>(context)
	{
		xControl_ = CreateChild<FloatControl>("VC_XControl");
		xControl_->SetInternal(true);

		yControl_ = CreateChild<FloatControl>("VC_YControl");
		yControl_->SetInternal(true);

		zControl_ = CreateChild<FloatControl>("VC_ZControl");
		zControl_->SetInternal(true);

		SubscribeToEvent(xControl_, E_CONTROLCHANGED, URHO3D_HANDLER(Vector3Control, HandleXControlChanged));
		SubscribeToEvent(yControl_, E_CONTROLCHANGED, URHO3D_HANDLER(Vector3Control, HandleYControlChanged));
		SubscribeToEvent(zControl_, E_CONTROLCHANGED, URHO3D_HANDLER(Vector3Control, HandleZControlChanged));
	}

	void Vector3Control::RegisterObject(Context* context)
	{
		context->RegisterFactory<Vector3Control>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<Vector3>);
	}

	bool Vector3Control::IsEditing()
	{
		return (xControl_->IsEditing() || yControl_->IsEditing() || zControl_->IsEditing()) && IsEditable();
	}

	void Vector3Control::SetValue(const Vector3& value)
	{
		xControl_->SetBlockEvents(true);
		yControl_->SetBlockEvents(true);
		zControl_->SetBlockEvents(true);

		xControl_->SetValue(value.x_);
		yControl_->SetValue(value.y_);
		zControl_->SetValue(value.z_);
		IControl::SetValue(value);

		xControl_->SetBlockEvents(false);
		yControl_->SetBlockEvents(false);
		zControl_->SetBlockEvents(false);
	}

	void Vector3Control::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		xControl_->SetEditable(editable);
		yControl_->SetEditable(editable);
		zControl_->SetEditable(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void Vector3Control::HandleXControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.x_ = xControl_->GetValue();

		IControl::SetValue(vec);
	}

	void Vector3Control::HandleYControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.y_ = yControl_->GetValue();

		IControl::SetValue(vec);
	}

	void Vector3Control::HandleZControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.z_ = zControl_->GetValue();

		IControl::SetValue(vec);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  VECTOR4 CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	Vector4Control::Vector4Control(Context* context) : IControl<Vector4>(context)
	{
		xControl_ = CreateChild<FloatControl>("VC_XControl");
		xControl_->SetInternal(true);

		yControl_ = CreateChild<FloatControl>("VC_YControl");
		yControl_->SetInternal(true);

		zControl_ = CreateChild<FloatControl>("VC_ZControl");
		zControl_->SetInternal(true);

		wControl_ = CreateChild<FloatControl>("VC_WControl");
		wControl_->SetInternal(true);

		SubscribeToEvent(xControl_, E_CONTROLCHANGED, URHO3D_HANDLER(Vector4Control, HandleXControlChanged));
		SubscribeToEvent(yControl_, E_CONTROLCHANGED, URHO3D_HANDLER(Vector4Control, HandleYControlChanged));
		SubscribeToEvent(zControl_, E_CONTROLCHANGED, URHO3D_HANDLER(Vector4Control, HandleZControlChanged));
		SubscribeToEvent(wControl_, E_CONTROLCHANGED, URHO3D_HANDLER(Vector4Control, HandleWControlChanged));
	}

	void Vector4Control::RegisterObject(Context* context)
	{
		context->RegisterFactory<Vector4Control>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<Vector4>);
	}

	bool Vector4Control::IsEditing()
	{
		return (xControl_->IsEditing() || yControl_->IsEditing() || zControl_->IsEditing() || wControl_->IsEditing()) && IsEditable();
	}

	void Vector4Control::SetValue(const Vector4& value)
	{
		xControl_->SetBlockEvents(true);
		yControl_->SetBlockEvents(true);
		zControl_->SetBlockEvents(true);
		wControl_->SetBlockEvents(true);

		xControl_->SetValue(value.x_);
		yControl_->SetValue(value.y_);
		zControl_->SetValue(value.z_);
		wControl_->SetValue(value.w_);
		IControl::SetValue(value);

		xControl_->SetBlockEvents(false);
		yControl_->SetBlockEvents(false);
		zControl_->SetBlockEvents(false);
		wControl_->SetBlockEvents(false);
	}

	void Vector4Control::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		xControl_->SetEditable(editable);
		yControl_->SetEditable(editable);
		zControl_->SetEditable(editable);
		wControl_->SetEditable(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void Vector4Control::HandleXControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.x_ = xControl_->GetValue();

		IControl::SetValue(vec);
	}

	void Vector4Control::HandleYControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.y_ = yControl_->GetValue();

		IControl::SetValue(vec);
	}

	void Vector4Control::HandleZControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.z_ = zControl_->GetValue();

		IControl::SetValue(vec);
	}

	void Vector4Control::HandleWControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.w_ = wControl_->GetValue();

		IControl::SetValue(vec);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  RECT CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	RectControl::RectControl(Context* context) : IControl<Rect>(context)
	{
		x1Control_ = CreateChild<FloatControl>("RC_X1Control");
		x1Control_->SetInternal(true);

		y1Control_ = CreateChild<FloatControl>("RC_Y1Control");
		y1Control_->SetInternal(true);

		x2Control_ = CreateChild<FloatControl>("RC_X2Control");
		x2Control_->SetInternal(true);

		y2Control_ = CreateChild<FloatControl>("RC_Y2Control");
		y2Control_->SetInternal(true);

		SubscribeToEvent(x1Control_, E_CONTROLCHANGED, URHO3D_HANDLER(RectControl, HandleX1ControlChanged));
		SubscribeToEvent(y1Control_, E_CONTROLCHANGED, URHO3D_HANDLER(RectControl, HandleY1ControlChanged));
		SubscribeToEvent(x2Control_, E_CONTROLCHANGED, URHO3D_HANDLER(RectControl, HandleX2ControlChanged));
		SubscribeToEvent(y2Control_, E_CONTROLCHANGED, URHO3D_HANDLER(RectControl, HandleY2ControlChanged));
	}

	void RectControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<RectControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<Rect>);
	}

	bool RectControl::IsEditing()
	{
		return (x1Control_->IsEditing() || y1Control_->IsEditing() || x2Control_->IsEditing() || y2Control_->IsEditing()) && IsEditable();
	}

	void RectControl::SetValue(const Rect& value)
	{
		x1Control_->SetBlockEvents(true);
		y1Control_->SetBlockEvents(true);
		x2Control_->SetBlockEvents(true);
		y2Control_->SetBlockEvents(true);

		x1Control_->SetValue(value.Left());
		y1Control_->SetValue(value.Top());
		x2Control_->SetValue(value.Right());
		y2Control_->SetValue(value.Bottom());
		IControl::SetValue(value);

		x1Control_->SetBlockEvents(false);
		y1Control_->SetBlockEvents(false);
		x2Control_->SetBlockEvents(false);
		y2Control_->SetBlockEvents(false);
	}

	void RectControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		x1Control_->SetEditable(editable);
		y1Control_->SetEditable(editable);
		x2Control_->SetEditable(editable);
		y2Control_->SetEditable(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void RectControl::HandleX1ControlChanged(StringHash, VariantMap&)
	{
		auto rect = GetValue();
		rect.min_.x_ = x1Control_->GetValue();

		IControl::SetValue(rect);
	}

	void RectControl::HandleY1ControlChanged(StringHash, VariantMap&)
	{
		auto rect = GetValue();
		rect.min_.y_ = y1Control_->GetValue();

		IControl::SetValue(rect);
	}

	void RectControl::HandleX2ControlChanged(StringHash, VariantMap&)
	{
		auto rect = GetValue();
		rect.max_.x_ = x2Control_->GetValue();

		IControl::SetValue(rect);
	}

	void RectControl::HandleY2ControlChanged(StringHash, VariantMap&)
	{
		auto rect = GetValue();
		rect.max_.y_ = y2Control_->GetValue();

		IControl::SetValue(rect);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  COLOR CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	ColorControl::ColorControl(Context* context) : IControl<Color>(context)
	{
		rControl_ = CreateChild<FloatControl>("RC_RControl");
		rControl_->SetInternal(true);
		rControl_->SetMin(0);
		rControl_->SetMax(1);

		gControl_ = CreateChild<FloatControl>("RC_GControl");
		gControl_->SetInternal(true);
		gControl_->SetMin(0);
		gControl_->SetMax(1);

		bControl_ = CreateChild<FloatControl>("RC_BControl");
		bControl_->SetInternal(true);
		bControl_->SetMin(0);
		bControl_->SetMax(1);

		aControl_ = CreateChild<FloatControl>("RC_AControl");
		aControl_->SetInternal(true);
		aControl_->SetMin(0);
		aControl_->SetMax(1);

		SubscribeToEvent(rControl_, E_CONTROLCHANGED, URHO3D_HANDLER(ColorControl, HandleRControlChanged));
		SubscribeToEvent(gControl_, E_CONTROLCHANGED, URHO3D_HANDLER(ColorControl, HandleGControlChanged));
		SubscribeToEvent(bControl_, E_CONTROLCHANGED, URHO3D_HANDLER(ColorControl, HandleBControlChanged));
		SubscribeToEvent(aControl_, E_CONTROLCHANGED, URHO3D_HANDLER(ColorControl, HandleAControlChanged));
	}

	void ColorControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<ColorControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<Rect>);
	}

	bool ColorControl::IsEditing()
	{
		return (rControl_->IsEditing() || gControl_->IsEditing() || bControl_->IsEditing() || aControl_->IsEditing()) && IsEditable();
	}

	void ColorControl::SetValue(const Color& value)
	{
		rControl_->SetBlockEvents(true);
		gControl_->SetBlockEvents(true);
		bControl_->SetBlockEvents(true);
		aControl_->SetBlockEvents(true);

		rControl_->SetValue(value.r_);
		gControl_->SetValue(value.g_);
		bControl_->SetValue(value.b_);
		aControl_->SetValue(value.a_);
		IControl::SetValue(value);

		rControl_->SetBlockEvents(false);
		gControl_->SetBlockEvents(false);
		bControl_->SetBlockEvents(false);
		aControl_->SetBlockEvents(false);
	}

	void ColorControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		rControl_->SetEditable(editable);
		gControl_->SetEditable(editable);
		bControl_->SetEditable(editable);
		aControl_->SetEditable(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void ColorControl::HandleRControlChanged(StringHash, VariantMap&)
	{
		auto color = GetValue();
		color.r_ = rControl_->GetValue();

		IControl::SetValue(color);
	}

	void ColorControl::HandleGControlChanged(StringHash, VariantMap&)
	{
		auto color = GetValue();
		color.g_ = gControl_->GetValue();

		IControl::SetValue(color);
	}

	void ColorControl::HandleBControlChanged(StringHash, VariantMap&)
	{
		auto color = GetValue();
		color.b_ = bControl_->GetValue();

		IControl::SetValue(color);
	}

	void ColorControl::HandleAControlChanged(StringHash, VariantMap&)
	{
		auto color = GetValue();
		color.a_ = aControl_->GetValue();

		IControl::SetValue(color);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  INTEGER CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	IntegerControl::IntegerControl(Context* context) : IControl<int>(context),
		sensibility_(1),
		min_(1),
		max_(524288)
	{
		lineEdit_ = CreateChild<LineEdit>("IC_LineEdit");
		lineEdit_->SetInternal(true);

		SubscribeToEvent(E_MOUSEWHEEL, URHO3D_HANDLER(IntegerControl, HandleMouseWheel));
		SubscribeToEvent(lineEdit_, E_TEXTCHANGED, URHO3D_HANDLER(IntegerControl, HandleTextChanged));
		SubscribeToEvent(lineEdit_, E_TEXTENTRY, URHO3D_HANDLER(IntegerControl, HandleTextEntry));
	}

	void IntegerControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<IntegerControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<int>);
	}

	int IntegerControl::GetSensibility()
	{
		return sensibility_;
	}

	int IntegerControl::GetMin()
	{
		return min_;
	}

	int IntegerControl::GetMax()
	{
		return max_;
	}

	bool IntegerControl::IsEditing()
	{
		return lineEdit_->HasFocus() && IsEditable();
	}

	void IntegerControl::SetValue(const int& value)
	{
		lineEdit_->SetBlockEvents(true);

		lineEdit_->SetText(String(value));
		lineEdit_->SetCursorPosition(0);
		IControl::SetValue(value);

		lineEdit_->SetBlockEvents(false);
	}

	void IntegerControl::SetSensibility(int sensibility)
	{
		sensibility_ = sensibility;
	}

	void IntegerControl::SetMin(int min)
	{
		if (min < max_)
		{
			min_ = min;
		}
	}

	void IntegerControl::SetMax(int max)
	{
		if (max > min_)
		{
			max_ = max;
		}
	}

	void IntegerControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		lineEdit_->SetEditable(editable);
	}

	void IntegerControl::Increase()
	{
		int increaseValue = GetValue() + sensibility_;

		if (Validate(increaseValue))
		{
			SetValue(increaseValue);
		}
		else
		{
			SetValue(min_);
		}
	}

	void IntegerControl::Decrease()
	{
		int decreaseValue = GetValue() - sensibility_;

		if (Validate(decreaseValue))
		{
			SetValue(decreaseValue);
		}
		else
		{
			SetValue(min_);
		}
	}

	String IntegerControl::Sanitize(String text)
	{
		for (unsigned int i = 0; i < text.Length(); i++)
		{
			if (!IsValidCharacter(text[i]))
			{
				text.Erase(i);
			}
		}

		return text;
	}

	bool IntegerControl::IsValidCharacter(char character)
	{
		return INTEGER_CONTROL_ALLOWED_CHARS.Find(character) != INTEGER_CONTROL_ALLOWED_CHARS.End();
	}

	bool IntegerControl::Validate(const int& value)
	{
		return (value >= min_ && value <= max_);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void IntegerControl::HandleMouseWheel(StringHash, VariantMap& eventData)
	{
		int wheelWay = eventData[MouseWheel::P_WHEEL].GetInt();

		if (lineEdit_->HasFocus() && IsEditable())
		{
			if (wheelWay == WHEEL_WAY_UP)
			{
				Increase();
			}
			if (wheelWay == WHEEL_WAY_DOWN)
			{
				Decrease();
			}
		}
	}

	void IntegerControl::HandleTextChanged(StringHash, VariantMap&)
	{
		String text = Sanitize(lineEdit_->GetText());
		int value = Variant(VariantType::VAR_INT, text).GetInt();

		IControl::SetValue(value);
	}

	void IntegerControl::HandleTextEntry(StringHash, VariantMap& eventData)
	{
		String character = eventData[TextChanged::P_TEXT].GetString();

		if (!IsValidCharacter(character[0]))
		{
			eventData[TextChanged::P_TEXT] = "";
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  INTVECTOR2 CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	IntVector2Control::IntVector2Control(Context* context) : IControl<IntVector2>(context)
	{
		xControl_ = CreateChild<IntegerControl>("VC_XControl");
		xControl_->SetInternal(true);

		yControl_ = CreateChild<IntegerControl>("VC_YControl");
		yControl_->SetInternal(true);

		SubscribeToEvent(xControl_, E_CONTROLCHANGED, URHO3D_HANDLER(IntVector2Control, HandleXControlChanged));
		SubscribeToEvent(yControl_, E_CONTROLCHANGED, URHO3D_HANDLER(IntVector2Control, HandleYControlChanged));
	}

	void IntVector2Control::RegisterObject(Context* context)
	{
		context->RegisterFactory<IntVector2Control>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<IntVector2>);
	}

	bool IntVector2Control::IsEditing()
	{
		return (xControl_->IsEditing() || yControl_->IsEditing()) && IsEditable();
	}

	void IntVector2Control::SetValue(const IntVector2& value)
	{
		xControl_->SetBlockEvents(true);
		yControl_->SetBlockEvents(true);

		xControl_->SetValue(value.x_);
		yControl_->SetValue(value.y_);
		IControl::SetValue(value);

		xControl_->SetBlockEvents(false);
		yControl_->SetBlockEvents(false);
	}

	void IntVector2Control::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		xControl_->SetEditable(editable);
		yControl_->SetEditable(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void IntVector2Control::HandleXControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.x_ = xControl_->GetValue();

		IControl::SetValue(vec);
	}

	void IntVector2Control::HandleYControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.y_ = yControl_->GetValue();

		IControl::SetValue(vec);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  INTVECTOR3 CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	IntVector3Control::IntVector3Control(Context* context) : IControl<IntVector3>(context)
	{
		xControl_ = CreateChild<IntegerControl>("VC_XControl");
		xControl_->SetInternal(true);

		yControl_ = CreateChild<IntegerControl>("VC_YControl");
		yControl_->SetInternal(true);

		zControl_ = CreateChild<IntegerControl>("VC_ZControl");
		zControl_->SetInternal(true);

		SubscribeToEvent(xControl_, E_CONTROLCHANGED, URHO3D_HANDLER(IntVector3Control, HandleXControlChanged));
		SubscribeToEvent(yControl_, E_CONTROLCHANGED, URHO3D_HANDLER(IntVector3Control, HandleYControlChanged));
		SubscribeToEvent(zControl_, E_CONTROLCHANGED, URHO3D_HANDLER(IntVector3Control, HandleZControlChanged));
	}

	void IntVector3Control::RegisterObject(Context* context)
	{
		context->RegisterFactory<IntVector3Control>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<Vector3>);
	}

	bool IntVector3Control::IsEditing()
	{
		return (xControl_->IsEditing() || yControl_->IsEditing() || zControl_->IsEditing()) && IsEditable();
	}

	void IntVector3Control::SetValue(const IntVector3& value)
	{
		xControl_->SetBlockEvents(true);
		yControl_->SetBlockEvents(true);
		zControl_->SetBlockEvents(true);

		xControl_->SetValue(value.x_);
		yControl_->SetValue(value.y_);
		zControl_->SetValue(value.z_);
		IControl::SetValue(value);

		xControl_->SetBlockEvents(false);
		yControl_->SetBlockEvents(false);
		zControl_->SetBlockEvents(false);
	}

	void IntVector3Control::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		xControl_->SetEditable(editable);
		yControl_->SetEditable(editable);
		zControl_->SetEditable(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void IntVector3Control::HandleXControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.x_ = xControl_->GetValue();

		IControl::SetValue(vec);
	}

	void IntVector3Control::HandleYControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.y_ = yControl_->GetValue();

		IControl::SetValue(vec);
	}

	void IntVector3Control::HandleZControlChanged(StringHash, VariantMap&)
	{
		auto vec = GetValue();
		vec.z_ = zControl_->GetValue();

		IControl::SetValue(vec);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  INTRECT CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	IntRectControl::IntRectControl(Context* context) : IControl<IntRect>(context)
	{
		leftControl_ = CreateChild<IntegerControl>("RC_LeftControl");
		leftControl_->SetInternal(true);

		topControl_ = CreateChild<IntegerControl>("RC_TopControl");
		topControl_->SetInternal(true);

		rightControl_ = CreateChild<IntegerControl>("RC_RightControl");
		rightControl_->SetInternal(true);

		bottomControl_ = CreateChild<IntegerControl>("RC_BottomControl");
		bottomControl_->SetInternal(true);

		SubscribeToEvent(leftControl_, E_CONTROLCHANGED, URHO3D_HANDLER(IntRectControl, HandleLeftControlChanged));
		SubscribeToEvent(topControl_, E_CONTROLCHANGED, URHO3D_HANDLER(IntRectControl, HandleTopControlChanged));
		SubscribeToEvent(rightControl_, E_CONTROLCHANGED, URHO3D_HANDLER(IntRectControl, HandleRightControlChanged));
		SubscribeToEvent(bottomControl_, E_CONTROLCHANGED, URHO3D_HANDLER(IntRectControl, HandleBottomControlChanged));
	}

	void IntRectControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<IntRectControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<Rect>);
	}

	bool IntRectControl::IsEditing()
	{
		return (leftControl_->IsEditing() || topControl_->IsEditing() || rightControl_->IsEditing() || bottomControl_->IsEditing()) && IsEditable();
	}

	void IntRectControl::SetValue(const IntRect& value)
	{
		leftControl_->SetBlockEvents(true);
		topControl_->SetBlockEvents(true);
		rightControl_->SetBlockEvents(true);
		bottomControl_->SetBlockEvents(true);

		leftControl_->SetValue(value.Left());
		topControl_->SetValue(value.Top());
		rightControl_->SetValue(value.Right());
		bottomControl_->SetValue(value.Bottom());
		IControl::SetValue(value);

		leftControl_->SetBlockEvents(false);
		topControl_->SetBlockEvents(false);
		rightControl_->SetBlockEvents(false);
		bottomControl_->SetBlockEvents(false);
	}

	void IntRectControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		leftControl_->SetEditable(editable);
		topControl_->SetEditable(editable);
		rightControl_->SetEditable(editable);
		bottomControl_->SetEditable(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void IntRectControl::HandleLeftControlChanged(StringHash, VariantMap&)
	{
		auto rect = GetValue();
		rect.left_ = leftControl_->GetValue();

		IControl::SetValue(rect);
	}

	void IntRectControl::HandleTopControlChanged(StringHash, VariantMap&)
	{
		auto rect = GetValue();
		rect.top_ = topControl_->GetValue();

		IControl::SetValue(rect);
	}

	void IntRectControl::HandleRightControlChanged(StringHash, VariantMap&)
	{
		auto rect = GetValue();
		rect.right_ = rightControl_->GetValue();

		IControl::SetValue(rect);
	}

	void IntRectControl::HandleBottomControlChanged(StringHash, VariantMap&)
	{
		auto rect = GetValue();
		rect.bottom_ = bottomControl_->GetValue();

		IControl::SetValue(rect);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CHECKBOX CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	CheckboxControl::CheckboxControl(Context* context) : IControl<bool>(context)
	{
		SetEnabled(true);

		checkBox_ = CreateChild<CheckBox>("BC_CheckBox");
		checkBox_->SetInternal(true);

		label_ = CreateChild<Text>("BC_Label");
		label_->SetInternal(true);

		SubscribeToEvent(checkBox_, E_TOGGLED, URHO3D_HANDLER(CheckboxControl, HandleCheckBoxToggled));
	}

	void CheckboxControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<CheckboxControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<bool>);

		URHO3D_ACCESSOR_ATTRIBUTE("Label", GetLabel, SetLabel, String, String::EMPTY, AM_FILE);
	}

	const String& CheckboxControl::GetLabel() const
	{
		return label_->GetText();
	}

	bool CheckboxControl::IsEditing()
	{
		return checkBox_->HasFocus() && IsEditable();
	}

	void CheckboxControl::SetValue(const bool& value)
	{
		checkBox_->SetBlockEvents(true);

		checkBox_->SetChecked(value);
		IControl::SetValue(value);

		checkBox_->SetBlockEvents(false);
	}

	void CheckboxControl::SetLabel(const String& label)
	{
		label_->SetText(label);
	}

	void CheckboxControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		checkBox_->SetEditable(editable);
	}

	void CheckboxControl::OnClickBegin(const IntVector2& position, const IntVector2& screenPosition, int button, int buttons, int qualifiers, Cursor* cursor)
	{
		if (label_->IsInside(screenPosition, true) && button == MOUSEB_LEFT)
		{
			SetValue(!GetValue());
		}
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void CheckboxControl::HandleCheckBoxToggled(StringHash, VariantMap& eventData)
	{
		bool value = eventData[Toggled::P_STATE].GetBool();
		IControl::SetValue(value);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  RADIO CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	RadioControl::RadioControl(Context* context) : IControl<bool>(context)
	{
		SetEnabled(true);

		radio_ = CreateChild<Radio>("RC_Radio");
		radio_->SetInternal(true);

		label_ = CreateChild<Text>("RC_Label");
		label_->SetInternal(true);

		SubscribeToEvent(radio_, E_TOGGLED, URHO3D_HANDLER(RadioControl, HandleRadioToggled));
	}

	void RadioControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<RadioControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<bool>);

		URHO3D_ACCESSOR_ATTRIBUTE("Group Name", GetGroupName, SetGroupName, String, String::EMPTY, AM_FILE);
		URHO3D_ACCESSOR_ATTRIBUTE("Label", GetLabel, SetLabel, String, String::EMPTY, AM_FILE);
	}

	const String& RadioControl::GetGroupName() const
	{
		return radio_->GetGroupName();
	}

	const String& RadioControl::GetLabel() const
	{
		return label_->GetText();
	}

	bool RadioControl::IsEditing()
	{
		return radio_->HasFocus() && IsEditable();
	}

	void RadioControl::SetGroupName(const String& groupName)
	{
		radio_->SetGroupName(groupName);
	}

	void RadioControl::SetValue(const bool& value)
	{
		radio_->SetBlockEvents(true);

		radio_->SetChecked(value);
		IControl::SetValue(value);

		radio_->SetBlockEvents(false);
	}

	void RadioControl::SetLabel(const String& label)
	{
		label_->SetText(label);
	}

	void RadioControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		radio_->SetEditable(editable);
	}

	void RadioControl::OnClickBegin(const IntVector2& position, const IntVector2& screenPosition, int button, int buttons, int qualifiers, Cursor* cursor)
	{
		if (label_->IsInside(screenPosition, true) && button == MOUSEB_LEFT)
		{
			SetValue(true);
		}
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void RadioControl::HandleRadioToggled(StringHash, VariantMap& eventData)
	{
		bool value = eventData[Toggled::P_STATE].GetBool();
		IControl::SetValue(value);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  STRING CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	StringControl::StringControl(Context* context) : IControl(context)
	{
		lineEdit_ = CreateChild<LineEdit>("SC_LineEdit");
		lineEdit_->SetInternal(true);

		SubscribeToEvent(lineEdit_, E_TEXTCHANGED, URHO3D_HANDLER(StringControl, HandleTextChanged));
	}

	void StringControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<StringControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl);
	}

	bool StringControl::IsEditing()
	{
		return lineEdit_->HasFocus() && IsEditable();
	}

	void StringControl::SetValue(const String& value)
	{
		lineEdit_->SetBlockEvents(true);

		lineEdit_->SetText(value);
		lineEdit_->SetCursorPosition(0);
		IControl::SetValue(value);

		lineEdit_->SetBlockEvents(false);
	}

	void StringControl::SetPattern(const String& pattern)
	{
		reg_.assign(pattern.CString());
	}

	void StringControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		lineEdit_->SetEditable(editable);
	}

	bool StringControl::Validate(const String& value)
	{
		return regex_match(value.CString(), reg_);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void StringControl::HandleTextChanged(StringHash, VariantMap& eventData)
	{
		IControl::SetValue(lineEdit_->GetText());
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  STRING LIST CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	StringListControl::StringListControl(Context* context) : IControl<StringVector>(context)
	{
		lineEdit_ = CreateChild<LineEdit>("SLC_LineEdit");
		lineEdit_->SetInternal(true);

		SubscribeToEvent(lineEdit_, E_TEXTCHANGED, URHO3D_HANDLER(StringListControl, HandleTextChanged));
	}

	void StringListControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<StringListControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<StringVector>);
	}

	bool StringListControl::IsEditing()
	{
		return lineEdit_->HasFocus() && IsEditable();
	}

	void StringListControl::SetValue(const StringVector& value)
	{
		lineEdit_->SetBlockEvents(true);

		lineEdit_->SetText(Join(value, ','));
		lineEdit_->SetCursorPosition(0);
		IControl::SetValue(value);

		lineEdit_->SetBlockEvents(false);
	}

	void StringListControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		lineEdit_->SetEditable(editable);
	}

	String StringListControl::Join(const StringVector& v, char separator)
	{
		String res;
		unsigned int length = v.Size();

		for (unsigned int i = 0; i < length; i++)
		{
			res += v[i];

			if (i < length - 1)
			{
				res += separator;
			}
		}

		return res;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void StringListControl::HandleTextChanged(StringHash, VariantMap& eventData)
	{
		StringVector words = lineEdit_->GetText().Split(',');

		for (String& word : words)
		{
			word = word.Trimmed();
		}

		IControl::SetValue(words);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  FILE CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	FileControl::FileControl(Context* context) : IControl<String>(context)
	{
		lineEdit_ = CreateChild<LineEdit>("FC_LineEdit");
		lineEdit_->SetInternal(true);
		lineEdit_->SetEditable(false);

		selectBtn_ = CreateChild<Button>("FC_SelectButton");
		selectBtn_->SetInternal(true);

		SubscribeToEvent(selectBtn_, E_RELEASED, URHO3D_HANDLER(FileControl, HandleSelectButtonReleased));
		SubscribeToEvent(lineEdit_, E_TEXTCHANGED, URHO3D_HANDLER(FileControl, HandleTextChanged));
	}

	void FileControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<FileControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<String>);
	}

	bool FileControl::IsEditing()
	{
		return lineEdit_->HasFocus() && IsEditable();
	}

	void FileControl::SetValue(const String& value)
	{
		lineEdit_->SetBlockEvents(true);

		lineEdit_->SetText(value);
		lineEdit_->SetCursorPosition(0);
		IControl::SetValue(value);

		lineEdit_->SetBlockEvents(false);
	}

	void FileControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		lineEdit_->SetEditable(editable);
		selectBtn_->SetEnabled(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void FileControl::HandleTextChanged(StringHash, VariantMap&)
	{
		IControl::SetValue(lineEdit_->GetText());
	}

	void FileControl::HandleSelectButtonReleased(StringHash, VariantMap&)
	{
		if (fileSelector_.NotNull())
		{
			fileSelector_.Reset();
		}

		fileSelector_ = MakeShared<FileSelector>(context_);
		fileSelector_->SetDefaultStyle(GetDefaultStyle());
		fileSelector_->SetTitle("Select File");
		fileSelector_->SetButtonTexts("SELECT", "CANCEL");

		SubscribeToEvent(fileSelector_, E_FILESELECTED, URHO3D_HANDLER(FileControl, HandleFileSelected));
	}

	void FileControl::HandleFileSelected(StringHash, VariantMap& eventData)
	{
		using namespace FileSelected;

		String filename = eventData[P_FILENAME].GetString();
		bool ok = eventData[P_OK].GetBool();

		if (ok)
		{
			SetValue(filename);
		}

		UnsubscribeFromEvents(fileSelector_);
		fileSelector_.Reset();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  RESSOURCE REF CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	ResourceRefControl::ResourceRefControl(Context* context) : IControl<ResourceRef>(context), selectBtn_(nullptr)
	{
		lineEdit_ = CreateChild<LineEdit>("RRC_LineEdit");
		lineEdit_->SetInternal(true);
		lineEdit_->SetEditable(false);

		selectBtn_ = CreateChild<Button>("RRC_SelectBtn");
		selectBtn_->SetInternal(true);

		SubscribeToEvent(selectBtn_, E_RELEASED, URHO3D_HANDLER(ResourceRefControl, HandleSelectButtonReleased));
		SubscribeToEvent(lineEdit_, E_TEXTCHANGED, URHO3D_HANDLER(ResourceRefControl, HandleTextChanged));
	}

	void ResourceRefControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<ResourceRefControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl);
	}

	bool ResourceRefControl::IsEditing()
	{
		return lineEdit_->HasFocus() && IsEditable();
	}

	void ResourceRefControl::SetValue(const ResourceRef& value)
	{
		lineEdit_->SetBlockEvents(true);

		lineEdit_->SetText(value.name_);
		lineEdit_->SetCursorPosition(0);
		IControl::SetValue(value);

		lineEdit_->SetBlockEvents(false);
	}

	void ResourceRefControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		lineEdit_->SetEditable(editable);
		selectBtn_->SetEnabled(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void ResourceRefControl::HandleTextChanged(StringHash, VariantMap&)
	{
		ResourceRef value = GetValue();
		value.name_ = lineEdit_->GetText();
		IControl::SetValue(value);
	}

	void ResourceRefControl::HandleSelectButtonReleased(StringHash, VariantMap&)
	{
		if (fileSelector_.NotNull())
		{
			fileSelector_.Reset();
		}

		fileSelector_ = MakeShared<FileSelector>(context_);
		fileSelector_->SetDefaultStyle(GetDefaultStyle());
		fileSelector_->SetTitle("Select Resource");
		fileSelector_->SetButtonTexts("SELECT", "CANCEL");

		SubscribeToEvent(fileSelector_, E_FILESELECTED, URHO3D_HANDLER(ResourceRefControl, HandleFileSelected));
	}

	void ResourceRefControl::HandleFileSelected(StringHash, VariantMap& eventData)
	{
		using namespace FileSelected;
		String filename = eventData[P_FILENAME].GetString();
		bool ok = eventData[P_OK].GetBool();

		if (ok)
		{
			ResourceRef value = GetValue();
			value.name_ = filename;
			SetValue(value);
		}

		UnsubscribeFromEvents(fileSelector_);
		fileSelector_.Reset();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  SELECT CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	SelectControl::SelectControl(Context* context) : IControl<int>(context)
	{
		dropDownList_ = CreateChild<DropDownList>("SC_DropDownList");
		dropDownList_->SetInternal(true);

		SubscribeToEvent(dropDownList_, E_ITEMSELECTED, URHO3D_HANDLER(SelectControl, HandleItemSelected));
	}

	SelectControl::~SelectControl()
	{
		UnsubscribeFromEvent(dropDownList_, E_ITEMSELECTED);
	}

	void SelectControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<SelectControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<int>);
	}

	void SelectControl::AddOption(const String& text, int value)
	{
		auto uiRoot = GetSubsystem<UI>()->GetRoot();
		auto option = new Text(context_);
		option->SetDefaultStyle(uiRoot->GetDefaultStyle());
		option->SetStyleAuto();
		option->SetText(text);
		option->SetVar(VAR_SELECT_OPTION_VALUE, value);

		dropDownList_->AddItem(option);
	}

	int SelectControl::IndexOf(int value)
	{
		auto items = dropDownList_->GetItems();

		for (int index = 0; index < items.Size(); index++)
		{
			if (value == items[index]->GetVar(VAR_SELECT_OPTION_VALUE).GetInt())
			{
				return index;
			}
		}

		return -1;
	}

	bool SelectControl::IsEditing()
	{
		return dropDownList_->HasFocus() && IsEditable();
	}

	void SelectControl::SetValue(const int& value)
	{
		int index = IndexOf(value);

		if (index != -1)
		{
			IControl::SetValue(value);
			dropDownList_->SetSelection(index);
		}
	}

	void SelectControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		dropDownList_->SetEditable(editable);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void SelectControl::HandleItemSelected(StringHash, VariantMap& eventData)
	{
		auto index = eventData[ItemSelected::P_SELECTION].GetInt();
		auto item = dropDownList_->GetItem(index);

		if (item != nullptr)
		{
			auto value = item->GetVar(VAR_SELECT_OPTION_VALUE).GetInt();
			IControl::SetValue(value);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  VARIANT CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	VariantControl::VariantControl(Context* context) : IControl<Variant>(context)
	{
		internalControl_ = CreateChild<StringControl>("VC_Control");
		internalControl_->SetStyleAuto();

		typeSelector_ = CreateChild<SelectControl>("VC_TypeSelector");
		typeSelector_->SetInternal(true);

		SubscribeToEvent(typeSelector_, E_CONTROLCHANGED, URHO3D_HANDLER(VariantControl, HandleTypeSelectorValueChanged));
	}

	void VariantControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<VariantControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<Variant>);
	}

	bool VariantControl::IsEditing()
	{
		return internalControl_ != nullptr && internalControl_->IsEditing();
	}

	void VariantControl::SetValue(const Variant& value)
	{
		typeSelector_->SetBlockEvents(true);

		typeSelector_->SetValue(value.GetType());
		SetInternalControlValue(value);		
		IControl::SetValue(value);

		typeSelector_->SetBlockEvents(false);
	}

	void VariantControl::SetEditable(bool editable)
	{
		UIElement::SetEditable(editable);
		internalControl_->SetEditable(editable);
	}

	void VariantControl::SetAllowedTypes(Vector<VariantType> types, VariantType selectedType)
	{
		for (auto key : types)
		{
			typeSelector_->AddOption(VARIANT_CONTROL_TYPE_NAMES[key], key);
		}

		typeSelector_->SetValue(selectedType);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void VariantControl::SetInternalControlValue(Variant value)
	{
		auto type = value.GetType();

		if (internalControl_ != nullptr)
		{
			UnsubscribeFromEvent(internalControl_, E_CONTROLCHANGED);
			internalControl_->Remove();
			internalControl_ = nullptr;
		}

		if (type == VAR_FLOAT)
		{
			auto control = CreateChild<FloatControl>("VC_Control", 0);
			control->SetValue(value.GetFloat());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_QUATERNION)
		{
			auto control = CreateChild<QuaternionControl>("VC_Control", 0);
			control->SetValue(value.GetQuaternion());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_VECTOR2)
		{
			auto control = CreateChild<Vector2Control>("VC_Control", 0);
			control->SetValue(value.GetVector2());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_VECTOR3)
		{
			auto control = CreateChild<Vector3Control>("VC_Control", 0);
			control->SetValue(value.GetVector3());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_VECTOR4)
		{
			auto control = CreateChild<Vector4Control>("VC_Control", 0);
			control->SetValue(value.GetVector4());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_RECT)
		{
			auto control = CreateChild<RectControl>("VC_Control", 0);
			control->SetValue(value.GetRect());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_COLOR)
		{
			auto control = CreateChild<ColorControl>("VC_Control", 0);
			control->SetValue(value.GetColor());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_INT)
		{
			auto control = CreateChild<IntegerControl>("VC_Control", 0);
			control->SetValue(value.GetInt());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_INTVECTOR2)
		{
			auto control = CreateChild<IntVector2Control>("VC_Control", 0);
			control->SetValue(value.GetIntVector2());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_INTVECTOR3)
		{
			auto control = CreateChild<IntVector3Control>("VC_Control", 0);
			control->SetValue(value.GetIntVector3());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_INTRECT)
		{
			auto control = CreateChild<IntRectControl>("VC_Control", 0);
			control->SetValue(value.GetIntRect());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_BOOL)
		{
			auto control = CreateChild<CheckboxControl>("VC_Control", 0);
			control->SetValue(value.GetBool());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_STRING)
		{
			auto control = CreateChild<StringControl>("VC_Control", 0);
			control->SetValue(value.GetString());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_STRINGVECTOR)
		{
			auto control = CreateChild<StringListControl>("VC_Control", 0);
			control->SetValue(value.GetStringVector());
			control->SetStyleAuto();
			internalControl_ = control;
		}
		else if (type == VAR_RESOURCEREF)
		{
			auto control = CreateChild<ResourceRefControl>("VC_Control", 0);
			control->SetValue(value.GetResourceRef());
			control->SetStyleAuto();
			internalControl_ = control;
		}

		if (internalControl_ != nullptr)
		{
			SubscribeToEvent(internalControl_, E_CONTROLCHANGED, URHO3D_HANDLER(VariantControl, HandleInternalControlValueChanged));
		}
	}

	void VariantControl::HandleInternalControlValueChanged(StringHash, VariantMap& eventData)
	{
		auto control = static_cast<UIElement*>(eventData[ControlChanged::P_ELEMENT].GetPtr());
		auto variant = eventData[ControlChanged::P_VALUE];

		IControl::SetValue(variant);
	}

	void VariantControl::HandleTypeSelectorValueChanged(StringHash, VariantMap& eventData)
	{
		auto typeSelector = static_cast<SelectControl*>(eventData[ControlChanged::P_ELEMENT].GetPtr());
		auto selectedType = typeSelector->GetValue();
		auto value = GetValue();

		if (selectedType == value.GetType())
		{
			return;
		}

		auto newValue = Variant();

		if (selectedType == VAR_STRINGVECTOR)
		{
			newValue = StringVector();
		}
		else if (selectedType == VAR_RESOURCEREF)
		{
			newValue = ResourceRef();
		}
		else
		{
			newValue = Variant(VariantType(selectedType), String::EMPTY);
		}

		SetInternalControlValue(newValue);
		IControl::SetValue(newValue);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  VARIANT MAP CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	VariantMapControl::VariantMapControl(Context* context) : IControl<VariantMap>(context)
	{
		auto createBlock = CreateChild<UIElement>();
		createBlock->SetInternal(true);

		keyNameControl_ = createBlock->CreateChild<StringControl>("VMC_KeyInput");
		keyNameControl_->SetInternal(true);

		newBtn_ = createBlock->CreateChild<Button>("VMC_NewBtn");
		newBtn_->SetInternal(true);
		
		items_ = CreateChild<UIElement>("VMC_Items");
		items_->SetInternal(true);

		SubscribeToEvent(newBtn_, E_RELEASED, URHO3D_HANDLER(VariantMapControl, HandleNewButtonReleased));
	}

	void VariantMapControl::RegisterObject(Context* context)
	{
		context->RegisterFactory<VariantMapControl>();
		URHO3D_COPY_BASE_ATTRIBUTES(IControl<VariantMap>);
	}

	bool VariantMapControl::IsEditing()
	{
		return true;
	}

	void VariantMapControl::SetValue(const VariantMap& value)
	{
		auto oldValue = GetValue();

		for (auto key : oldValue.Keys())
		{
			if ((!value.Contains(key)) || (value.Contains(key) && value.Find(key) != oldValue.Find(key)))
			{
				auto foundedItem = FindItem(key);
				if (foundedItem != nullptr)
				{
					items_->RemoveChild(foundedItem);
				}
			}
		}

		for (auto key : value.Keys())
		{
			auto variant = value.Find(key)->second_;
			if (oldValue.Contains(key) && value.Find(key) == oldValue.Find(key))
			{
				continue;
			}

			AddItem(key.Reverse(), variant);
		}

		IControl::SetValue(value);
	}

	UIElement* VariantMapControl::FindItem(StringHash key)
	{
		for (auto item : items_->GetChildren())
		{
			if (item->GetVar(VAR_VARIANTMAP_ITEM_KEY).GetStringHash() == key)
			{
				return item;
			}
		}

		return nullptr;
	}

	void VariantMapControl::AddItem(String keyName, Variant variant)
	{
		auto key = StringHash(keyName);

		auto item = items_->CreateChild<UIElement>("VMC_Item");
		item->SetStyle("VariantMapControlItem");
		item->SetVar(VAR_VARIANTMAP_ITEM_KEY, key);

		auto top = item->CreateChild<UIElement>("VMC_ItemTop");
		top->SetStyle("VariantMapControlItemTop");

		auto name = top->CreateChild<Text>("VMC_ItemTopName");
		name->SetStyle("VariantMapControlItemTopName");
		name->SetText(keyName);

		auto deleteButton = top->CreateChild<Button>("VMC_ItemTopDeleteButton");
		deleteButton->SetStyle("VariantMapControlItemTopDeleteButton");
		deleteButton->SetVar(VAR_VARIANTMAP_ITEM_KEY, key);

		auto label = deleteButton->CreateChild<Text>("Label");
		label->SetStyle("ClassicButtonLabel");
		label->SetText("Delete");

		auto variantControl = item->CreateChild<VariantControl>("VMC_ItemVariantControl");
		variantControl->SetStyleAuto();
		variantControl->SetAllowedTypes(VARIANT_CONTROL_DEFAULT_ALLOWED_TYPES, VAR_STRING);
		variantControl->SetValue(variant);
		variantControl->SetVar(VAR_VARIANTMAP_ITEM_KEY, key);

		SubscribeToEvent(deleteButton, E_RELEASED, URHO3D_HANDLER(VariantMapControl, HandleItemDeleteButtonReleased));
		SubscribeToEvent(variantControl, E_CONTROLCHANGED, URHO3D_HANDLER(VariantMapControl, HandleVariantControlValueChanged));
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void VariantMapControl::HandleVariantControlValueChanged(StringHash, VariantMap& eventData)
	{
		auto control = static_cast<UIElement*>(eventData[ControlChanged::P_ELEMENT].GetPtr());
		auto variant = eventData[ControlChanged::P_VALUE];
		auto key = control->GetVar(VAR_VARIANTMAP_ITEM_KEY).GetStringHash();

		auto value = GetValue();
		value.Insert(Pair<StringHash, Variant>(key, variant));
		IControl::SetValue(value);
	}

	void VariantMapControl::HandleNewButtonReleased(StringHash, VariantMap& eventData)
	{
		auto value = GetValue();
		auto keyName = keyNameControl_->GetValue();
		
		if (keyName != String::EMPTY && !value.Contains(keyName))
		{
			keyNameControl_->SetValue(String::EMPTY);

			auto value = GetValue();
			auto variant = Variant(VAR_STRING, String::EMPTY);
			value.Insert(Pair<StringHash, Variant>(keyName, variant));
			IControl::SetValue(value);

			AddItem(keyName, variant);
		}
	}

	void VariantMapControl::HandleItemDeleteButtonReleased(StringHash, VariantMap& eventData)
	{
		auto deleteButton = static_cast<UIElement*>(eventData[Released::P_ELEMENT].GetPtr());
		auto key = deleteButton->GetVar(VAR_VARIANTMAP_ITEM_KEY).GetStringHash();
		auto item = FindItem(key);

		auto value = GetValue();
		value.Erase(key);
		IControl::SetValue(value);

		items_->RemoveChild(item);
	}
}
