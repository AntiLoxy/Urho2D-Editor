#include "CollisionCircle2DTool.h"

#include <Urho3D/Scene/SceneEvents.h>

using namespace Urho3D;

namespace Geode
{
	CollisionCircle2DTool::CollisionCircle2DTool(Context* context, CommandHistory::Ptr commandHistory, SceneView::Ptr sceneView) : ITool(context)
	{
		commandHistory_ = commandHistory;
		sceneView_ = sceneView;
		editorScene_ = sceneView->GetEditorScene();
		selectedCollisionCircle2D_ = nullptr;

		anchorBox_ = MakeShared<AnchorBox>(context, sceneView);
		anchorBox_->SetEnabled(false);
		anchorBox_->SetHomothetics(true);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::TOP);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::MIDDLE);
		anchorBox_->AddAnchor(AnchorBox::AnchorId::BOTTOM);

		SetSelectedCollisionCircle2D(editorScene_->GetSelectedObjectAs<CollisionCircle2D>());

		SubscribeToEvent(editorScene_, E_SCENEUPDATE, URHO3D_HANDLER(CollisionCircle2DTool, HandleSceneUpdate));
		SubscribeToEvent(editorScene_, E_SELECTEDOBJECTCHANGED, URHO3D_HANDLER(CollisionCircle2DTool, HandleSelectedObjectChanged));
		SubscribeToEvent(anchorBox_, E_ANCHORBOX_SIZECHANGED, URHO3D_HANDLER(CollisionCircle2DTool, HandleAnchorBoxSizeChanged));
		SubscribeToEvent(anchorBox_, E_ANCHORBOX_CENTERCHANGED, URHO3D_HANDLER(CollisionCircle2DTool, HandleAnchorBoxCenterChanged));
		SubscribeToEvent(anchorBox_, E_ANCHORBOX_CONFIRM_SIZECHANGED, URHO3D_HANDLER(CollisionCircle2DTool, HandleAnchorBoxConfirmSizeChanged));
		SubscribeToEvent(anchorBox_, E_ANCHORBOX_CONFIRM_CENTERCHANGED, URHO3D_HANDLER(CollisionCircle2DTool, HandleAnchorBoxConfirmCenterChanged));
		SubscribeToEvent(anchorBox_, E_ANCHORBOX_CANCELED, URHO3D_HANDLER(CollisionCircle2DTool, HandleAnchorBoxCanceled));
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	UIElement* CollisionCircle2DTool::PopulateToolBar(ToolBar* toolBar)
	{
		return nullptr;
	}

	bool CollisionCircle2DTool::IsActive()
	{
		return IsEnabled() && selectedCollisionCircle2D_ != nullptr;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT METHODS
	///------------------------------------------------------------------------------------------------

	void CollisionCircle2DTool::HandleSceneUpdate(StringHash, VariantMap&)
	{
		if (!IsActive())
		{
			return;
		}

		if (!anchorBox_->IsEditing())
		{
			auto size = selectedCollisionCircle2D_->GetRadius() * 2;

			anchorBox_->SetBlockEvents(true);

			anchorBox_->SetTransform(selectedCollisionCircle2D_->GetNode()->GetTransform());
			anchorBox_->SetSize(Vector2(size, size));
			anchorBox_->SetCenter(selectedCollisionCircle2D_->GetCenter());

			anchorBox_->SetBlockEvents(false);
		}
	}

	void CollisionCircle2DTool::HandleSelectedObjectChanged(StringHash, VariantMap&)
	{
		SetSelectedCollisionCircle2D(editorScene_->GetSelectedObjectAs<CollisionCircle2D>());
	}

	void CollisionCircle2DTool::HandleAnchorBoxSizeChanged(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto size = eventData[AnchorBoxSizeChanged::P_HEIGHT].GetFloat();

		anchorBox_->SetBlockEvents(true);
		anchorBox_->SetSize(Vector2(size, size));
		anchorBox_->SetBlockEvents(false);

		selectedCollisionCircle2D_->SetRadius(size / 2);
	}

	void CollisionCircle2DTool::HandleAnchorBoxCenterChanged(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto center = Vector2(eventData[AnchorBoxCenterChanged::P_X].GetFloat(), eventData[AnchorBoxCenterChanged::P_Y].GetFloat());
		selectedCollisionCircle2D_->SetCenter(center);
	}

	void CollisionCircle2DTool::HandleAnchorBoxConfirmSizeChanged(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto size = eventData[AnchorBoxConfirmSizeChanged::P_HEIGHT].GetFloat();
		auto beginSize = eventData[AnchorBoxConfirmSizeChanged::P_BEGIN_HEIGHT].GetFloat();

		CommandInvoker(MakeShared<ChangeRadiusCollisionCircle2DCommand>(editorScene_, beginSize / 2, size / 2), commandHistory_).Exec();
	}

	void CollisionCircle2DTool::HandleAnchorBoxConfirmCenterChanged(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto center = Vector2(eventData[AnchorBoxConfirmCenterChanged::P_X].GetFloat(), eventData[AnchorBoxConfirmCenterChanged::P_Y].GetFloat());
		auto beginCenter = Vector2(eventData[AnchorBoxConfirmCenterChanged::P_BEGIN_X].GetFloat(), eventData[AnchorBoxConfirmCenterChanged::P_BEGIN_Y].GetFloat());

		CommandInvoker(MakeShared<ChangeCenterCollisionCircle2DCommand>(editorScene_, beginCenter, center), commandHistory_).Exec();
	}

	void CollisionCircle2DTool::HandleAnchorBoxCanceled(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto size = eventData[AnchorBoxCanceled::P_HEIGHT].GetFloat();
		auto center = Vector2(eventData[AnchorBoxCanceled::P_X].GetFloat(), eventData[AnchorBoxCanceled::P_Y].GetFloat());

		selectedCollisionCircle2D_->SetRadius(size / 2);
		selectedCollisionCircle2D_->SetCenter(center);
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void CollisionCircle2DTool::SetSelectedCollisionCircle2D(CollisionCircle2D* selectedCollisionCircle2D)
	{
		if (selectedCollisionCircle2D == nullptr)
		{
			SetEnabled(false);
			anchorBox_->SetEnabled(false);
		}
		else
		{
			SetEnabled(true);
			anchorBox_->SetEnabled(true);
		}

		selectedCollisionCircle2D_ = selectedCollisionCircle2D;
	}
}