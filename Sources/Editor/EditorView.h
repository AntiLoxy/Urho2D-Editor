/**
 * @file    Editor.h
 * @ingroup Geode
 * @brief   The Editor module entry point.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "EditorScene.h"
#include "Commands.h"
#include "ToolsView.h"
#include "PanelView.h"
#include "SceneView.h"
#include "AttributeWindowView.h"
#include "HierarchyWindowView.h"
#include "../Gui/IView.h"
#include "../Gui/MenuBar.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/FileSelector.h>

namespace Geode
{
	class EditorView : public Geode::IView
	{
		URHO3D_OBJECT(EditorView, Geode::IView)

	public:
		using Ptr = Urho3D::SharedPtr<EditorView>;

	public:
		/// Constructors.
		explicit EditorView(Urho3D::Context* context);

	private:
		/// Event handlers.
		void HandleFileMenuOpenReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleFileMenuSaveReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleFileMenuQuitReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleEditMenuUndoReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleEditMenuDeleteReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleEditMenuUnselectReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleEditMenuCreateNewNodeReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleEditMenuCreateStaticSprite2DReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleEditMenuCreateRigidBody2DReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleEditMenuCreateCollisionBox2DReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleEditMenuCreateCollisionPolygon2DReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleEditMenuCreateCollisionCircle2DReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleViewMenuAttributeWindowReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleViewMenuHierarchyWindowReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleViewMenuCloseAllWindowReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleViewMenuOpenAllWindowReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleViewMenuResetWindowLayoutReleased(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleViewMenuDebugGeometryEnabledToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleViewMenuNodePositionGizmoEnabledToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleViewMenuGridEnabledToggled(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleOpenSceneFileSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSaveSceneFileSelected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		
		/// Other methods.
		void ResetWindowLayout();
		void CreateOpenSceneFileSelector();
		void CreateSaveSceneFileSelector();

	private:
		Urho3D::SharedPtr<Geode::EditorScene> editorScene_;
		Urho3D::SharedPtr<Geode::CommandHistory> commandHistory_;

		Urho3D::UIElement* topBlock_;
		Urho3D::UIElement* middleBlock_;
		Urho3D::UIElement* bottomBlock_;

		Urho3D::SharedPtr<Geode::MenuBar> menuBar_;
		Urho3D::SharedPtr<Geode::ToolsView> toolsView_;
		Urho3D::SharedPtr<Geode::AttributeWindowView> attributeWindowView_;
		Urho3D::SharedPtr<Geode::HierarchyWindowView> hierarchyWindowView_;
		Urho3D::SharedPtr<Geode::PanelView> panelView_;
		Urho3D::SharedPtr<Geode::SceneView> sceneView_;
		Urho3D::SharedPtr<Urho3D::FileSelector> fileSelector_;

		Geode::FlyMenu* fileMenu_;
		Urho3D::Button* fileMenuOpenButton_;
		Urho3D::Button* fileMenuSaveButton_;
		Urho3D::Button* fileMenuQuitButton_;
		Geode::FlyMenu* editMenu_;
		Urho3D::Button* editMenuUndoButton_;
		Urho3D::Button* editMenuDeleteButton_;
		Urho3D::Button* editMenuUnselectButton_;
		Urho3D::Button* editMenuNewNodeButton_;
		Urho3D::Button* editMenuCreateStaticSprite2DButton_;
		Urho3D::Button* editMenuCreateRigidBody2DButton_;
		Urho3D::Button* editMenuCreateCollisionBox2DButton_;
		Urho3D::Button* editMenuCreateCollisionPolygon2DButton_;
		Urho3D::Button* editMenuCreateCollisionCircle2DButton_;
		Geode::FlyMenu* viewMenu_;
		Urho3D::Button* viewMenuAttributeWindowButton_;
		Urho3D::Button* viewMenuHierarchyWindowButton_;
		Urho3D::Button* viewMenuCloseAllWindowButton_;
		Urho3D::Button* viewMenuOpenAllWindowButton_;
		Urho3D::Button* viewMenuResetWindowLayoutButton_;

		Urho3D::CheckBox* viewMenuDebugGeometryEnabledCheckBox_;
		Urho3D::CheckBox* viewMenuNodePositionGizmoEnabledCheckBox_;
		Urho3D::CheckBox* viewMenuGridEnabledCheckBox_;
	};
}
