/**
 * @file    Controls.h
 * @ingroup Gui
 * @brief   Contains all controls class.
 *				- FloatControl
 *				- QuaternionControl
 *				- Vector2Control
 *				- Vector3Control
 *				- Vector4Control
 *				- RectControl
 *				- ColorControl
 *				- IntegerControl
 *				- IntVector2Control
 *				- IntVector3Control
 *				- IntRectControl
 *				- CheckboxControl
 *				- RadioControl
 *				- StringControl
 *				- StringListControl
 *				- FileControl
 *				- ResourceRefControl
 *				- SelectControl
 *				- VariantControl
 *				- VariantMapControl
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "GuiEvents.h"
#include "Radio.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Math/Quaternion.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/FileSelector.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/DropDownList.h>
#include <regex>

namespace Geode
{
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  ICONTROL BASE
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class IControlBase : public Urho3D::UIElement
	{
	public:
		/// Construct.
		IControlBase(Urho3D::Context* context);

		/// Is valid.
		virtual bool IsValid();
		/// Is editing.
		virtual bool IsEditing();
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  ICONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class IControl : public IControlBase
	{
	public:
		/// Construct.
		IControl(Urho3D::Context* context) : IControlBase(context)
		{}

		/// Get value.
		T GetValue()
		{
			return value_;
		}

		/// Is valid.
		bool IsValid() override
		{
			return Validate(value_);
		}

		/// Set value.
		virtual void SetValue(const T& value)
		{
			auto oldValue = value_;
			value_ = value;

			VariantMap sendEventData;
			using namespace ControlChanged;
			sendEventData[P_ELEMENT] = this;
			sendEventData[P_OLDVALUE] = oldValue;
			sendEventData[P_VALUE] = value_;
			sendEventData[P_VALIDATED] = IsValid();
			SendEvent(E_CONTROLCHANGED, sendEventData);
		}

		/// Validate control value.
		virtual bool Validate(const T& value)
		{
			return true;
		}

	private:
		T value_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  FLOAT CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class FloatControl : public IControl<float>
	{
		URHO3D_OBJECT(FloatControl, IControl<float>)

	public:
		/// Construct.
		explicit FloatControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Get sensibility (default 0.1).
		float GetSensibility();
		/// Get min value (default 1).
		float GetMin();
		/// Get max value (default 524288).
		float GetMax();
		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const float& value) override;
		/// Set sensibility.
		void SetSensibility(float sensibility);
		/// Set min value.
		void SetMin(float min);
		/// Set max value.
		void SetMax(float max);
		/// Set editable.
		void SetEditable(bool editable);

		/// Increase value (+= sensibility).
		void Increase();
		/// Decrease value (-= sensibility).
		void Decrease();

	private:
		/// Sanitize string to normalized float value.
		Urho3D::String Sanitize(Urho3D::String text);
		/// Check if is a valid character, only [0-9.-] char are allowed.
		bool IsValidCharacter(char character);
		/// Validate predicate.
		bool Validate(const float& value) override;

		/// Handle mouse wheel events.
		void HandleMouseWheel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		/// Handle text events.
		void HandleTextChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleTextEntry(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::LineEdit* lineEdit_;

		float sensibility_;
		float min_;
		float max_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  QUATERNION CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class QuaternionControl : public Geode::IControl<Urho3D::Quaternion>
	{
		URHO3D_OBJECT(QuaternionControl, Geode::IControl<Urho3D::Quaternion>)

	public:
		/// Construct.
		explicit QuaternionControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::Quaternion& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle angle control value changed.
		void HandleAngleControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::FloatControl* angleControl_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  VECTOR2 CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class Vector2Control : public Geode::IControl<Urho3D::Vector2>
	{
		URHO3D_OBJECT(Vector2Control, Geode::IControl<Urho3D::Vector2>)

	public:
		/// Construct.
		explicit Vector2Control(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::Vector2& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle each float control value changed to update control value.
		void HandleXControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleYControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::FloatControl* xControl_;
		Geode::FloatControl* yControl_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  VECTOR3 CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class Vector3Control : public Geode::IControl<Urho3D::Vector3>
	{
		URHO3D_OBJECT(Vector3Control, Geode::IControl<Urho3D::Vector3>)

	public:
		/// Construct.
		explicit Vector3Control(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::Vector3& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle each float control value changed to update control value.
		void HandleXControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleYControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleZControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::FloatControl* xControl_;
		Geode::FloatControl* yControl_;
		Geode::FloatControl* zControl_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  VECTOR4 CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class Vector4Control : public Geode::IControl<Urho3D::Vector4>
	{
		URHO3D_OBJECT(Vector4Control, Geode::IControl<Urho3D::Vector4>)

	public:
		/// Construct.
		explicit Vector4Control(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::Vector4& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle each float control value changed to update control value.
		void HandleXControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleYControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleZControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleWControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::FloatControl* xControl_;
		Geode::FloatControl* yControl_;
		Geode::FloatControl* zControl_;
		Geode::FloatControl* wControl_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  RECT CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class RectControl : public Geode::IControl<Urho3D::Rect>
	{
		URHO3D_OBJECT(RectControl, Geode::IControl<Urho3D::Rect>)

	public:
		/// Construct.
		explicit RectControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::Rect& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle each float control value changed to update control value.
		void HandleX1ControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleY1ControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleX2ControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleY2ControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::FloatControl* x1Control_;
		Geode::FloatControl* y1Control_;
		Geode::FloatControl* x2Control_;
		Geode::FloatControl* y2Control_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  COLOR CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class ColorControl : public Geode::IControl<Urho3D::Color>
	{
		URHO3D_OBJECT(ColorControl, Geode::IControl<Urho3D::Color>)

	public:
		/// Construct.
		explicit ColorControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::Color& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle each float control value changed to update control value.
		void HandleRControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleGControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleBControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleAControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::FloatControl* rControl_;
		Geode::FloatControl* gControl_;
		Geode::FloatControl* bControl_;
		Geode::FloatControl* aControl_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  INTEGER CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class IntegerControl : public IControl<int>
	{
		URHO3D_OBJECT(IntegerControl, IControl<int>)

	public:
		/// Construct.
		explicit IntegerControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Get sensibility.
		int GetSensibility();
		/// Get min value.
		int GetMin();
		/// Get max value.
		int GetMax();
		/// Is Editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const int& value) override;
		/// Set sensibility.
		void SetSensibility(int sensibility);
		/// Set min value.
		void SetMin(int min);
		/// Set max value.
		void SetMax(int max);
		/// Set editable.
		void SetEditable(bool editable);

		/// Increase value (+= sensibility).
		void Increase();
		/// Decrease value (-= sensibility).
		void Decrease();

	private:
		/// Sanitize string to normalized int value, only [0-9] char are allowed.
		Urho3D::String Sanitize(Urho3D::String text);
		/// Check if is a valid character, only [0-9.-] char are allowed.
		bool IsValidCharacter(char character);
		/// Validate predicate.
		bool Validate(const int& value) override;

		/// Handle mouse wheel events.
		void HandleMouseWheel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		/// Handle text events.
		void HandleTextChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleTextEntry(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::LineEdit* lineEdit_;

		int sensibility_;
		int min_;
		int max_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  INTVECTOR2 CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class IntVector2Control : public Geode::IControl<Urho3D::IntVector2>
	{
		URHO3D_OBJECT(IntVector2Control, Geode::IControl<Urho3D::IntVector2>)

	public:
		/// Construct.
		explicit IntVector2Control(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::IntVector2& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle each float control value changed to update control value.
		void HandleXControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleYControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::IntegerControl* xControl_;
		Geode::IntegerControl* yControl_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  INTVECTOR3 CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class IntVector3Control : public Geode::IControl<Urho3D::IntVector3>
	{
		URHO3D_OBJECT(IntVector3Control, Geode::IControl<Urho3D::IntVector3>)

	public:
		/// Construct.
		explicit IntVector3Control(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::IntVector3& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle each float control value changed to update control value.
		void HandleXControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleYControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleZControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::IntegerControl* xControl_;
		Geode::IntegerControl* yControl_;
		Geode::IntegerControl* zControl_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  INTRECT CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class IntRectControl : public Geode::IControl<Urho3D::IntRect>
	{
		URHO3D_OBJECT(IntRectControl, Geode::IControl<Urho3D::IntRect>)

	public:
		/// Construct.
		explicit IntRectControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::IntRect& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle each float control value changed to update control value.
		void HandleLeftControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleTopControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleRightControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleBottomControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::IntegerControl* leftControl_;
		Geode::IntegerControl* topControl_;
		Geode::IntegerControl* rightControl_;
		Geode::IntegerControl* bottomControl_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CHECKBOX CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class CheckboxControl : public IControl<bool>
	{
		URHO3D_OBJECT(CheckboxControl, IControl<bool>)

	public:
		/// Construct.
		explicit CheckboxControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Get label.
		const Urho3D::String& GetLabel() const;
		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const bool& value) override;
		/// Set label.
		void SetLabel(const Urho3D::String& label);
		/// Set editable.
		void SetEditable(bool editable);

		/// React on element click.
		void OnClickBegin(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor);

	private:
		/// Handle toggled checkbox event.
		void HandleCheckBoxToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::CheckBox* checkBox_;
		Urho3D::Text* label_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  RADIO CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class RadioControl : public IControl<bool>
	{
		URHO3D_OBJECT(RadioControl, IControl<bool>)

	public:
		/// Construct.
		explicit RadioControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		const Urho3D::String& GetGroupName() const;
		/// Get label.
		const Urho3D::String& GetLabel() const;
		/// Is editing.
		bool IsEditing() override;

		/// Set group name.
		void SetGroupName(const Urho3D::String& groupName);
		/// Set value.
		void SetValue(const bool& value) override;
		/// Set label.
		void SetLabel(const Urho3D::String& label);
		/// Set editable.
		void SetEditable(bool editable);

		/// React on element click.
		void OnClickBegin(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor);

	private:
		/// Handle toggled radio event.
		void HandleRadioToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::Radio* radio_;
		Urho3D::Text* label_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  STRING CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class StringControl : public Geode::IControl<Urho3D::String>
	{
		URHO3D_OBJECT(StringControl, Geode::IControl<Urho3D::String>)

	public:
		/// Construct.
		explicit StringControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::String& value) override;
		/// Set pattern.
		void SetPattern(const Urho3D::String& pattern);
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Validate predicate.
		bool Validate(const Urho3D::String& value) override;

		/// Handle text changed.
		void HandleTextChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::LineEdit* lineEdit_;
		std::regex reg_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  STRING LIST CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class StringListControl : public Geode::IControl<Urho3D::StringVector>
	{
		URHO3D_OBJECT(StringListControl, Geode::IControl<Urho3D::StringVector>)

	public:
		/// Construct.
		explicit StringListControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::StringVector& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Join string list to string.
		Urho3D::String Join(const Urho3D::StringVector& v, char separator);

		/// Handle text changed.
		void HandleTextChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::LineEdit* lineEdit_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  FILE CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class FileControl : public IControl<Urho3D::String>
	{
		URHO3D_OBJECT(FileControl, IControl<Urho3D::String>)

	public:
		/// Construct.
		explicit FileControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::String& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle text changed.
		void HandleTextChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		/// Handle select button released event.
		void HandleSelectButtonReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		/// Handle file selected from file dialog selector.
		void HandleFileSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::LineEdit* lineEdit_;
		Urho3D::Button* selectBtn_;
		Urho3D::SharedPtr<Urho3D::FileSelector> fileSelector_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  RESOURCE REF CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class ResourceRefControl : public Geode::IControl<Urho3D::ResourceRef>
	{
		URHO3D_OBJECT(ResourceRefControl, Geode::IControl<Urho3D::ResourceRef>)

	public:
		/// Construct.
		explicit ResourceRefControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::ResourceRef& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle text changed.
		void HandleTextChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		/// Handle select button released event.
		void HandleSelectButtonReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		/// Handle file selected from file dialog selector.
		void HandleFileSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::LineEdit* lineEdit_;
		Urho3D::Button* selectBtn_;
		Urho3D::SharedPtr<Urho3D::FileSelector> fileSelector_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  SELECT CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class SelectControl : public Geode::IControl<int>
	{
		URHO3D_OBJECT(SelectControl, Geode::IControl<int>)

	public:
		/// Construct.
		explicit SelectControl(Urho3D::Context* context);
		~SelectControl();
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Add option.
		void AddOption(const Urho3D::String& text, int value);
		/// Get the index of matching option to value.
		int IndexOf(int value);

		/// Is editing.
		bool IsEditing() override;

		/// Set value if found in options.
		void SetValue(const int& value) override;
		/// Set editable.
		void SetEditable(bool editable);

	private:
		/// Handle dropdownlist item selected to update control value.
		void HandleItemSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::DropDownList* dropDownList_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  VARIANT CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class VariantControl : public Geode::IControl<Urho3D::Variant>
	{
		URHO3D_OBJECT(VariantControl, Geode::IControl<Urho3D::Variant>)

	public:
		/// Construct.
		explicit VariantControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::Variant& value) override;
		/// Set editable.
		void SetEditable(bool editable);
		/// Set types.
		void SetAllowedTypes(Urho3D::Vector<Urho3D::VariantType> types, Urho3D::VariantType selectedType);

	private:
		/// Set internal control value.
		void SetInternalControlValue(Urho3D::Variant value);

		/// Handle control value changed.
		void HandleInternalControlValueChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		/// Handle type selection value changed.
		void HandleTypeSelectorValueChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::IControlBase* internalControl_;
		Geode::SelectControl* typeSelector_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  VARIANT MAP CONTROL
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class VariantMapControl : public Geode::IControl<Urho3D::VariantMap>
	{
		URHO3D_OBJECT(VariantMapControl, Geode::IControl<Urho3D::VariantMap>)

	public:
		/// Construct.
		explicit VariantMapControl(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Is editing.
		bool IsEditing() override;

		/// Set value.
		void SetValue(const Urho3D::VariantMap& value) override;

	private:
		/// Find item in ui list element.
		Urho3D::UIElement* FindItem(Urho3D::StringHash keyName);
		/// Add item in ui list element.
		void AddItem(Urho3D::String keyName, Urho3D::Variant variant);

		/// Handle variant control value changed; sync with variantmap.
		void HandleVariantControlValueChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		/// Handle new button released.
		void HandleNewButtonReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		/// Handle item delete button released.
		void HandleItemDeleteButtonReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::StringControl* keyNameControl_;
		Urho3D::Button* newBtn_;
		Urho3D::UIElement* items_;
	};
}
