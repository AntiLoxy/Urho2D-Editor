/**
 * @file    CollisionCircle2DTool.h
 * @ingroup Editor
 * @brief   Tool used to edit CollisionCircle2D component.
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
#include "AnchorBoxEvents.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>

namespace Geode
{
	class CollisionCircle2DTool : public Geode::ITool
	{
		URHO3D_OBJECT(CollisionCircle2DTool, Geode::ITool)

	public:
		/// Constructors.
		explicit CollisionCircle2DTool(Urho3D::Context* context, Geode::CommandHistory::Ptr commandHistory, Geode::SceneView::Ptr sceneView);

		/// Other methods.
		Urho3D::UIElement* PopulateToolBar(Geode::ToolBar* toolBar) override;
		bool IsActive() override;

	private:
		/// Event handlers.
		void HandleSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSelectedObjectChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleAnchorBoxSizeChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleAnchorBoxCenterChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleAnchorBoxConfirmSizeChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleAnchorBoxConfirmCenterChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		void HandleAnchorBoxCanceled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		/// Accessors & Mutators.
		void SetSelectedCollisionCircle2D(Urho3D::CollisionCircle2D* selectedCollisionCircle2D);

	private:
		Geode::CommandHistory::Ptr commandHistory_;
		Geode::SceneView::Ptr sceneView_;
		Geode::EditorScene::Ptr editorScene_;
		Geode::AnchorBox::Ptr anchorBox_;
		Urho3D::CollisionCircle2D* selectedCollisionCircle2D_;
	};
}