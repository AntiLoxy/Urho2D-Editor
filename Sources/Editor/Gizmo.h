/**
 * @file    PositionGizmo.h
 * @ingroup Editor
 * @brief   Scene position node gizmo.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "EditorScene.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/StaticModel.h>

namespace Geode
{
	class Gizmo : public Urho3D::Object
	{
		URHO3D_OBJECT(Gizmo, Urho3D::Object)

	public:
		/// Constructors.
		Gizmo(Urho3D::Context* context, Geode::EditorScene::Ptr editorScene);

		/// Accessors & Mutators.
		void SetEnabled(bool enabled);
		void SetAttachedNode(Urho3D::Node* node);
		bool IsEnabled();

	private:
		/// Event handlers.
		void HandleSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		/// Update methods.
		void UpdateRender();

	private:
		Geode::EditorScene::Ptr editorScene_;
		bool enabled_;
		Urho3D::Node* attachedNode_;
		Urho3D::SharedPtr<Urho3D::Node> gizmoNode_;
		Urho3D::StaticModel* xAxisModel_;
		Urho3D::StaticModel* yAxisModel_;
	};
}