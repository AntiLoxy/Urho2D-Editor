#include "Commands.h"

using namespace Urho3D;

namespace Geode
{
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  ICOMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	ICommand::ICommand()
	{
	}

	void ICommand::Undo()
	{
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  COMMAND HISTORY
	///////////////////////////////////////////////////////////////////////////////////////////////////
	CommandHistory::CommandHistory(unsigned int limit)
	{
		limit_ = limit;
		currentIndex_ = -1;
		history_.Resize(limit);
	}

	void CommandHistory::Push(ICommand::Ptr commandPtr)
	{
		currentIndex_++;
		if (currentIndex_ >= limit_)
		{
			currentIndex_ = 0;
		}

		if (history_[currentIndex_] != nullptr)
		{
			history_[currentIndex_].Reset();
		}
		
		history_[currentIndex_] = commandPtr;
	}

	ICommand::Ptr CommandHistory::Pop()
	{
		if (currentIndex_ == -1)
		{
			return nullptr;
		}

		auto lastCommand = history_[currentIndex_];
		history_[currentIndex_].Reset();

		currentIndex_--;
		if (currentIndex_ < 0)
		{
			currentIndex_ = limit_ - 1;
		}

		return lastCommand;
	}

	void CommandHistory::Clear()
	{
		history_.Clear();
		history_.Resize(limit_);
		currentIndex_ = -1;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  COMMAND INVOKER
	///////////////////////////////////////////////////////////////////////////////////////////////////
	CommandInvoker::CommandInvoker(ICommand::Ptr command, CommandHistory::Ptr commandHistory)
	{
		command_ = command;
		commandHistory_ = commandHistory;
	}

	void CommandInvoker::Exec()
	{
		if (command_->Exec())
		{
			commandHistory_->Push(command_);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  UNDO COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	UndoCommand::UndoCommand(CommandHistory::Ptr commandHistory)
	{
		commandHistory_ = commandHistory;
	}

	bool UndoCommand::Exec()
	{
		auto lastCommand = commandHistory_->Pop();

		if (lastCommand != nullptr)
		{
			lastCommand->Undo();
		}
		
		return false;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  UNSELECT COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	UnselectCommand::UnselectCommand(EditorScene::Ptr editorScene)
	{
		editorScene_ = editorScene;
	}

	bool UnselectCommand::Exec()
	{
		editorScene_->ClearSelection();
		return false;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  NEW NODE COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	NewNodeCommand::NewNodeCommand(EditorScene::Ptr editorScene)
	{
		editorScene_ = editorScene;
		createdNode_ = nullptr;
	}

	bool NewNodeCommand::Exec()
	{
		auto selectedNode = editorScene_->GetSelectedNode();
		createdNode_ = editorScene_->CreateNewNode(selectedNode);
		return true;
	}

	void NewNodeCommand::Undo()
	{
		if (createdNode_ != nullptr)
		{
			createdNode_->Remove();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  DELETE COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	DeleteCommand::DeleteCommand(EditorScene::Ptr editorScene)
	{
		editorScene_ = editorScene;
		parentNode_ = nullptr;
		deletedNode_ = nullptr;
		deletedComponent_ = nullptr;
	}

	bool DeleteCommand::Exec()
	{
		auto selectedObject = editorScene_->GetSelectedObject();

		if (selectedObject == nullptr)
		{
			return false;
		}

		if (selectedObject->IsInstanceOf<Scene>())
		{
			return false;
		}

		if (selectedObject->IsInstanceOf<Node>())
		{
			auto selectedNode = static_cast<Node*>(selectedObject);

			parentNode_ = selectedNode->GetParent();
			nodeIndex_ = editorScene_->IndexOfNode(parentNode_, selectedNode);
			deletedNode_ = selectedNode;
			deletedNode_->Remove();
		}
		else if (selectedObject->IsInstanceOf<Component>())
		{
			auto selectedComponent = static_cast<Component*>(selectedObject);

			parentNode_ = selectedComponent->GetNode();
			deletedComponent_ = selectedComponent;
			deletedComponent_->Remove();
		}
		else
		{
			return false;
		}

		return true;
	}

	void DeleteCommand::Undo()
	{
		if (parentNode_ == nullptr)
		{
			return;
		}

		if (deletedNode_ != nullptr)
		{
			parentNode_->AddChild(deletedNode_, nodeIndex_);
		}
		else if (deletedComponent_ != nullptr)
		{
			parentNode_->AddComponent(deletedComponent_, deletedComponent_->GetID(), CreateMode::LOCAL);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  MOVE NODE COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	MoveNodeCommand::MoveNodeCommand(EditorScene::Ptr editorScene, Vector2 previousPosition, Vector2 position)
	{
		editorScene_ = editorScene;
		previousPosition_ = previousPosition;
		position_ = position;
		movedNode_ = nullptr;
	}

	bool MoveNodeCommand::Exec()
	{
		auto selectedNode = editorScene_->GetSelectedNode();

		if (selectedNode == nullptr)
		{
			return false;
		}

		selectedNode->SetWorldPosition2D(position_);
		movedNode_ = selectedNode;
		return true;
	}

	void MoveNodeCommand::Undo()
	{
		if (movedNode_ != nullptr)
		{
			movedNode_->SetWorldPosition2D(previousPosition_);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  SCALE NODE COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	ScaleNodeCommand::ScaleNodeCommand(EditorScene::Ptr editorScene, Vector2 previousScale, Vector2 scale)
	{
		editorScene_ = editorScene;
		previousScale_ = previousScale;
		scale_ = scale;
		scaledNode_ = nullptr;
	}

	bool ScaleNodeCommand::Exec()
	{
		auto selectedNode = editorScene_->GetSelectedNode();

		if (selectedNode == nullptr)
		{
			return false;
		}

		selectedNode->SetWorldScale2D(scale_);
		scaledNode_ = selectedNode;
		return true;
	}

	void ScaleNodeCommand::Undo()
	{
		if (scaledNode_ != nullptr)
		{
			scaledNode_->SetWorldScale2D(previousScale_);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  ROTATE NODE COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	RotateNodeCommand::RotateNodeCommand(EditorScene::Ptr editorScene, float previousRotation, float rotation)
	{
		editorScene_ = editorScene;
		previousRotation_ = previousRotation;
		rotation_ = rotation;
		rotatedNode_ = nullptr;
	}

	bool RotateNodeCommand::Exec()
	{
		auto selectedNode = editorScene_->GetSelectedNode();

		if (selectedNode == nullptr)
		{
			return false;
		}

		selectedNode->SetWorldRotation2D(rotation_);
		rotatedNode_ = selectedNode;
		return true;
	}

	void RotateNodeCommand::Undo()
	{
		if (rotatedNode_ != nullptr)
		{
			rotatedNode_->SetWorldRotation2D(previousRotation_);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CREATE STATICSPRITE2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	CreateStaticSprite2DCommand::CreateStaticSprite2DCommand(EditorScene::Ptr editorScene)
	{
		editorScene_ = editorScene;
		createdComponent_ = nullptr;
	}

	bool CreateStaticSprite2DCommand::Exec()
	{
		auto selectedNode = editorScene_->GetSelectedNode();

		if (selectedNode == nullptr)
		{
			return false;
		}

		createdComponent_ = selectedNode->CreateComponent<StaticSprite2D>();
		return true;
	}

	void CreateStaticSprite2DCommand::Undo()
	{
		if (createdComponent_ != nullptr)
		{
			createdComponent_->Remove();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CREATE RIGIBODY2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	CreateRigidBody2DCommand::CreateRigidBody2DCommand(EditorScene::Ptr editorScene)
	{
		editorScene_ = editorScene;
		createdComponent_ = nullptr;
	}

	bool CreateRigidBody2DCommand::Exec()
	{
		auto selectedNode = editorScene_->GetSelectedNode();

		if (selectedNode == nullptr)
		{
			return false;
		}

		createdComponent_ = selectedNode->CreateComponent<RigidBody2D>();
		return true;
	}

	void CreateRigidBody2DCommand::Undo()
	{
		if (createdComponent_ != nullptr)
		{
			createdComponent_->Remove();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CREATE COLLISIONBOX2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	CreateCollisionBox2DCommand::CreateCollisionBox2DCommand(EditorScene::Ptr editorScene)
	{
		editorScene_ = editorScene;
		createdComponent_ = nullptr;
	}

	bool CreateCollisionBox2DCommand::Exec()
	{
		auto selectedNode = editorScene_->GetSelectedNode();

		if (selectedNode == nullptr)
		{
			return false;
		}

		createdComponent_ = selectedNode->CreateComponent<CollisionBox2D>();
		createdComponent_->SetCenter(0, 0);
		createdComponent_->SetSize(1, 1);
		return true;
	}

	void CreateCollisionBox2DCommand::Undo()
	{
		if (createdComponent_ != nullptr)
		{
			createdComponent_->Remove();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CREATE COLLISIONPOLYGON2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	CreateCollisionPolygon2DCommand::CreateCollisionPolygon2DCommand(EditorScene::Ptr editorScene)
	{
		editorScene_ = editorScene;
		createdComponent_ = nullptr;
	}

	bool CreateCollisionPolygon2DCommand::Exec()
	{
		auto selectedNode = editorScene_->GetSelectedNode();

		if (selectedNode == nullptr)
		{
			return false;
		}

		createdComponent_ = selectedNode->CreateComponent<CollisionPolygon2D>();
		return true;
	}

	void CreateCollisionPolygon2DCommand::Undo()
	{
		if (createdComponent_ != nullptr)
		{
			createdComponent_->Remove();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CREATE COLLISIONCIRCLE2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	CreateCollisionCircle2DCommand::CreateCollisionCircle2DCommand(EditorScene::Ptr editorScene)
	{
		editorScene_ = editorScene;
		createdComponent_ = nullptr;
	}

	bool CreateCollisionCircle2DCommand::Exec()
	{
		auto selectedNode = editorScene_->GetSelectedNode();

		if (selectedNode == nullptr)
		{
			return false;
		}

		createdComponent_ = selectedNode->CreateComponent<CollisionCircle2D>();
		createdComponent_->SetCenter(0, 0);
		createdComponent_->SetRadius(1);
		return true;
	}

	void CreateCollisionCircle2DCommand::Undo()
	{
		if (createdComponent_ != nullptr)
		{
			createdComponent_->Remove();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  ADD VERTEX COLLISIONPOLYGON2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	AddVertexCollisionPolygon2DCommand::AddVertexCollisionPolygon2DCommand(EditorScene::Ptr editorScene, Vector2 vertex)
	{
		editorScene_ = editorScene;
		component_ = nullptr;
		vertex_ = vertex;
	}

	bool AddVertexCollisionPolygon2DCommand::Exec()
	{
		auto component = editorScene_->GetSelectedObjectAs<CollisionPolygon2D>();

		if (component == nullptr)
		{
			return false;
		}

		auto vertices = component->GetVertices();
		
		if (vertices.Size() >= 8)
		{
			return false;
		}

		vertices.Push(vertex_);
		component->SetVertices(vertices);

		component_ = component;
		return true;
	}

	void AddVertexCollisionPolygon2DCommand::Undo()
	{
		if (component_ != nullptr)
		{
			auto vertices = component_->GetVertices();
			vertices.Pop();
			component_->SetVertices(vertices);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  MOVE VERTEX COLLISIONPOLYGON2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	MoveVertexCollisionPolygon2DCommand::MoveVertexCollisionPolygon2DCommand(EditorScene::Ptr editorScene, unsigned int index, Vector2 previousVertex, Vector2 vertex)
	{
		editorScene_ = editorScene;
		component_ = nullptr;
		index_ = index;
		previousVertex_ = previousVertex;
		vertex_ = vertex;
	}

	bool MoveVertexCollisionPolygon2DCommand::Exec()
	{
		auto component = editorScene_->GetSelectedObjectAs<CollisionPolygon2D>();

		if (component == nullptr)
		{
			return false;
		}

		if (index_ >= component->GetVertexCount())
		{
			return false;
		}

		auto vertices = component->GetVertices();
		vertices[index_] = vertex_;
		component->SetVertices(vertices);
		component_ = component;
		return true;
	}

	void MoveVertexCollisionPolygon2DCommand::Undo()
	{
		if (component_ != nullptr)
		{
			auto vertices = component_->GetVertices();
			vertices[index_] = previousVertex_;
			component_->SetVertices(vertices);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  DELETE VERTEX COLLISIONPOLYGON2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	DeleteVertexCollisionPolygon2DCommand::DeleteVertexCollisionPolygon2DCommand(EditorScene::Ptr editorScene, unsigned int index)
	{
		editorScene_ = editorScene;
		component_ = nullptr;
		index_ = index;
	}

	bool DeleteVertexCollisionPolygon2DCommand::Exec()
	{
		auto component = editorScene_->GetSelectedObjectAs<CollisionPolygon2D>();

		if (component == nullptr)
		{
			return false;
		}

		if (index_ >= component->GetVertexCount())
		{
			return false;
		}

		vertex_ = component->GetVertex(index_);
		auto vertices = component->GetVertices();
		vertices.Erase(index_);
		component->SetVertices(vertices);
		component_ = component;
		return true;
	}

	void DeleteVertexCollisionPolygon2DCommand::Undo()
	{
		if (component_ != nullptr)
		{
			auto vertices = component_->GetVertices();
			vertices.Insert(index_, vertex_);
			component_->SetVertices(vertices);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CHANGE SIZE CENTER COLLISIONBOX2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	ChangeSizeCenterCollisionBox2DCommand::ChangeSizeCenterCollisionBox2DCommand(EditorScene::Ptr editorScene, Vector2 previousSize, Vector2 previousCenter, Vector2 size, Vector2 center)
	{
		editorScene_ = editorScene;
		component_ = nullptr;
		previousSize_ = previousSize;
		previousCenter_ = previousCenter;
		size_ = size;
		center_ = center;
	}

	bool ChangeSizeCenterCollisionBox2DCommand::Exec()
	{
		auto component = editorScene_->GetSelectedObjectAs<CollisionBox2D>();

		if (component == nullptr)
		{
			return false;
		}

		component->SetSize(size_);
		component->SetCenter(center_);
		component_ = component;
		return true;
	}

	void ChangeSizeCenterCollisionBox2DCommand::Undo()
	{
		if (component_ != nullptr)
		{
			component_->SetSize(previousSize_);
			component_->SetCenter(previousCenter_);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CHANGE RADIUS COLLISIONBOX2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	ChangeRadiusCollisionCircle2DCommand::ChangeRadiusCollisionCircle2DCommand(EditorScene::Ptr editorScene, float previousRadius, float radius)
	{
		editorScene_ = editorScene;
		component_ = nullptr;
		previousRadius_ = previousRadius;
		radius_ = radius;
	}

	bool ChangeRadiusCollisionCircle2DCommand::Exec()
	{
		auto component = editorScene_->GetSelectedObjectAs<CollisionCircle2D>();

		if (component == nullptr)
		{
			return false;
		}

		component->SetRadius(radius_);
		component_ = component;
		return true;
	}

	void ChangeRadiusCollisionCircle2DCommand::Undo()
	{
		if (component_ != nullptr)
		{
			component_->SetRadius(previousRadius_);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CHANGE CENTER COLLISIONBOX2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	ChangeCenterCollisionCircle2DCommand::ChangeCenterCollisionCircle2DCommand(EditorScene::Ptr editorScene, Vector2 previousCenter, Vector2 center)
	{
		editorScene_ = editorScene;
		component_ = nullptr;
		previousCenter_ = previousCenter;
		center_ = center;
	}

	bool ChangeCenterCollisionCircle2DCommand::Exec()
	{
		auto component = editorScene_->GetSelectedObjectAs<CollisionCircle2D>();

		if (component == nullptr)
		{
			return false;
		}

		component->SetCenter(center_);
		component_ = component;
		return true;
	}

	void ChangeCenterCollisionCircle2DCommand::Undo()
	{
		if (component_ != nullptr)
		{
			component_->SetCenter(previousCenter_);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CHANGE ATTRIBUTE SERIALIZABLE
	///////////////////////////////////////////////////////////////////////////////////////////////////
	ChangeAttributeSerializableCommand::ChangeAttributeSerializableCommand(EditorScene::Ptr editorScene, String name, Variant previousValue, Variant value)
	{
		editorScene_ = editorScene;
		serializable_ = nullptr;
		previousValue_ = previousValue;
		value_ = value;
		name_ = name;
	}

	bool ChangeAttributeSerializableCommand::Exec()
	{
		auto serializable = editorScene_->GetSelectedObjectAs<Serializable>();

		if (serializable == nullptr)
		{
			return false;
		}

		serializable->SetAttribute(name_, value_);
		serializable_ = serializable;
		return true;
	}

	void ChangeAttributeSerializableCommand::Undo()
	{
		if (serializable_ != nullptr)
		{
			serializable_->SetAttribute(name_, previousValue_);
		}
	}
}