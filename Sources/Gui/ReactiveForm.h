/**
 * @file    ReactiveForm.h
 * @ingroup Gui
 * @brief   AttributeField : Form field for serializable object attribute.
 *			ReactiveForm   : Provide a model-driven (only for serializable object) approach to handling form inputs whose values update model.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Controls.h"
#include "GuiEvents.h"

#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Container/HashMap.h>

namespace Geode
{
	class AttributeField : public Urho3D::UIElement
	{
		URHO3D_OBJECT(AttributeField, Urho3D::UIElement)

	public:
		/// Construct.
		explicit AttributeField(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Get attribute name.
		Urho3D::String GetAttributeName();
		/// Get control.
		Geode::IControlBase* GetControl();

		/// Set serializable attribute.
		void SetAttribute(Urho3D::Serializable* serializable, const Urho3D::String& attributeName);
		/// Set label.
		void SetLabel(const Urho3D::String& label);
		/// Set bi-directionnal binding enable.
		void SetBidirectionnalBinding(bool enable);

		/// Show field & enable events.
		void Activate();
		/// Hide field & disable events.
		void Desactivate();

	private:
		/// Handle control value changed to update attribute value.
		void HandleControlChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		/// Handle core events.
		void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		bool activated_;
		bool bidirectionnalBinding_;
		Urho3D::Serializable* serializable_;
		Urho3D::String attributeName_;

		Urho3D::Text* label_;
		Geode::IControlBase* control_;
	};
}

namespace Geode
{
	class ReactiveForm : public Urho3D::UIElement
	{
		URHO3D_OBJECT(ReactiveForm, Urho3D::UIElement)

	public:
		/// Construct.
		explicit ReactiveForm(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Get attribute field.
		Geode::AttributeField* GetAttributeField(const Urho3D::String& attributeName);
		/// Test if form is valid.
		bool IsValid();

		/// Add attribute field.
		Geode::AttributeField* AddAttributeField(Urho3D::Serializable* serializable, Urho3D::String attributeName, Urho3D::String label);
		// Remove all attribute fields.
		void Clear();

	private:
		// Handle attribute field data changed.
		void HandleAttributeFieldDataChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::Vector<Geode::AttributeField*> attrFields_;
	};
}
