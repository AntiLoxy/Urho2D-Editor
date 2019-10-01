/**
 * @file    EditorScene.h
 * @ingroup Geode
 * @brief	Editor scene.
 *
 * Copyright (c) 2018 Julien Rongière (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "EditorSceneEvents.h"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Component.h>

namespace Geode
{
	class EditorScene : public Urho3D::Scene
	{
		URHO3D_OBJECT(EditorScene, Urho3D::Scene)

	public:
		using Ptr = Urho3D::SharedPtr<EditorScene>;

	public:
		/// Constructors.
		explicit EditorScene(Urho3D::Context* context);
		static void RegisterObject(Urho3D::Context* context);

		/// Accessors & Mutators.
		void SetSelectedObject(Urho3D::Node* node);
		void SetSelectedObject(Urho3D::Component* component);
		template<typename T> T* GetSelectedObjectAs();
		Urho3D::Object* GetSelectedObject();
		Urho3D::Node* GetSelectedNode();
		Urho3D::Component* GetSelectedComponent();
		void ClearSelection();

		/// Other methods.
		void Load(const Urho3D::String& filename);
		void Save(const Urho3D::String& filename);
		Urho3D::Node* CreateNewNode(Urho3D::Node* parentNode);
		Urho3D::Node* GetNodeAt(Urho3D::Vector3 pos);
		Urho3D::Node* GetNodeAt(Urho3D::Vector2 pos);
		Urho3D::BoundingBox GetNodeWorldBoundingBox(Urho3D::Node* node);
		Urho3D::Rect GetNodeWorldRect(Urho3D::Node* node);
		int IndexOfNode(Urho3D::Node* parentNode, Urho3D::Node* childNode);
		int IndexOfComponent(Urho3D::Node* parentNode, Urho3D::Component* component);

	private:
		/// Event handlers.
		void HandleSceneNodeRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneComponentRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	private:
		Urho3D::Object* selectedObject_;
	};
}

namespace Geode
{
	template<typename T>
	T* EditorScene::GetSelectedObjectAs()
	{
		auto selectedObject = GetSelectedObject();

		if (selectedObject != nullptr && selectedObject->IsInstanceOf<T>())
		{
			return static_cast<T*>(selectedObject);
		}
		else
		{
			return nullptr;
		}
	}
}