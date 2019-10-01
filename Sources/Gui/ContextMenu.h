/**
 * @file    ContextMenu.h
 * @ingroup Gui
 * @brief	ContextMenu implementation based on FlyMenu.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "FlyMenu.h"
#include "GuiEvents.h"

#include <Urho3D/Core/Object.h>

namespace Geode
{
	class ContextMenu : public Urho3D::Object
	{
		URHO3D_OBJECT(ContextMenu, Urho3D::Object)

	public:
		/// Construct.
		explicit ContextMenu(Urho3D::Context* context);
		/// Destruct.
		~ContextMenu();

		/// Get flymenu.
		Geode::FlyMenu* Get();
		/// Show menu.
		void Show(int x, int y);

	private:
		/// Handle flymenu events.
		void HandleFlyMenuClosed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::FlyMenu* flyMenu_;
	};
}
