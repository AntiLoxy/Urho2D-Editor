/**
 * @file    ITool.h
 * @ingroup Editor
 * @brief   ITool is a base class for all tools.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "../Gui/Toolbar.h"

#include <Urho3D/Core/Context.h>

namespace Geode
{
	class ITool : public Urho3D::Object
	{
		URHO3D_OBJECT(ITool, Urho3D::Object)

	public:
		using Ptr = Urho3D::SharedPtr<ITool>;

	public:
		/// Constructors.
		explicit ITool(Urho3D::Context* context);

		/// Accessors & Mutators.
		virtual void SetEnabled(bool enabled);
		virtual bool IsEnabled() const;

		/// Other methods.
		virtual Urho3D::UIElement* PopulateToolBar(Geode::ToolBar* toolBar);
		virtual bool IsActive();

	private:
		bool enabled_;
	};
}