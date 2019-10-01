/**
 * @file    ToolsView.h
 * @ingroup Editor
 * @brief   Tools view container.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "ITool.h"
#include "ToolsEvents.h"
#include "SeparatorTool.h"
#include "../Gui/IView.h"
#include "../Gui/Toolbar.h"

namespace Geode
{
	class ToolsView : public Geode::IView
	{
		URHO3D_OBJECT(ToolsView, Geode::IView)

	public:
		/// Constructors.
		explicit ToolsView(Urho3D::Context* context, Urho3D::UIElement* parent);

		/// Other methods.
		void AddTool(Geode::ITool::Ptr tool);
		void AddSeparator();

	private:
		/// Event handlers.
		void HandleSelectedTool(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Geode::ToolBar* toolBar_;
		Urho3D::Vector<Geode::ITool::Ptr> tools_;
	};
}
