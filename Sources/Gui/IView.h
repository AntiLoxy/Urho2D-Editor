/**
 * @file    IView.h
 * @ingroup Gui
 * @brief   Base class for all ui view.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UIElement.h>

namespace Geode
{
	class IView : public Urho3D::Object
	{
		URHO3D_OBJECT(IView, Urho3D::Object)

	public:
		/// Construct.
		explicit IView(Urho3D::Context* context, Urho3D::String layoutFile = "", Urho3D::UIElement* parent = nullptr);
		~IView();

		/// Set name.
		void SetName(Urho3D::String name);

		/// Get local root element.
		Urho3D::SharedPtr<Urho3D::UIElement> GetElementRoot();
		/// Get name.
		Urho3D::String GetName();

	protected:
		Urho3D::UIElement* uiRoot_;
		Urho3D::SharedPtr<Urho3D::UIElement> elRoot_;
		Urho3D::String name_;
	};
}
