/**
 * @file    FlyMenu.h
 * @ingroup Gui
 * @brief   Flying menu.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "GuiEvents.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/Menu.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>

namespace Geode
{
	class FlyMenu : public Urho3D::Menu
	{
		URHO3D_OBJECT(FlyMenu, Urho3D::Menu)

	public:
		/// Construct.
		explicit FlyMenu(Urho3D::Context* context);
		/// Destruct.
		~FlyMenu();
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Add button item.
		Urho3D::Button* AddButton(const Urho3D::String& text);
		/// Add toggle item.
		Urho3D::CheckBox* AddToggle(const Urho3D::String& text);
		/// Add sub menu item.
		FlyMenu* AddSubMenu(const Urho3D::String& text);
		/// Add separator item.
		Urho3D::UIElement* AddSeparator();

		/// Reset actions.
		void Reset();

	private:
		/// React on popup show.
		void OnShowPopup() override;
		/// React on popup hide.
		void OnHidePopup() override;

	private:
		/// Handle item click end event.
		void HandleItemClickEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	};
}