#include "MoveTool.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/Scene/SceneEvents.h>

using namespace Urho3D;

static const String ACTIVE_CURSOR_SHAPE = "Move";

namespace Geode
{
	MoveTool::MoveTool(Context* context, CommandHistory::Ptr commandHistory, SceneView::Ptr sceneView) : ITool(context)
	{
		commandHistory_ = commandHistory;
		sceneView_ = sceneView;
		editorScene_ = sceneView->GetEditorScene();
		cursor_ = GetSubsystem<UI>()->GetCursor();
		radio_ = nullptr;
		selectedNode_ = nullptr;
		editing_ = false;

		SetSelectedNode(editorScene_->GetSelectedNode());

		SubscribeToEvent(editorScene_, E_SCENEUPDATE, URHO3D_HANDLER(MoveTool, HandleSceneUpdate));
		SubscribeToEvent(editorScene_, E_SELECTEDOBJECTCHANGED, URHO3D_HANDLER(MoveTool, HandleSelectedObjectChanged));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGBEGIN, URHO3D_HANDLER(MoveTool, HandleSceneViewDragBegin));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGMOVE, URHO3D_HANDLER(MoveTool, HandleSceneViewDragMove));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGEND, URHO3D_HANDLER(MoveTool, HandleSceneViewDragEnd));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGCANCEL, URHO3D_HANDLER(MoveTool, HandleSceneViewDragCancel));
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void MoveTool::SetEnabled(bool enabled)
	{
		EnableRadio(enabled);
		ITool::SetEnabled(enabled);
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	UIElement* MoveTool::PopulateToolBar(ToolBar* toolBar)
	{
		radio_ = toolBar->AddIconRadio("MoveToolRadio", "NodeToolRgn", "IconMove", "Move");
		EnableRadio(IsEnabled());

		SubscribeToEvent(radio_, E_TOGGLED, URHO3D_HANDLER(MoveTool, HandleRadioToggled));
		return radio_;
	}

	bool MoveTool::IsActive()
	{
		return IsEnabled() && radio_->IsChecked() && selectedNode_ != nullptr;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT METHODS
	///------------------------------------------------------------------------------------------------

	void MoveTool::HandleSceneUpdate(StringHash, VariantMap&)
	{
		UpdateEditingState();
		UpdateCursor();
	}

	void MoveTool::HandleSelectedObjectChanged(StringHash, VariantMap&)
	{
		SetSelectedNode(editorScene_->GetSelectedNode());
	}

	void MoveTool::HandleSceneViewDragBegin(StringHash, VariantMap&)
	{
		if (!IsActive() || !sceneView_->IsMouseInsideNode(selectedNode_))
		{
			return;
		}

		currentNodePosition_ = beginNodePosition_ = selectedNode_->GetWorldPosition2D();
		editing_ = true;
	}

	void MoveTool::HandleSceneViewDragMove(StringHash, VariantMap& eventData)
	{
		if (!IsActive() || !editing_)
		{
			return;
		}

		auto delta = Vector2(eventData[SceneViewDragMove::P_DX].GetFloat(), eventData[SceneViewDragMove::P_DY].GetFloat());
		currentNodePosition_ = beginNodePosition_ + delta;
		selectedNode_->SetWorldPosition2D(currentNodePosition_);
	}

	void MoveTool::HandleSceneViewDragEnd(StringHash, VariantMap&)
	{
		if (!IsActive() || !editing_)
		{
			return;
		}

		CommandInvoker(MakeShared<MoveNodeCommand>(editorScene_, beginNodePosition_, currentNodePosition_), commandHistory_).Exec();
		editing_ = false;
	}

	void MoveTool::HandleSceneViewDragCancel(StringHash, VariantMap&)
	{
		if (!IsActive() || !editing_)
		{
			return;
		}

		selectedNode_->SetWorldPosition2D(beginNodePosition_);
		editing_ = false;
	}

	void MoveTool::HandleRadioToggled(StringHash, VariantMap& eventData)
	{
		if (eventData[Toggled::P_STATE].GetBool())
		{
			VariantMap sendEventData;
			sendEventData[ToolSelected::P_NAME] = GetTypeName();
			sendEventData[ToolSelected::P_OBJECT] = this;
			SendEvent(E_TOOLSELECTED, sendEventData);
		}
	}

	///------------------------------------------------------------------------------------------------
	///  UPDATE METHODS
	///------------------------------------------------------------------------------------------------

	void MoveTool::UpdateEditingState()
	{
		if (!IsActive())
		{
			editing_ = false;
		}
	}

	void MoveTool::UpdateCursor()
	{
		if (!IsActive())
		{
			return;
		}

		if (editing_ || sceneView_->IsMouseInsideNode(selectedNode_))
		{
			cursor_->SetShape(ACTIVE_CURSOR_SHAPE);
		}
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void MoveTool::SetSelectedNode(Node* selectedNode)
	{
		if (selectedNode == nullptr)
		{
			SetEnabled(false);
		}
		else
		{
			SetEnabled(true);
		}

		selectedNode_ = selectedNode;
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	void MoveTool::EnableRadio(bool enable)
	{
		if (radio_ == nullptr)
		{
			return;
		}

		if (enable)
		{
			radio_->SetEnabled(true);
			radio_->SetOpacity(1);
		}
		else
		{
			radio_->SetEnabled(false);
			radio_->SetOpacity(0.5);
		}
	}
}