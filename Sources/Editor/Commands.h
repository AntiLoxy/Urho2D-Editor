/**
 * @file    Commands.h
 * @ingroup Editor
 * @brief   Contains all commands class.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

//@todo: fixe issue when undo remove command on root child node.

#pragma once

#include "EditorScene.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/CollisionPolygon2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>

namespace Geode
{
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  ICOMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class ICommand : public Urho3D::RefCounted
	{
	public:
		using Ptr = Urho3D::SharedPtr<ICommand>;

	public:
		ICommand();
		virtual bool Exec() = 0;
		virtual void Undo();
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  COMMAND HISTORY
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class CommandHistory : public Urho3D::RefCounted
	{
	public:
		using Ptr = Urho3D::SharedPtr<CommandHistory>;

	public:
		explicit CommandHistory(unsigned int limit);
		void Push(Geode::ICommand::Ptr commandPtr);
		Geode::ICommand::Ptr Pop();
		void Clear();

	private:
		unsigned int limit_;
		int currentIndex_;
		Urho3D::Vector<Geode::ICommand::Ptr> history_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  COMMAND INVOKER
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class CommandInvoker
	{
	public:
		explicit CommandInvoker(Geode::ICommand::Ptr command, Geode::CommandHistory::Ptr commandHistory);
		void Exec();

	private:
		Geode::ICommand::Ptr command_;
		Geode::CommandHistory::Ptr commandHistory_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  UNDO COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class UndoCommand : public ICommand
	{
	public:
		explicit UndoCommand(Geode::CommandHistory::Ptr commandHistory);
		bool Exec() override;

	private:
		Geode::CommandHistory::Ptr commandHistory_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  UNSELECT COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class UnselectCommand : public ICommand
	{
	public:
		explicit UnselectCommand(Geode::EditorScene::Ptr editorScene);
		bool Exec() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  NEW NODE COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class NewNodeCommand : public ICommand
	{
	public:
		explicit NewNodeCommand(Geode::EditorScene::Ptr editorScene);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::Node* createdNode_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  DELETE COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class DeleteCommand : public ICommand
	{
	public:
		explicit DeleteCommand(Geode::EditorScene::Ptr editorScene);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		unsigned int nodeIndex_;
		Urho3D::SharedPtr<Urho3D::Node> parentNode_;
		Urho3D::SharedPtr<Urho3D::Node> deletedNode_;
		Urho3D::SharedPtr<Urho3D::Component> deletedComponent_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  MOVE NODE COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class MoveNodeCommand : public ICommand
	{
	public:
		explicit MoveNodeCommand(Geode::EditorScene::Ptr editorScene, Urho3D::Vector2 previousPosition, Urho3D::Vector2 position);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::Vector2 previousPosition_;
		Urho3D::Vector2 position_;
		Urho3D::Node* movedNode_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  SCALE NODE COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class ScaleNodeCommand : public ICommand
	{
	public:
		explicit ScaleNodeCommand(Geode::EditorScene::Ptr editorScene, Urho3D::Vector2 previousScale, Urho3D::Vector2 scale);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::Vector2 previousScale_;
		Urho3D::Vector2 scale_;
		Urho3D::Node* scaledNode_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  ROTATE NODE COMMAND
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class RotateNodeCommand : public ICommand
	{
	public:
		explicit RotateNodeCommand(Geode::EditorScene::Ptr editorScene, float previousRotation, float rotation);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		float previousRotation_;
		float rotation_;
		Urho3D::Node* rotatedNode_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CREATE STATICSPRITE2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class CreateStaticSprite2DCommand : public ICommand
	{
	public:
		explicit CreateStaticSprite2DCommand(Geode::EditorScene::Ptr editorScene);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::StaticSprite2D* createdComponent_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CREATE RIGIDBODY2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class CreateRigidBody2DCommand : public ICommand
	{
	public:
		explicit CreateRigidBody2DCommand(Geode::EditorScene::Ptr editorScene);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::RigidBody2D* createdComponent_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CREATE COLLISIONBOX2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class CreateCollisionBox2DCommand : public ICommand
	{
	public:
		explicit CreateCollisionBox2DCommand(Geode::EditorScene::Ptr editorScene);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::CollisionBox2D* createdComponent_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CREATE COLLISIONPOLYGON2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class CreateCollisionPolygon2DCommand : public ICommand
	{
	public:
		explicit CreateCollisionPolygon2DCommand(Geode::EditorScene::Ptr editorScene);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::CollisionPolygon2D* createdComponent_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CREATE COLLISIONCIRCLE2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class CreateCollisionCircle2DCommand : public ICommand
	{
	public:
		explicit CreateCollisionCircle2DCommand(Geode::EditorScene::Ptr editorScene);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::CollisionCircle2D* createdComponent_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  ADD VERTEX COLLISIONPOLYGON2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class AddVertexCollisionPolygon2DCommand : public ICommand
	{
	public:
		explicit AddVertexCollisionPolygon2DCommand(Geode::EditorScene::Ptr editorScene, Urho3D::Vector2 vertex);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::CollisionPolygon2D* component_;
		Urho3D::Vector2 vertex_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  MOVE VERTEX COLLISIONPOLYGON2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class MoveVertexCollisionPolygon2DCommand : public ICommand
	{
	public:
		explicit MoveVertexCollisionPolygon2DCommand(Geode::EditorScene::Ptr editorScene, unsigned int index, Urho3D::Vector2 previousVertex, Urho3D::Vector2 vertex);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::CollisionPolygon2D* component_;
		Urho3D::Vector2 previousVertex_;
		Urho3D::Vector2 vertex_;
		unsigned int index_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  DELETE VERTEX COLLISIONPOLYGON2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class DeleteVertexCollisionPolygon2DCommand : public ICommand
	{
	public:
		explicit DeleteVertexCollisionPolygon2DCommand(Geode::EditorScene::Ptr editorScene, unsigned int index);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::CollisionPolygon2D* component_;
		Urho3D::Vector2 vertex_;
		unsigned int index_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CHANGE SIZE CENTER COLLISIONBOX2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class ChangeSizeCenterCollisionBox2DCommand : public ICommand
	{
	public:
		explicit ChangeSizeCenterCollisionBox2DCommand(Geode::EditorScene::Ptr editorScene, Urho3D::Vector2 previousSize, Urho3D::Vector2 previousCenter, Urho3D::Vector2 size, Urho3D::Vector2 center);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::CollisionBox2D* component_;
		Urho3D::Vector2 previousSize_;
		Urho3D::Vector2 previousCenter_;
		Urho3D::Vector2 size_;
		Urho3D::Vector2 center_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CHANGE RADIUS COLLISIONCIRCLE2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class ChangeRadiusCollisionCircle2DCommand : public ICommand
	{
	public:
		explicit ChangeRadiusCollisionCircle2DCommand(Geode::EditorScene::Ptr editorScene, float previousRadius, float radius);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::CollisionCircle2D* component_;
		float previousRadius_;
		float radius_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CHANGE CENTER COLLISIONCIRCLE2D
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class ChangeCenterCollisionCircle2DCommand : public ICommand
	{
	public:
		explicit ChangeCenterCollisionCircle2DCommand(Geode::EditorScene::Ptr editorScene, Urho3D::Vector2 previousCenter, Urho3D::Vector2 center);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::CollisionCircle2D* component_;
		Urho3D::Vector2 previousCenter_;
		Urho3D::Vector2 center_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	///  CHANGE ATTRIBUTE SERIALIZABLE
	///////////////////////////////////////////////////////////////////////////////////////////////////
	class ChangeAttributeSerializableCommand : public ICommand
	{
	public:
		explicit ChangeAttributeSerializableCommand(Geode::EditorScene::Ptr editorScene, Urho3D::String name, Urho3D::Variant previousValue, Urho3D::Variant value);
		bool Exec() override;
		void Undo() override;

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::Serializable* serializable_;
		Urho3D::Variant previousValue_;
		Urho3D::Variant value_;
		Urho3D::String name_;
	};
}