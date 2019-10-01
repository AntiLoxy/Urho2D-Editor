/**
 * @file    Toolbar.h
 * @ingroup Gui
 * @brief   Toolbar UI element.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Radio.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/Button.h>

namespace Geode
{
	class ToolBar : public Urho3D::BorderImage
	{
		URHO3D_OBJECT(ToolBar, Urho3D::BorderImage)

	public:
		/// Construct.
		explicit ToolBar(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Add text button.
		Urho3D::Button* AddButton(Urho3D::String name, Urho3D::String labelText);
		/// Add icon button.
		Urho3D::Button* AddIconButton(Urho3D::String name, Urho3D::String iconName, Urho3D::String tooltipText);
		/// Add icon radio.
		Geode::Radio* AddIconRadio(Urho3D::String name, Urho3D::String radioGroupName, Urho3D::String iconName, Urho3D::String tooltipText);
		/// Add icon toggle.
		Urho3D::CheckBox* AddIconToggle(Urho3D::String name, Urho3D::String iconName, Urho3D::String tooltipText);
		/// Add separator.
		Urho3D::UIElement* ToolBar::AddSeparator();
	};
}
