#include "CollisionBox2DTool.h"

#include <Urho3D/Scene/SceneEvents.h>

using namespace Urho3D;

namespace Geode
{
	CollisionBox2DTool::CollisionBox2DTool(Context* context, CommandHistory::Ptr commandHistory, SceneView::Ptr sceneView) : ITool(context)
	{
		commandHistory_ = commandHistory;
		sceneView_ = sceneView;
		editorScene_ = sceneView->GetEditorScene();
		selectedCollisionBox2D_ = nullptr;
		
		anchorBox_ = MakeShared<AnchorBox>(context, sceneView);
		anchorBox_->SetEnabled(false);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::TOP);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::TOP_LEFT);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::TOP_RIGHT);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::MIDDLE);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::MIDDLE_LEFT);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::MIDDLE_RIGHT);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::BOTTOM);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::BOTTOM_LEFT);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::BOTTOM_RIGHT);

		SetSelectedCollisionBox2D(editorScene_->GetSelectedObjectAs<CollisionBox2D>());

		SubscribeToEvent(editorScene_, E_SCENEUPDATE, URHO3D_HANDLER(CollisionBox2DTool, HandleSceneUpdate));
		SubscribeToEvent(editorScene_, E_SELECTEDOBJECTCHANGED, URHO3D_HANDLER(CollisionBox2DTool, HandleSelectedObjectChanged));
		SubscribeToEvent(anchorBox_, E_ANCHORBOX_SIZECHANGED, URHO3D_HANDLER(CollisionBox2DTool, HandleAnchorBoxSizeChanged));
		SubscribeToEvent(anchorBox_, E_ANCHORBOX_CENTERCHANGED, URHO3D_HANDLER(CollisionBox2DTool, HandleAnchorBoxCenterChanged));
		SubscribeToEvent(anchorBox_, E_ANCHORBOX_CONFIRM_CHANGED, URHO3D_HANDLER(CollisionBox2DTool, HandleAnchorBoxConfirmChanged));
		SubscribeToEvent(anchorBox_, E_ANCHORBOX_CANCELED, URHO3D_HANDLER(CollisionBox2DTool, HandleAnchorBoxCanceled));
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	UIElement* CollisionBox2DTool::PopulateToolBar(ToolBar* toolBar)
	{
		return nullptr;
	}

	bool CollisionBox2DTool::IsActive()
	{
		return IsEnabled() && selectedCollisionBox2D_ != nullptr;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT METHODS
	///------------------------------------------------------------------------------------------------

	void CollisionBox2DTool::HandleSceneUpdate(StringHash, VariantMap&)
	{
		if (!IsActive())
		{
			return;
		}

		if (!anchorBox_->IsEditing())
		{
			anchorBox_->SetBlockEvents(true);

			anchorBox_->SetTransform(selectedCollisionBox2D_->GetNode()->GetTransform());
			anchorBox_->SetSize(selectedCollisionBox2D_->GetSize());
			anchorBox_->SetCenter(selectedCollisionBox2D_->GetCenter());

			anchorBox_->SetBlockEvents(false);
		}
	}

	void CollisionBox2DTool::HandleSelectedObjectChanged(StringHash, VariantMap&)
	{
		SetSelectedCollisionBox2D(editorScene_->GetSelectedObjectAs<CollisionBox2D>());
	}

	void CollisionBox2DTool::HandleAnchorBoxSizeChanged(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto size = Vector2(eventData[AnchorBoxSizeChanged::P_WIDTH].GetFloat(), eventData[AnchorBoxSizeChanged::P_HEIGHT].GetFloat());
		selectedCollisionBox2D_->SetSize(size);
	}

	void CollisionBox2DTool::HandleAnchorBoxCenterChanged(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto center = Vector2(eventData[AnchorBoxCenterChanged::P_X].GetFloat(), eventData[AnchorBoxCenterChanged::P_Y].GetFloat());
		selectedCollisionBox2D_->SetCenter(center);
	}

	void CollisionBox2DTool::HandleAnchorBoxConfirmChanged(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto size = Vector2(eventData[AnchorBoxConfirmChanged::P_WIDTH].GetFloat(), eventData[AnchorBoxConfirmChanged::P_HEIGHT].GetFloat());
		auto beginSize = Vector2(eventData[AnchorBoxConfirmChanged::P_BEGIN_WIDTH].GetFloat(), eventData[AnchorBoxConfirmChanged::P_BEGIN_HEIGHT].GetFloat());

		auto center = Vector2(eventData[AnchorBoxConfirmChanged::P_X].GetFloat(), eventData[AnchorBoxConfirmChanged::P_Y].GetFloat());
		auto beginCenter = Vector2(eventData[AnchorBoxConfirmChanged::P_BEGIN_X].GetFloat(), eventData[AnchorBoxConfirmChanged::P_BEGIN_Y].GetFloat());

		CommandInvoker(MakeShared<ChangeSizeCenterCollisionBox2DCommand>(editorScene_, beginSize, beginCenter, size, center), commandHistory_).Exec();
	}

	void CollisionBox2DTool::HandleAnchorBoxCanceled(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto size = Vector2(eventData[AnchorBoxCanceled::P_WIDTH].GetFloat(), eventData[AnchorBoxCanceled::P_HEIGHT].GetFloat());
		auto center = Vector2(eventData[AnchorBoxCanceled::P_X].GetFloat(), eventData[AnchorBoxCanceled::P_Y].GetFloat());

		selectedCollisionBox2D_->SetSize(size);
		selectedCollisionBox2D_->SetCenter(center);
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void CollisionBox2DTool::SetSelectedCollisionBox2D(CollisionBox2D* selectedCollisionBox2D)
	{
		if (selectedCollisionBox2D == nullptr)
		{
			SetEnabled(false);
			anchorBox_->SetEnabled(false);
		}
		else
		{
			SetEnabled(true);
			anchorBox_->SetEnabled(true);
		}

		selectedCollisionBox2D_ = selectedCollisionBox2D;
	}
}