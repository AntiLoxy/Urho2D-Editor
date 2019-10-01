/**
 * @file    AnchorBox.h
 * @ingroup Editor
 * @brief   AnchorBox is tool used to resize object.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "SceneView.h"
#include "SceneViewEvents.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/CustomGeometry.h>

namespace Geode
{
	class AnchorBox : public Urho3D::Object
	{
		URHO3D_OBJECT(AnchorBox, Urho3D::Object)

	public:
		using Ptr = Urho3D::SharedPtr<AnchorBox>;

		enum AnchorId {
			TOP,
			TOP_LEFT,
			TOP_RIGHT,
			MIDDLE,
			MIDDLE_LEFT,
			MIDDLE_RIGHT,
			BOTTOM,
			BOTTOM_LEFT,
			BOTTOM_RIGHT,
			LENGTH
		};

	public:
		/// Constructors.
		explicit AnchorBox(Urho3D::Context* context, Geode::SceneView::Ptr sceneView);

		/// Accessors & Mutators.
		void SetEnabled(bool enabled_);
		void SetHomothetics(bool homothetics);
		void SetCenter(Urho3D::Vector2 center);
		void SetSize(Urho3D::Vector2 size);
		void SetTransform(Urho3D::Matrix3x4 transform);
		bool IsEnabled();
		bool IsEditing();
		bool IsHomothetics();
		Urho3D::Vector2 GetCenter();
		Urho3D::Vector2 GetSize();

		/// Other methods.
		void AddAnchor(AnchorId anchorId);
		void RemoveAnchor(AnchorId anchorId);
		bool HasAnchor(AnchorId anchorId);
		int GetAnchorIdAt(Urho3D::Vector2 position);
		Urho3D::Vector2 GetAnchorPosition(int anchorId);

	private:
		/// Event handlers.
		void HandleSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragBegin(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragCancel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneViewDragEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		/// Update methods.
		void UpdateRender();

	private:
		Geode::SceneView::Ptr sceneView_;
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::SharedPtr<Urho3D::Node> boxNode_;
		Urho3D::CustomGeometry* boxGeometry_;
		bool enabled_;
		bool editing_;
		Urho3D::Vector2 center_;
		Urho3D::Vector2 size_;
		Urho3D::Matrix3x4 transform_;
		Urho3D::Vector2 beginEditingCenter_;
		Urho3D::Vector2 beginEditingSize_;
		Urho3D::Vector2 beginEditingMousePosition_;
		Urho3D::HashSet<int> anchorsIds_;
		int selectedAnchorId_;
		bool homothetics_;
	};
}