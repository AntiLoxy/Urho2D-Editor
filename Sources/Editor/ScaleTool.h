/**
 * @file    ScaleTool.h
 * @ingroup Editor
 * @brief   Tool used to scale node.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "ITool.h"
#include "ToolsEvents.h"
#include "Commands.h"
#include "SceneView.h"
#include "SceneViewEvents.h"
#include "EditorScene.h"
#include "EditorSceneEvents.h"
#include "../Gui/Toolbar.h"

#include <Urho3D/Core/Context.h>

namespace Geode
{
	class ScaleTool : public Geode::ITool
	{
		URHO3D_OBJECT(ScaleTool, Geode::ITool)

	public:
		/// Constructor.
		explicit ScaleTool(Urho3D::Context* context, Geode::CommandHistory::Ptr commandHistory, Geode::SceneView::Ptr sceneView);

		/// Accessors & Mutators.
		void SetEnabled(bool enabled) override;
		
		/// Other methods.
		Urho3D::UIElement* PopulateToolBar(Geode::ToolBar* toolBar) override;
		bool IsActive() override;

	private:
		/// Event handlers.
		void HandleSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSelectedObjectChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragCancel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleRadioToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		/// Update methods.
		void UpdateEditingState();
		void UpdateCursor();

		/// Accessors & Mutators.
		void SetSelectedNode(Urho3D::Node* selectedNode);

		/// Other methods.
		void EnableRadio(bool enable);

	private:
		Geode::CommandHistory::Ptr commandHistory_;
		Geode::SceneView::Ptr sceneView_;
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::Cursor* cursor_;
		Geode::Radio* radio_;
		Urho3D::Node* selectedNode_;
		bool editing_;
		Urho3D::Vector2 beginNodeScale_;
		Urho3D::Vector2 currentNodeScale_;

		Urho3D::Vector2 beginMouseWorldPosition_;

		Urho3D::Vector2 baseDelta_;

		float baseLength_;
	};
}