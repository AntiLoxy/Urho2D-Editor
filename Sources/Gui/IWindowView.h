/**
 * @file    IWindowView.h
 * @ingroup Gui
 * @brief   Base class for all window view. (load the given layout file with some requirements)
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "IView.h"
#include "GuiEvents.h"

#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Window.h>

namespace Geode
{
	class IWindowView : public Geode::IView
	{
		URHO3D_OBJECT(IWindowView, Geode::IView)
	
	public:
		enum class Mode { NORMAL, DOCKED };

	public:
		/// Construct.
		explicit IWindowView(Urho3D::Context* context, Urho3D::String layoutFile);

		/// Get title.
		const Urho3D::String& GetTitle() const;

		/// Is visible.
		bool IsOpened();

		/// Set mode.
		void SetMode(Mode mode);
		/// Set title.
		void SetTitle(Urho3D::String title);
		/// Open window.
		void Open();
		/// Close window.
		void Close();

		/// Called when window closed.
		virtual void OnClosed();
		/// Called when window opened.
		virtual void OnOpened();

	private:
		/// Handle ui events.
		void HandleCloseButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleDragBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleDragMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	protected:
		bool opened_;
		Mode mode_;

		Urho3D::Window* win_;
		Urho3D::UIElement* header_;
		Urho3D::Text* title_;
		Urho3D::Button* closeButton_;

		Urho3D::SharedPtr<Urho3D::UIElement> body_;
	};
}
