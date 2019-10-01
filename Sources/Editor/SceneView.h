/**
 * @file    SceneView.h
 * @ingroup Editor
 * @brief   Editor scene view container.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "Gizmo.h"
#include "Grid.h"
#include "EditorScene.h"
#include "../Gui/IView.h"

#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Viewport.h>

namespace Geode
{
	class SceneView : public Geode::IView
	{
		URHO3D_OBJECT(SceneView, Geode::IView)

	public:
		using Ptr = Urho3D::SharedPtr<SceneView>;

	public:
		/// Constructors.
		explicit SceneView(Urho3D::Context* context, Urho3D::UIElement* parent, Geode::EditorScene::Ptr editorScene);

		/// Accessors & Mutators.
		void SetDebugGeometryEnabled(bool enabled);
		void SetNodeSelectionEnabled(bool enabled);
		void SetNodePositionGizmoEnabled(bool enabled);
		void SetGridEnabled(bool enabled);
		Geode::EditorScene::Ptr GetEditorScene();
		bool GetDebugGeometryEnabled();
		bool GetNodeSelectionEnabled();
		bool GetNodePositionGizmoEnabled();
		bool GetGridEnabled();
		Urho3D::Vector2 GetMouseMove();
		bool IsDragging();

		/// Other methods.
		Urho3D::Vector2 GetMouseWorldPosition();
		bool GetMouseButtonDown(Urho3D::MouseButtonFlags button);
		bool GetMouseButtonPress(Urho3D::MouseButtonFlags button);
		bool IsMouseInsideNode(Urho3D::Node* node);
		bool IsMouseInsideViewport();

	private:
		/// Event handlers.
		void HandleResized(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSelectedObjectChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleScenePostRender(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleMouseButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleMouseMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleMouseButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		/// Update methods.
		void UpdateCameraMoves(float ts);
		void UpdateNodeSelection(float ts);
		void UpdateMouseMove(float ts);

	private:
		Geode::EditorScene::Ptr editorScene_;
		bool debugGeometryEnabled_;
		bool nodeSelectionEnabled_;
		Urho3D::Vector2 lastMouseWorldPosition_;
		Urho3D::Vector2 mouseWorldMove_;
		bool dragging_;
		Urho3D::Vector2 beginDragMouseWorldPosition_;
		Urho3D::SharedPtr<Urho3D::Viewport> viewport_;
		Urho3D::SharedPtr<Urho3D::Node> cameraNode_;
		Urho3D::SharedPtr<Urho3D::Camera> camera_;
		Urho3D::SharedPtr<Geode::Gizmo> gizmo_;
		Urho3D::SharedPtr<Geode::Grid> grid_;
	};
}
