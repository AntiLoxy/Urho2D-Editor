#include "EditorView.h"
#include "Commands.h"
#include "SeparatorTool.h"
#include "MoveTool.h"
#include "RotateTool.h"
#include "ScaleTool.h"
#include "CollisionPolygon2DTool.h"
#include "CollisionBox2DTool.h"
#include "CollisionCircle2DTool.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Engine/Engine.h>

using namespace Urho3D;

static const unsigned int HISTORY_LIMIT = 4;

static const String FILE_MENU_TEXT = "File";
static const String FILE_MENU_OPEN_TEXT = "Open Scene";
static const String FILE_MENU_SAVE_TEXT = "Save Scene";
static const String FILE_MENU_QUIT_TEXT = "Quit";

static const String EDIT_MENU_TEXT = "Edit";
static const String EDIT_MENU_UNDO_TEXT = "Undo";
static const String EDIT_MENU_DELETE_TEXT = "Delete";
static const String EDIT_MENU_UNSELECT_TEXT = "Unselect";
static const String EDIT_MENU_NEW_NODE_TEXT = "New Node";
static const String EDIT_MENU_CREATE_STATICSPRITE_TEXT = "Create StaticSprite2D";
static const String EDIT_MENU_CREATE_RIGIDBODY2D_TEXT = "Create RigidBody2D";
static const String EDIT_MENU_CREATE_COLLISIONBOX2D_TEXT = "Create CollisionBox2D";
static const String EDIT_MENU_CREATE_COLLISIONPOLYGON2D_TEXT = "Create CollisionPolygon2D";
static const String EDIT_MENU_CREATE_COLLISIONCIRCLE2D_TEXT = "Create CollisionCircle2D";

static const String VIEW_MENU_TEXT = "View";
static const String VIEW_MENU_ATTRIBUTE_WINDOW_TEXT = "Attribute Window";
static const String VIEW_MENU_HIERARCHY_WINDOW_TEXT = "Hierarchy Window";
static const String VIEW_MENU_CLOSE_ALL_WINDOW_TEXT = "Close All Window";
static const String VIEW_MENU_OPEN_ALL_WINDOW_TEXT = "Open All Window";
static const String VIEW_MENU_RESET_WINDOW_LAYOUT_TEXT = "Reset Window Layout";
static const String VIEW_MENU_DEBUG_GEOMETRY_ENABLED_TEXT = "Enabled Debug Geometry";
static const String VIEW_MENU_NODE_POSITION_GIZMO_ENABLED_TEXT = "Enabled Node Position Gizmo";
static const String VIEW_MENU_GRID_ENABLED_TEXT = "Enabled Grid";

namespace Geode
{
	EditorView::EditorView(Context* context) : IView(context, "UI/EditorView.xml")
	{
		elRoot_->SetFixedWidth(uiRoot_->GetWidth());
		elRoot_->SetFixedHeight(uiRoot_->GetHeight());

		// Init layout blocks.
		// ----------------------------------------------------------------------------------------------------------------
		topBlock_ = elRoot_->GetChildDynamicCast<UIElement>("TopBlock", true);
		assert(topBlock_);

		middleBlock_ = elRoot_->GetChildDynamicCast<UIElement>("MiddleBlock", true);
		assert(middleBlock_);

		// Init editor scene.
		// ----------------------------------------------------------------------------------------------------------------
		editorScene_ = MakeShared<EditorScene>(context_);

		// Init command history.
		// ----------------------------------------------------------------------------------------------------------------
		commandHistory_ = MakeShared<CommandHistory>(HISTORY_LIMIT);

		// Init menu bar.
		// ----------------------------------------------------------------------------------------------------------------
		menuBar_ = topBlock_->CreateChild<MenuBar>("MenuBar");
		menuBar_->SetStyleAuto();

		// Init hierarchy window view.
		// ----------------------------------------------------------------------------------------------------------------
		hierarchyWindowView_ = MakeShared<HierarchyWindowView>(context_, commandHistory_, editorScene_);

		// Init attribute window view.
		// ----------------------------------------------------------------------------------------------------------------
		attributeWindowView_ = MakeShared<AttributeWindowView>(context_, commandHistory_, editorScene_);

		// Init scene view.
		// ----------------------------------------------------------------------------------------------------------------
		sceneView_ = MakeShared<SceneView>(context_, middleBlock_, editorScene_);

		// Init panel view.
		// ----------------------------------------------------------------------------------------------------------------
		panelView_ = MakeShared<PanelView>(context_, middleBlock_);
		panelView_->RegisterWindowView(hierarchyWindowView_);
		panelView_->RegisterWindowView(attributeWindowView_);
		panelView_->AddTab(hierarchyWindowView_, true);
		panelView_->AddTab(attributeWindowView_);

		middleBlock_->UpdateLayout();

		// Init tools view.
		// ----------------------------------------------------------------------------------------------------------------
		toolsView_ = MakeShared<ToolsView>(context_, topBlock_);
		toolsView_->AddTool(MakeShared<MoveTool>(context_, commandHistory_, sceneView_));
		toolsView_->AddTool(MakeShared<ScaleTool>(context_, commandHistory_, sceneView_));
		toolsView_->AddTool(MakeShared<RotateTool>(context_, commandHistory_, sceneView_));

		toolsView_->AddTool(MakeShared<CollisionPolygon2DTool>(context_, commandHistory_, sceneView_));
		toolsView_->AddTool(MakeShared<CollisionBox2DTool>(context_, commandHistory_, sceneView_));
		toolsView_->AddTool(MakeShared<CollisionCircle2DTool>(context_, commandHistory_, sceneView_));

		// Populate menu bar.
		// ----------------------------------------------------------------------------------------------------------------
		fileMenu_ = menuBar_->AddMenu(FILE_MENU_TEXT);
		fileMenuOpenButton_ = fileMenu_->AddButton(FILE_MENU_OPEN_TEXT);
		fileMenuSaveButton_ = fileMenu_->AddButton(FILE_MENU_SAVE_TEXT);
		fileMenuQuitButton_ = fileMenu_->AddButton(FILE_MENU_QUIT_TEXT);

		editMenu_ = menuBar_->AddMenu(EDIT_MENU_TEXT);
		editMenuUndoButton_ = editMenu_->AddButton(EDIT_MENU_UNDO_TEXT);
		editMenuDeleteButton_ = editMenu_->AddButton(EDIT_MENU_DELETE_TEXT);
		editMenuUnselectButton_ = editMenu_->AddButton(EDIT_MENU_UNSELECT_TEXT);
		editMenuNewNodeButton_ = editMenu_->AddButton(EDIT_MENU_NEW_NODE_TEXT);
		editMenuCreateStaticSprite2DButton_ = editMenu_->AddButton(EDIT_MENU_CREATE_STATICSPRITE_TEXT);
		editMenuCreateRigidBody2DButton_ = editMenu_->AddButton(EDIT_MENU_CREATE_RIGIDBODY2D_TEXT);
		editMenuCreateCollisionBox2DButton_ = editMenu_->AddButton(EDIT_MENU_CREATE_COLLISIONBOX2D_TEXT);
		editMenuCreateCollisionPolygon2DButton_ = editMenu_->AddButton(EDIT_MENU_CREATE_COLLISIONPOLYGON2D_TEXT);
		editMenuCreateCollisionCircle2DButton_ = editMenu_->AddButton(EDIT_MENU_CREATE_COLLISIONCIRCLE2D_TEXT);

		viewMenu_ = menuBar_->AddMenu(VIEW_MENU_TEXT);
		viewMenuAttributeWindowButton_ = viewMenu_->AddButton(VIEW_MENU_ATTRIBUTE_WINDOW_TEXT);
		viewMenuHierarchyWindowButton_ = viewMenu_->AddButton(VIEW_MENU_HIERARCHY_WINDOW_TEXT);
		viewMenuCloseAllWindowButton_ = viewMenu_->AddButton(VIEW_MENU_CLOSE_ALL_WINDOW_TEXT);
		viewMenuOpenAllWindowButton_ = viewMenu_->AddButton(VIEW_MENU_OPEN_ALL_WINDOW_TEXT);
		viewMenuResetWindowLayoutButton_ = viewMenu_->AddButton(VIEW_MENU_RESET_WINDOW_LAYOUT_TEXT);
		viewMenuDebugGeometryEnabledCheckBox_ = viewMenu_->AddToggle(VIEW_MENU_DEBUG_GEOMETRY_ENABLED_TEXT);
		viewMenuNodePositionGizmoEnabledCheckBox_ = viewMenu_->AddToggle(VIEW_MENU_NODE_POSITION_GIZMO_ENABLED_TEXT);
		viewMenuGridEnabledCheckBox_ = viewMenu_->AddToggle(VIEW_MENU_GRID_ENABLED_TEXT);

		// Synchronize menu bar.
		// ----------------------------------------------------------------------------------------------------------------
		viewMenuDebugGeometryEnabledCheckBox_->SetChecked(sceneView_->GetDebugGeometryEnabled());
		viewMenuNodePositionGizmoEnabledCheckBox_->SetChecked(sceneView_->GetNodePositionGizmoEnabled());
		viewMenuGridEnabledCheckBox_->SetChecked(sceneView_->GetGridEnabled());

		// Init all events.
		// ----------------------------------------------------------------------------------------------------------------
		SubscribeToEvent(fileMenuOpenButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleFileMenuOpenReleased));
		SubscribeToEvent(fileMenuSaveButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleFileMenuSaveReleased));
		SubscribeToEvent(fileMenuQuitButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleFileMenuQuitReleased));
		SubscribeToEvent(editMenuUndoButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleEditMenuUndoReleased));
		SubscribeToEvent(editMenuDeleteButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleEditMenuDeleteReleased));
		SubscribeToEvent(editMenuUnselectButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleEditMenuUnselectReleased));
		SubscribeToEvent(editMenuNewNodeButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleEditMenuCreateNewNodeReleased));
		SubscribeToEvent(editMenuCreateStaticSprite2DButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleEditMenuCreateStaticSprite2DReleased));
		SubscribeToEvent(editMenuCreateRigidBody2DButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleEditMenuCreateRigidBody2DReleased));
		SubscribeToEvent(editMenuCreateCollisionBox2DButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleEditMenuCreateCollisionBox2DReleased));
		SubscribeToEvent(editMenuCreateCollisionPolygon2DButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleEditMenuCreateCollisionPolygon2DReleased));
		SubscribeToEvent(editMenuCreateCollisionCircle2DButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleEditMenuCreateCollisionCircle2DReleased));
		SubscribeToEvent(viewMenuAttributeWindowButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleViewMenuAttributeWindowReleased));
		SubscribeToEvent(viewMenuHierarchyWindowButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleViewMenuHierarchyWindowReleased));
		SubscribeToEvent(viewMenuCloseAllWindowButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleViewMenuCloseAllWindowReleased));
		SubscribeToEvent(viewMenuOpenAllWindowButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleViewMenuOpenAllWindowReleased));
		SubscribeToEvent(viewMenuResetWindowLayoutButton_, E_RELEASED, URHO3D_HANDLER(EditorView, HandleViewMenuResetWindowLayoutReleased));
		SubscribeToEvent(viewMenuDebugGeometryEnabledCheckBox_, E_TOGGLED, URHO3D_HANDLER(EditorView, HandleViewMenuDebugGeometryEnabledToggled));
		SubscribeToEvent(viewMenuNodePositionGizmoEnabledCheckBox_, E_TOGGLED, URHO3D_HANDLER(EditorView, HandleViewMenuNodePositionGizmoEnabledToggled));
		SubscribeToEvent(viewMenuGridEnabledCheckBox_, E_TOGGLED, URHO3D_HANDLER(EditorView, HandleViewMenuGridEnabledToggled));
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void EditorView::HandleFileMenuOpenReleased(StringHash, VariantMap&)
	{
		CreateOpenSceneFileSelector();
	}

	void EditorView::HandleFileMenuSaveReleased(StringHash, VariantMap&)
	{
		CreateSaveSceneFileSelector();
	}

	void EditorView::HandleFileMenuQuitReleased(StringHash, VariantMap&)
	{
		GetSubsystem<Engine>()->Exit();
	}

	void EditorView::HandleEditMenuUndoReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<UndoCommand>(commandHistory_), commandHistory_).Exec();
	}

	void EditorView::HandleEditMenuDeleteReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<DeleteCommand>(editorScene_), commandHistory_).Exec();
	}

	void EditorView::HandleEditMenuUnselectReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<UnselectCommand>(editorScene_), commandHistory_).Exec();
	}

	void EditorView::HandleEditMenuCreateNewNodeReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<NewNodeCommand>(editorScene_), commandHistory_).Exec();
	}

	void EditorView::HandleEditMenuCreateStaticSprite2DReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<CreateStaticSprite2DCommand>(editorScene_), commandHistory_).Exec();
	}

	void EditorView::HandleEditMenuCreateRigidBody2DReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<CreateRigidBody2DCommand>(editorScene_), commandHistory_).Exec();
	}

	void EditorView::HandleEditMenuCreateCollisionBox2DReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<CreateCollisionBox2DCommand>(editorScene_), commandHistory_).Exec();
	}

	void EditorView::HandleEditMenuCreateCollisionPolygon2DReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<CreateCollisionPolygon2DCommand>(editorScene_), commandHistory_).Exec();
	}

	void EditorView::HandleEditMenuCreateCollisionCircle2DReleased(StringHash, VariantMap&)
	{
		CommandInvoker(MakeShared<CreateCollisionCircle2DCommand>(editorScene_), commandHistory_).Exec();
	}

	void EditorView::HandleViewMenuAttributeWindowReleased(StringHash, VariantMap&)
	{
		attributeWindowView_->Open();
	}

	void EditorView::HandleViewMenuHierarchyWindowReleased(StringHash, VariantMap&)
	{
		hierarchyWindowView_->Open();
	}

	void EditorView::HandleViewMenuCloseAllWindowReleased(StringHash, VariantMap&)
	{
		attributeWindowView_->Close();
		hierarchyWindowView_->Close();
	}

	void EditorView::HandleViewMenuOpenAllWindowReleased(StringHash, VariantMap&)
	{
		attributeWindowView_->Open();
		hierarchyWindowView_->Open();
	}

	void EditorView::HandleViewMenuResetWindowLayoutReleased(StringHash, VariantMap&)
	{
		ResetWindowLayout();
	}

	void EditorView::HandleViewMenuDebugGeometryEnabledToggled(StringHash, VariantMap& eventData)
	{
		sceneView_->SetDebugGeometryEnabled(eventData[Toggled::P_STATE].GetBool());
	}

	void EditorView::HandleViewMenuNodePositionGizmoEnabledToggled(StringHash, VariantMap& eventData)
	{
		sceneView_->SetNodePositionGizmoEnabled(eventData[Toggled::P_STATE].GetBool());
	}

	void EditorView::HandleViewMenuGridEnabledToggled(StringHash, VariantMap& eventData)
	{
		sceneView_->SetGridEnabled(eventData[Toggled::P_STATE].GetBool());
	}

	void EditorView::HandleOpenSceneFileSelected(StringHash, VariantMap& eventData)
	{
		using namespace FileSelected;
		String filename = eventData[P_FILENAME].GetString();
		bool ok = eventData[P_OK].GetBool();

		UnsubscribeFromEvents(fileSelector_);

		if (ok)
		{
			editorScene_->Load(filename);
		}

		fileSelector_.Reset();
	}

	void EditorView::HandleSaveSceneFileSelected(StringHash, VariantMap& eventData)
	{
		using namespace FileSelected;
		String filename = eventData[P_FILENAME].GetString();
		bool ok = eventData[P_OK].GetBool();

		UnsubscribeFromEvents(fileSelector_);

		if (ok)
		{
			editorScene_->Save(filename);
		}

		fileSelector_.Reset();
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	void EditorView::ResetWindowLayout()
	{
	}

	void EditorView::CreateOpenSceneFileSelector()
	{
		if (fileSelector_.NotNull())
		{
			fileSelector_.Reset();
		}

		fileSelector_ = MakeShared<FileSelector>(context_);
		fileSelector_->SetDefaultStyle(uiRoot_->GetDefaultStyle());
		fileSelector_->SetTitle("Open Scene File");
		fileSelector_->SetButtonTexts("OPEN", "CANCEL");
		fileSelector_->SetFilters({ ".xml" }, 0);

		SubscribeToEvent(fileSelector_, E_FILESELECTED, URHO3D_HANDLER(EditorView, HandleOpenSceneFileSelected));
	}

	void EditorView::CreateSaveSceneFileSelector()
	{
		if (fileSelector_.NotNull())
		{
			fileSelector_.Reset();
		}

		fileSelector_ = MakeShared<FileSelector>(context_);
		fileSelector_->SetDefaultStyle(uiRoot_->GetDefaultStyle());
		fileSelector_->SetTitle("Save Scene File");
		fileSelector_->SetButtonTexts("SAVE", "CANCEL");
		fileSelector_->SetFilters({ ".xml" }, 0);

		SubscribeToEvent(fileSelector_, E_FILESELECTED, URHO3D_HANDLER(EditorView, HandleSaveSceneFileSelected));
	}
}
