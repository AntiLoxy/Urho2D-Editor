/**
 * @file    Helper.h
 * @ingroup Gui
 * @brief   A list of shortcut methods for ui management.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/Button.h>

namespace Geode
{
	class Helper
	{
	public:
		static Urho3D::BorderImage* CreateIcon(Urho3D::UIElement* parent, Urho3D::String iconName, Urho3D::HorizontalAlignment ha, Urho3D::VerticalAlignment va, unsigned int index = Urho3D::M_MAX_UNSIGNED);
		static Urho3D::ToolTip* CreateToolTip(Urho3D::UIElement* parent, Urho3D::String tooltipText, Urho3D::IntVector2 offset);

	public:
		static void	Sensitive(Urho3D::UIElement* element);
		static void	Unsensitive(Urho3D::UIElement* element);
	};
}