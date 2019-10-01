#include "HierarchyWindowView.h"

#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Core/CoreEvents.h>

using namespace Urho3D;

static const int NONE_SELECTED = M_MAX_UNSIGNED;
static const int LAST_POSITION = M_MAX_UNSIGNED;

static const String VAR_OBJECT_ID = "VAR_OBJECT_ID";
static const String VAR_OBJECT_PTR = "VAR_OBJECT_PTR";

namespace Geode
{
	HierarchyWindowView::HierarchyWindowView(Context* context, CommandHistory::Ptr commandHistory, EditorScene::Ptr editorScene) : IWindowView(context, "UI/HierarchyWindowView.xml")
	{
		commandHistory_ = commandHistory;
		editorScene_ = editorScene;

		hierarchyList_ = elRoot_->GetChildDynamicCast<ListView>("HierarchyList", true);
		assert(hierarchyList_);

		deleteButton_ = elRoot_->GetChildDynamicCast<Button>("DeleteButton", true);
		assert(deleteButton_);

		createNodeButton_ = elRoot_->GetChildDynamicCast<Button>("CreateNodeButton", true);
		assert(createNodeButton_);

		UpdateHierarchyList();
		SetSelectedObject(editorScene_->GetSelectedObject());

		SubscribeToEvent(hierarchyList_, E_ITEMSELECTED, URHO3D_HANDLER(HierarchyWindowView, HandleSelectedItem));
		SubscribeToEvent(editorScene_, E_SCENELOADED, URHO3D_HANDLER(HierarchyWindowView, HandleSceneLoaded));
		SubscribeToEvent(editorScene_, E_SELECTEDOBJECTCHANGED, URHO3D_HANDLER(HierarchyWindowView, HandleSelectedObjectChanged));
		SubscribeToEvent(editorScene_, E_NODENAMECHANGED, URHO3D_HANDLER(HierarchyWindowView, HandleSceneNodeNameChanged));
		SubscribeToEvent(editorScene_, E_NODEADDED, URHO3D_HANDLER(HierarchyWindowView, HandleSceneNodeAdded));
		SubscribeToEvent(editorScene_, E_NODEREMOVED, URHO3D_HANDLER(HierarchyWindowView, HandleSceneNodeRemoved));
		SubscribeToEvent(editorScene_, E_COMPONENTADDED, URHO3D_HANDLER(HierarchyWindowView, HandleSceneComponentAdded));
		SubscribeToEvent(editorScene_, E_COMPONENTREMOVED, URHO3D_HANDLER(HierarchyWindowView, HandleSceneComponentRemoved));
		SubscribeToEvent(deleteButton_, E_RELEASED, URHO3D_HANDLER(HierarchyWindowView, HandleDeleteButtonReleased));
		SubscribeToEvent(createNodeButton_, E_RELEASED, URHO3D_HANDLER(HierarchyWindowView, HandleCreateNodeButtonReleased));
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void HierarchyWindowView::HandleSceneLoaded(StringHash, VariantMap&)
	{
		UpdateHierarchyList();
	}

	void HierarchyWindowView::HandleSelectedObjectChanged(StringHash, VariantMap&)
	{
		auto selectedObject = editorScene_->GetSelectedObject();
		auto selectedItem = hierarchyList_->GetSelectedItem();

		if (selectedItem && selectedItem->GetVar(VAR_OBJECT_PTR).GetPtr() == selectedObject)
		{
			return;
		}

		SetSelectedObject(selectedObject);
	}

	void HierarchyWindowView::HandleSelectedItem(StringHash, VariantMap&)
	{
		auto selectedItem = hierarchyList_->GetSelectedItem();
		auto selectedItemObject = static_cast<Object*>(selectedItem->GetVar(VAR_OBJECT_PTR).GetPtr());

		if (selectedItemObject == nullptr)
		{
			return;
		}

		if (selectedItemObject->IsInstanceOf<Scene>())
		{
			editorScene_->ClearSelection();
			return;
		}

		if (selectedItemObject->IsInstanceOf<Node>())
		{
			editorScene_->SetSelectedObject(static_cast<Node*>(selectedItemObject));
		}

		if (selectedItemObject->IsInstanceOf<Component>())
		{
			editorScene_->SetSelectedObject(static_cast<Component*>(selectedItemObject));
		}
	}

	void HierarchyWindowView::HandleSceneNodeNameChanged(StringHash, VariantMap& eventData)
	{
		auto node = static_cast<Node*>(eventData[NodeNameChanged::P_NODE].GetPtr());
		auto foundedItem = FindItem(node);

		if (foundedItem != nullptr)
		{
			auto text = static_cast<Text*>(foundedItem);
			text->SetText(node->GetName());
		}
	}

	void HierarchyWindowView::HandleSceneNodeAdded(StringHash, VariantMap& eventData)
	{
		UpdateHierarchyList();
	}

	void HierarchyWindowView::HandleSceneNodeRemoved(StringHash, VariantMap& eventData)
	{
		auto node = static_cast<Node*>(eventData[NodeRemoved::P_NODE].GetPtr());
		removedNodesBuf_.Push(node);

		UpdateHierarchyList();
	}

	void HierarchyWindowView::HandleSceneComponentAdded(StringHash, VariantMap& eventData)
	{
		UpdateHierarchyList();
	}

	void HierarchyWindowView::HandleSceneComponentRemoved(StringHash, VariantMap& eventData)
	{
		auto component = static_cast<Component*>(eventData[ComponentRemoved::P_COMPONENT].GetPtr());
		removedComponentsBuf_.Push(component);

		UpdateHierarchyList();
	}

	void HierarchyWindowView::HandleDeleteButtonReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<DeleteCommand>(editorScene_), commandHistory_).Exec();
	}

	void HierarchyWindowView::HandleCreateNodeButtonReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<NewNodeCommand>(editorScene_), commandHistory_).Exec();
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void HierarchyWindowView::SetSelectedObject(Object* object)
	{
		if (object == nullptr)
		{
			hierarchyList_->ClearSelection();
		}

		int index = IndexOfObject(object);

		if (index == -1)
		{
			hierarchyList_->ClearSelection();
		}
		else
		{
			hierarchyList_->SetSelection(index);
		}
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	void HierarchyWindowView::UpdateHierarchyList()
	{
		hierarchyList_->RemoveAllItems();
		AddNode(editorScene_, nullptr);

		removedNodesBuf_.Clear();
		removedComponentsBuf_.Clear();
	}

	UIElement* HierarchyWindowView::AddNode(Node* node, UIElement* parentItem)
	{
		if (removedNodesBuf_.Contains(node))
		{
			return nullptr;
		}

		auto isScene = node->IsInstanceOf<Scene>();
		auto nodeItem = MakeShared<Text>(context_);
		hierarchyList_->InsertItem(LAST_POSITION, nodeItem, parentItem);	

		nodeItem->SetStyle("FileSelectorListText");
		nodeItem->SetText(isScene ? "Scene" : node->GetName());
		nodeItem->SetVar(VAR_OBJECT_PTR, node);
		nodeItem->SetVar(VAR_OBJECT_ID, node->GetID());

		if (!isScene)
		{
			for (auto component : node->GetComponents())
			{
				AddComponent(component, nodeItem);
			}
		}

		for (auto child : node->GetChildren())
		{
			AddNode(child, nodeItem);
		}

		return nodeItem;
	}

	UIElement* HierarchyWindowView::AddComponent(Component* component, UIElement* parentItem)
	{
		if (removedComponentsBuf_.Contains(component))
		{
			return nullptr;
		}

		auto item = MakeShared<Text>(context_);
		hierarchyList_->InsertItem(LAST_POSITION, item, parentItem);

		item->SetStyle("FileSelectorListText");
		item->SetText(component->GetTypeName());
		item->SetVar(VAR_OBJECT_PTR, component);
		item->SetVar(VAR_OBJECT_ID, component->GetID());
		item->SetColor(Color::YELLOW);

		return item;
	}

	UIElement* HierarchyWindowView::FindItem(Object* object)
	{
		for (auto item : hierarchyList_->GetItems())
		{
			if (object == item->GetVar(VAR_OBJECT_PTR).GetPtr())
			{
				return item;
			}
		}

		return nullptr;
	}

	int HierarchyWindowView::IndexOfObject(Object* object)
	{
		for (int i = 0; i < hierarchyList_->GetNumItems(); i++)
		{
			auto item = hierarchyList_->GetItem(i);

			if (object == item->GetVar(VAR_OBJECT_PTR).GetPtr())
			{
				return i;
			}
		}

		return -1;
	}
}
