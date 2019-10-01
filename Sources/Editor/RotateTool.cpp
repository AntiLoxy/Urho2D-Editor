#include "RotateTool.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/Scene/SceneEvents.h>

using namespace Urho3D;

static const String ACTIVE_CURSOR_SHAPE = "Rotate";

namespace Geode
{
	RotateTool::RotateTool(Context* context, CommandHistory::Ptr commandHistory, SceneView::Ptr sceneView) : ITool(context)
	{
		commandHistory_ = commandHistory;
		sceneView_ = sceneView;
		editorScene_ = sceneView->GetEditorScene();
		cursor_ = GetSubsystem<UI>()->GetCursor();
		radio_ = nullptr;
		selectedNode_ = nullptr;
		editing_ = false;
		beginNodeRotation_ = 0;
		currentNodeRotation_ = 0;

		SetSelectedNode(editorScene_->GetSelectedNode());

		SubscribeToEvent(editorScene_, E_SCENEUPDATE, URHO3D_HANDLER(RotateTool, HandleSceneUpdate));
		SubscribeToEvent(editorScene_, E_SELECTEDOBJECTCHANGED, URHO3D_HANDLER(RotateTool, HandleSelectedObjectChanged));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGBEGIN, URHO3D_HANDLER(RotateTool, HandleSceneViewDragBegin));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGMOVE, URHO3D_HANDLER(RotateTool, HandleSceneViewDragMove));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGEND, URHO3D_HANDLER(RotateTool, HandleSceneViewDragEnd));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGCANCEL, URHO3D_HANDLER(RotateTool, HandleSceneViewDragCancel));
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void RotateTool::SetEnabled(bool enabled)
	{
		EnableRadio(enabled);
		ITool::SetEnabled(enabled);
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	UIElement* RotateTool::PopulateToolBar(ToolBar* toolBar)
	{
		radio_ = toolBar->AddIconRadio("RotateToolRadio", "NodeToolRgn", "IconRotate", "Rotate");
		EnableRadio(IsEnabled());

		SubscribeToEvent(radio_, E_TOGGLED, URHO3D_HANDLER(RotateTool, HandleRadioToggled));
		return radio_;
	}

	bool RotateTool::IsActive()
	{
		return IsEnabled() && radio_->IsChecked() && selectedNode_ != nullptr;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void RotateTool::HandleSceneUpdate(StringHash, VariantMap&)
	{
		UpdateEditingState();
		UpdateCursor();
	}

	void RotateTool::HandleSelectedObjectChanged(StringHash, VariantMap&)
	{
		SetSelectedNode(editorScene_->GetSelectedNode());
	}

	void RotateTool::HandleSceneViewDragBegin(StringHash, VariantMap&)
	{
		if (!IsActive() || !sceneView_->IsMouseInsideNode(selectedNode_))
		{
			return;
		}

		currentNodeRotation_ = beginNodeRotation_ = selectedNode_->GetWorldRotation2D();
		beginMouseWorldPosition_ = sceneView_->GetMouseWorldPosition();
		editing_ = true;
	}

	void RotateTool::HandleSceneViewDragMove(StringHash, VariantMap&)
	{
		if (!IsActive() || !editing_)
		{
			return;
		}

		auto currentMouseWorldPosition = sceneView_->GetMouseWorldPosition();
		auto nodePosition = selectedNode_->GetWorldPosition2D();
		auto v1 = nodePosition - beginMouseWorldPosition_;
		auto v2 = nodePosition - currentMouseWorldPosition;

		auto angleDelta = CalculateAngleBetween(v2, v1);
		currentNodeRotation_ = AngleBounded(beginNodeRotation_ + angleDelta);
		selectedNode_->SetWorldRotation2D(currentNodeRotation_);
	}

	void RotateTool::HandleSceneViewDragEnd(StringHash, VariantMap&)
	{
		if (!IsActive() || !editing_)
		{
			return;
		}

		CommandInvoker(MakeShared<RotateNodeCommand>(editorScene_, beginNodeRotation_, currentNodeRotation_), commandHistory_).Exec();
		editing_ = false;
	}

	void RotateTool::HandleSceneViewDragCancel(StringHash, VariantMap&)
	{
		if (!IsActive() || !editing_)
		{
			return;
		}

		selectedNode_->SetWorldRotation2D(beginNodeRotation_);
		editing_ = false;
	}

	void RotateTool::HandleRadioToggled(StringHash, VariantMap& eventData)
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

	void RotateTool::UpdateEditingState()
	{
		if (!IsActive())
		{
			editing_ = false;
		}
	}

	void RotateTool::UpdateCursor()
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

	void RotateTool::SetSelectedNode(Node* selectedNode)
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

	void RotateTool::EnableRadio(bool enable)
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

	float RotateTool::Atan2Abs(float y, float x)
	{
		auto angle = Atan2<float>(y, x);
		return (angle > 0) ? angle : (angle + 360.0f);
	}

	float RotateTool::CalculateAngleBetween(Vector2 v1, Vector2 v2)
	{
		auto angleV1 = Atan2Abs(v1.y_, v1.x_);
		auto angleV2 = Atan2Abs(v2.y_, v2.x_);

		return angleV1 - angleV2;
	}

	float RotateTool::AngleBounded(float angle)
	{
		if (angle > 360)
		{
			angle -= 360;
		}

		if (angle < 0)
		{
			angle += 360;
		}

		return angle;
	}
}