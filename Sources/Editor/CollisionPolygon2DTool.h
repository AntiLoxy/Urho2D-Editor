/**
 * @file    CollisionPolygon2DTool.h
 * @ingroup Editor
 * @brief   Tool used to create/edit CollisionPolygon2D component.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "ITool.h"
#include "Commands.h"
#include "SceneView.h"
#include "SceneViewEvents.h"
#include "EditorScene.h"
#include "EditorSceneEvents.h"
#include "AnchorBox.h"
#include "../Gui/Toolbar.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Urho2D/CollisionPolygon2D.h>

namespace Geode
{
	class CollisionPolygon2DTool : public Geode::ITool
	{
		URHO3D_OBJECT(CollisionPolygon2DTool, Geode::ITool)

		enum Mode {
			INSERTION,
			EDITION
		};

		struct Vertex {
			Urho3D::Vector2 pos;
			Urho3D::Color color;
			float radius;
			bool show;
		};

	public:
		/// Constructors.
		explicit CollisionPolygon2DTool(Urho3D::Context* context, Geode::CommandHistory::Ptr commandHistory, Geode::SceneView::Ptr sceneView);

		/// Accessors & Mutators.
		void SetMode(Mode mode);

		/// Other methods.
		Urho3D::UIElement* PopulateToolBar(Geode::ToolBar* toolBar) override;
		bool IsActive() override;

	private:
		/// Event handlers.
		void HandleSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSelectedObjectChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragCancel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		/// Update methods.
		void UpdateCursor();
		void UpdateComponent();
		void UpdateRender();

		/// Accessors & Mutators.
		void SetSelectedCollisionPolygon2D(Urho3D::CollisionPolygon2D* selectedCollisionPolygon2D);

		/// Other methods.
		int GetVertexIndexAt(Urho3D::Vector2 position);

	private:
		Geode::CommandHistory::Ptr commandHistory_;
		Geode::SceneView::Ptr sceneView_;
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::Cursor* cursor_;
		Urho3D::CollisionPolygon2D* selectedCollisionPolygon2D_;
		Urho3D::SharedPtr<Urho3D::Node> pathNode_;
		Urho3D::CustomGeometry* pathGeometry_;
		Mode mode_;
		Urho3D::Vector2 beginEditingVertexPosition_;
		Urho3D::Vector2 beginEditingMousePosition_;
		int selectedVertexIndex_;
	};
}