/**
 * @file    TabBar.h
 * @ingroup Gui
 * @brief   TabBar class provide a stack of radio buttons.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Radio.h"
#include "GuiEvents.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UIElement.h>

namespace Geode
{
	class TabBar : public Urho3D::BorderImage
	{
		URHO3D_OBJECT(TabBar, Urho3D::BorderImage)

	public:
		/// Construct.
		explicit TabBar(Urho3D::Context* context);
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Add new tab.
		unsigned int AddTab(const Urho3D::String& label, const Urho3D::String& toolTip = Urho3D::String::EMPTY);
		/// Insert new tab.
		unsigned int InsertTab(unsigned int index, const Urho3D::String& label, const Urho3D::String& toolTip = Urho3D::String::EMPTY);
		/// Remove tab.
		void RemoveTab(unsigned int index);
		/// Move tab.
		void MoveTab(unsigned int from, unsigned int to);
		/// Select tab.
		void SelectTab(unsigned int index);
		/// Set tab visiblity.
		void SetTabVisible(unsigned int index, bool visible);
		/// Set tabs closable.
		void SetTabsClosable(bool tabsClosable);

		/// Get tab visibility.
		bool GetTabVisible(unsigned int index) const;
		/// Get tabs closable.
		bool GetTabsClosable() const;
		/// Has tab.
		bool HasTab(unsigned int index) const;
		/// Get current index.
		unsigned int GetCurrentIndex() const;

	private:
		/// Create tab radio element.
		Urho3D::SharedPtr<Geode::Radio> CreateTab(const Urho3D::String& label, const Urho3D::String& toolTip = Urho3D::String::EMPTY);
		/// Create tooltip in parent element.
		void CreateToolTip(Urho3D::UIElement* parent, Urho3D::String text, Urho3D::IntVector2 offset);
		/// Create close button in parent element.
		void CreateCloseButton(Urho3D::UIElement* parent);

		/// Handle ui events.
		void HandleTabCloseButtonReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleTabToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		bool tabsClosable_;
		Urho3D::Vector<Geode::Radio*> tabs_;
		unsigned int currentIndex_;
		unsigned int id_;
	};
}
