/**
 * @file    MenuBar.h
 * @ingroup Gui
 * @brief   Top menu bar.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "FlyMenu.h"
#include "GuiEvents.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/BorderImage.h>

namespace Geode
{
	class MenuBar : public Urho3D::BorderImage
	{
		URHO3D_OBJECT(MenuBar, Urho3D::BorderImage)

	public:
		/// Construct.
		explicit MenuBar(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Add menu.
		Geode::FlyMenu* AddMenu(const Urho3D::String& text);
	};
}
