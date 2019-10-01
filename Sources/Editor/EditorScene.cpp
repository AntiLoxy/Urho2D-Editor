#include "EditorScene.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Graphics/Drawable.h>

using namespace Urho3D;

namespace Geode
{
	EditorScene::EditorScene(Context* context) : Scene(context)
	{
		selectedObject_ = nullptr;

		SubscribeToEvent(this, E_NODEREMOVED, URHO3D_HANDLER(EditorScene, HandleSceneNodeRemoved));
		SubscribeToEvent(this, E_COMPONENTREMOVED, URHO3D_HANDLER(EditorScene, HandleSceneComponentRemoved));

		Load("Scenes/Room/scene.xml");
	}

	void EditorScene::RegisterObject(Context* context)
	{
		context->RegisterFactory<EditorScene>();
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void EditorScene::SetSelectedObject(Node* node)
	{
		if (node != selectedObject_)
		{
			selectedObject_ = node;

			VariantMap sendEventData;
			sendEventData[SelectedObjectChanged::P_OBJECT] = selectedObject_;
			SendEvent(E_SELECTEDOBJECTCHANGED, sendEventData);
		}
	}

	void EditorScene::SetSelectedObject(Component* component)
	{
		if (component != selectedObject_)
		{
			selectedObject_ = component;

			VariantMap sendEventData;
			sendEventData[SelectedObjectChanged::P_OBJECT] = selectedObject_;
			SendEvent(E_SELECTEDOBJECTCHANGED, sendEventData);
		}
	}

	Object* EditorScene::GetSelectedObject()
	{
		return selectedObject_;
	}

	Node* EditorScene::GetSelectedNode()
	{
		auto selectedObject = GetSelectedObject();

		if (selectedObject == nullptr || !selectedObject->IsInstanceOf<Node>())
		{
			return nullptr;
		}

		return static_cast<Node*>(selectedObject);
	}

	Component* EditorScene::GetSelectedComponent()
	{
		auto selectedObject = GetSelectedObject();

		if (selectedObject == nullptr || !selectedObject->IsInstanceOf<Component>())
		{
			return nullptr;
		}

		return static_cast<Component*>(selectedObject);
	}

	void EditorScene::ClearSelection()
	{
		if (selectedObject_ == nullptr)
		{
			return;
		}

		selectedObject_ = nullptr;

		VariantMap sendEventData;
		sendEventData[SelectedObjectChanged::P_OBJECT] = selectedObject_;
		SendEvent(E_SELECTEDOBJECTCHANGED, sendEventData);
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	void EditorScene::Load(const String& filename)
	{
		auto cache = GetSubsystem<ResourceCache>();
		LoadXML(cache->GetResource<XMLFile>(filename)->GetRoot());

		SendEvent(E_SCENELOADED);
	}

	void EditorScene::Save(const String& filename)
	{
		File file(context_, filename, FileMode::FILE_WRITE);
		SaveXML(file);

		SendEvent(E_SCENESAVED);
	}

	Node* EditorScene::CreateNewNode(Node* parentNode)
	{
		auto newNode = MakeShared<Node>(context_);
		newNode->SetName("NewNode");
		newNode->SetPosition2D(0, 0);
		newNode->SetRotation2D(0);

		if (parentNode == nullptr)
		{
			AddChild(newNode);
		}
		else
		{
			parentNode->AddChild(newNode);
		}

		return newNode;
	}

	Node* EditorScene::GetNodeAt(Vector3 pos)
	{
		PODVector<Node*> nodes;
		GetChildren(nodes, true);

		for (auto node : nodes)
		{
			if (GetNodeWorldBoundingBox(node).IsInside(pos))
			{
				return node;
			}
		}

		return nullptr;
	}

	Node* EditorScene::GetNodeAt(Vector2 pos)
	{
		PODVector<Node*> nodes;
		GetChildren(nodes, true);

		for (auto node : nodes)
		{
			if (GetNodeWorldRect(node).IsInside(pos))
			{
				return node;
			}
		}

		return nullptr;
	}

	BoundingBox EditorScene::GetNodeWorldBoundingBox(Node* node)
	{
		BoundingBox bb;
		PODVector<Drawable*> drawables;
		node->GetDerivedComponents<Drawable>(drawables);

		for (auto drawable : drawables)
		{
			bb.Merge(drawable->GetWorldBoundingBox());
		}

		return bb;
	}

	Rect EditorScene::GetNodeWorldRect(Node* node)
	{
		auto bb = GetNodeWorldBoundingBox(node);
		return Rect(bb.min_.x_, bb.min_.y_, bb.max_.x_, bb.max_.y_);
	}

	int EditorScene::IndexOfNode(Node* parentNode, Node* childNode)
	{
		if (parentNode == nullptr)
		{
			return -1;
		}

		auto children = parentNode->GetChildren();

		for (int i = 0; i < children.Size(); i++)
		{
			if (children[i]->GetID() == childNode->GetID())
			{
				return i;
			}
		}

		return -1;
	}

	int EditorScene::IndexOfComponent(Node* parentNode, Component* component)
	{
		if (parentNode == nullptr)
		{
			return -1;
		}

		auto components = parentNode->GetComponents();

		for (int i = 0; i < components.Size(); i++)
		{
			if (components[i]->GetID() == component->GetID())
			{
				return i;
			}
		}

		return -1;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void EditorScene::HandleSceneNodeRemoved(StringHash, VariantMap& eventData)
	{
		auto node = eventData[NodeRemoved::P_NODE].GetPtr();

		if (node == selectedObject_)
		{
			ClearSelection();
		}
	}

	void EditorScene::HandleSceneComponentRemoved(StringHash, VariantMap& eventData)
	{
		auto component = eventData[ComponentRemoved::P_COMPONENT].GetPtr();

		if (component == selectedObject_)
		{
			ClearSelection();
		}
	}
}
