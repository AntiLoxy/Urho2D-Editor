/**
 * @file    HierarchyWindowView.h
 * @ingroup Editor
 * @brief   WindowView to handle the scene hierarchy.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "EditorScene.h"
#include "EditorSceneEvents.h"
#include "Commands.h"
#include "../Gui/IWindowView.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Component.h>

namespace Geode
{
	class HierarchyWindowView : public Geode::IWindowView
	{
		URHO3D_OBJECT(HierarchyWindowView, Geode::IWindowView)

	public:
		/// Constructors.
		explicit HierarchyWindowView(Urho3D::Context* context, Geode::CommandHistory::Ptr commandHistory, Geode::EditorScene::Ptr editorScene);
		
	private:
		/// Event handlers.
		void HandleSelectedItem(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneLoaded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSelectedObjectChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneNodeNameChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneNodeAdded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneNodeRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneComponentAdded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneComponentRemoved(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleDeleteButtonReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleCreateNodeButtonReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		/// Accessors & Mutators.
		void SetSelectedObject(Urho3D::Object* object);

		/// Other methods.
		void UpdateHierarchyList();
		Urho3D::UIElement* AddNode(Urho3D::Node* node, Urho3D::UIElement* parentItem);
		Urho3D::UIElement* AddComponent(Urho3D::Component* component, Urho3D::UIElement* parentItem);
		Urho3D::UIElement* FindItem(Urho3D::Object* object);
		int IndexOfObject(Urho3D::Object* object);

	private:
		Geode::CommandHistory::Ptr commandHistory_;
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::ListView* hierarchyList_;
		Urho3D::Button* deleteButton_;
		Urho3D::Button* createNodeButton_;
		Urho3D::Vector<Urho3D::Component*> removedComponentsBuf_;
		Urho3D::Vector<Urho3D::Node*> removedNodesBuf_;
	};
}
