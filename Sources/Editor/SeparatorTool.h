/**
 * @file    SeparatorTool.h
 * @ingroup Editor/Tools
 * @brief   Tool used to separate others tool.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "ITool.h"
#include "../Gui/Toolbar.h"

#include <Urho3D/Core/Context.h>

namespace Geode
{
	class SeparatorTool : public Geode::ITool
	{
		URHO3D_OBJECT(SeparatorTool, Geode::ITool)

	public:
		/// Constructors.
		explicit SeparatorTool(Urho3D::Context* context);

		/// Other methods.
		Urho3D::UIElement* PopulateToolBar(Geode::ToolBar* toolBar) override;

	private:
		Urho3D::UIElement* separator_;
	};
}