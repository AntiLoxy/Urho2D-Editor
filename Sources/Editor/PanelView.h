/**
 * @file    PanelView.h
 * @ingroup Editor
 * @brief   Panel view class provide a convenient way to group windows views.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "../Gui/IView.h"
#include "../Gui/GuiEvents.h"
#include "../Gui/IWindowView.h"
#include "../Gui/TabBar.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/ScrollView.h>

namespace Geode
{
	class PanelView : public Geode::IView
	{
		URHO3D_OBJECT(PanelView, Geode::IView)

	public:
		explicit PanelView(Urho3D::Context* context, Urho3D::UIElement* parent = nullptr);

		bool RegisterWindowView(Urho3D::SharedPtr<Geode::IWindowView> windowView);
		void UnregisterWindowView(Geode::IWindowView* windowView);
		unsigned int IndexOfRegisteredWindowView(Geode::IWindowView* windowView);
		bool IsRegisteredWindowView(Geode::IWindowView* windowView);

		bool AddTab(Geode::IWindowView* windowView, bool autoSelect = false);
		void RemoveTab(unsigned int index);
		void SelectTab(unsigned int index);

	private:
		void SetContent(Urho3D::UIElement* element);

		void HandleTabRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleTabSelectionChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		void HandleWindowViewClosed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleWindowViewOpened(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleWindowViewDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::TabBar* tabBar_;
		Urho3D::ScrollView* scrollView_;
		Urho3D::SharedPtr<Urho3D::UIElement> scrollContent_;

		Urho3D::Vector<Urho3D::SharedPtr<Geode::IWindowView>> registeredWindowViews_;
		Urho3D::Vector<Geode::IWindowView*> windowViews_;
		Geode::IWindowView* currentWindowView_;
	};
}
