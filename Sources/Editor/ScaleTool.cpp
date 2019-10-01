#include "ScaleTool.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/SceneEvents.h>

using namespace Urho3D;

static const String ACTIVE_CURSOR_SHAPE = "ResizeDiagonalTopRight";

namespace Geode
{
	ScaleTool::ScaleTool(Context* context, CommandHistory::Ptr commandHistory, SceneView::Ptr sceneView) : ITool(context)
	{
		commandHistory_ = commandHistory;
		sceneView_ = sceneView;
		editorScene_ = sceneView->GetEditorScene();
		cursor_ = GetSubsystem<UI>()->GetCursor();
		radio_ = nullptr;
		selectedNode_ = nullptr;
		editing_ = false;
		
		SetSelectedNode(editorScene_->GetSelectedNode());

		SubscribeToEvent(editorScene_, E_SCENEUPDATE, URHO3D_HANDLER(ScaleTool, HandleSceneUpdate));
		SubscribeToEvent(editorScene_, E_SELECTEDOBJECTCHANGED, URHO3D_HANDLER(ScaleTool, HandleSelectedObjectChanged));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGBEGIN, URHO3D_HANDLER(ScaleTool, HandleSceneViewDragBegin));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGMOVE, URHO3D_HANDLER(ScaleTool, HandleSceneViewDragMove));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGEND, URHO3D_HANDLER(ScaleTool, HandleSceneViewDragEnd));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGCANCEL, URHO3D_HANDLER(ScaleTool, HandleSceneViewDragCancel));
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void ScaleTool::SetEnabled(bool enabled)
	{
		EnableRadio(enabled);
		ITool::SetEnabled(enabled);
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	UIElement* ScaleTool::PopulateToolBar(ToolBar* toolBar)
	{
		radio_ = toolBar->AddIconRadio("ScaleToolRadio", "NodeToolRgn", "IconScale", "Scale");
		EnableRadio(IsEnabled());

		SubscribeToEvent(radio_, E_TOGGLED, URHO3D_HANDLER(ScaleTool, HandleRadioToggled));
		return radio_;
	}

	bool ScaleTool::IsActive()
	{
		return IsEnabled() && radio_->IsChecked() && selectedNode_ != nullptr;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void ScaleTool::HandleSceneUpdate(StringHash, VariantMap&)
	{
		UpdateEditingState();
		UpdateCursor();
	}

	void ScaleTool::HandleSelectedObjectChanged(StringHash, VariantMap&)
	{
		SetSelectedNode(editorScene_->GetSelectedNode());
	}

	void ScaleTool::HandleSceneViewDragBegin(StringHash, VariantMap& eventData)
	{
		if (!IsActive() || !sceneView_->IsMouseInsideNode(selectedNode_))
		{
			return;
		}

		auto mouseWorldPosition = Vector2(eventData[SceneViewDragBegin::P_X].GetFloat(), eventData[SceneViewDragBegin::P_Y].GetFloat());

		currentNodeScale_ = beginNodeScale_ = selectedNode_->GetWorldScale2D();
		baseDelta_ = selectedNode_->GetWorldPosition2D() - mouseWorldPosition;
		editing_ = true;
	}

	void ScaleTool::HandleSceneViewDragMove(StringHash, VariantMap& eventData)
	{
		if (!IsActive() || !editing_)
		{
			return;
		}

		auto input = GetSubsystem<Input>();

		auto mouseWorldPosition = Vector2(eventData[SceneViewDragMove::P_X].GetFloat(), eventData[SceneViewDragMove::P_Y].GetFloat());
		auto delta = selectedNode_->GetWorldPosition2D() - mouseWorldPosition;
		auto factor = (delta / baseDelta_);

		if (input->GetKeyDown(KEY_CTRL))
		{
			factor.y_ = factor.x_;
		}

		currentNodeScale_ = factor * beginNodeScale_;
		selectedNode_->SetWorldScale2D(currentNodeScale_);
	}

	void ScaleTool::HandleSceneViewDragEnd(StringHash, VariantMap&)
	{
		if (!IsActive() || !editing_)
		{
			return;
		}

		CommandInvoker(MakeShared<ScaleNodeCommand>(editorScene_, beginNodeScale_, currentNodeScale_), commandHistory_).Exec();
		editing_ = false;
	}

	void ScaleTool::HandleSceneViewDragCancel(StringHash, VariantMap&)
	{
		if (!IsActive() || !editing_)
		{
			return;
		}

		selectedNode_->SetWorldScale2D(beginNodeScale_);
		editing_ = false;
	}

	void ScaleTool::HandleRadioToggled(StringHash, VariantMap& eventData)
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

	void ScaleTool::UpdateEditingState()
	{
		if (!IsActive())
		{
			editing_ = false;
		}
	}

	void ScaleTool::UpdateCursor()
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

	void ScaleTool::SetSelectedNode(Node* selectedNode)
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

	void ScaleTool::EnableRadio(bool enable)
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