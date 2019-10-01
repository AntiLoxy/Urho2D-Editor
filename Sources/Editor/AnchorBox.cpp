#include "AnchorBox.h"
#include "AnchorBoxEvents.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Material.h>

using namespace Urho3D;

static const Color BOX_COLOR = Color(0, 1, 0);
static const int BOX_Z_INDEX = -1;
static const float ANCHOR_SIZE = 0.1f;
static const int ANCHOR_Z_INDEX = -2;
static const Color ANCHOR_COLOR = Color(0, 0, 1);
static const Color SELECTED_ANCHOR_COLOR = Color(1, 0, 0);

static const float ANCHOR_MATRICES[][2][2] = {	// [anchorId][size][center]
	{{  0,  1 }, {   0, 0.5 }},	// TOP
	{{ -1,  1 }, { 0.5, 0.5 }},	// TOP_LEFT
	{{  1,  1 }, { 0.5, 0.5 }},	// TOP_RIGHT
	{{  0,  0 }, {   1,   1 }},	// MIDDLE
	{{ -1,  0 }, { 0.5,   0 }},	// MIDDLE_LEFT
	{{  1,  0 }, { 0.5,   0 }},	// MIDDLE_RIGHT
	{{  0, -1 }, {   0, 0.5 }},	// BOTTOM
	{{ -1, -1 }, { 0.5, 0.5 }},	// BOTTOM_LEFT
	{{  1, -1 }, { 0.5, 0.5 }}	// BOTTOM_RIGHT
};

static const float ANCHOR_HOMOTHETICS_MATRICES[][2][2] = { // [anchorId][size][center]
	{{  0,  2 }, { 0, 0 }},	// TOP
	{{ -2,  2 }, { 0, 0 }},	// TOP_LEFT
	{{  2,  2 }, { 0, 0 }},	// TOP_RIGHT
	{{  0,  0 }, { 1, 1 }},	// MIDDLE
	{{ -2,  0 }, { 0, 0 }},	// MIDDLE_LEFT
	{{  2,  0 }, { 0, 0 }},	// MIDDLE_RIGHT
	{{  0, -2 }, { 0, 0 }},	// BOTTOM
	{{ -2, -2 }, { 0, 0 }},	// BOTTOM_LEFT
	{{  2, -2 }, { 0, 0 }}	// BOTTOM_RIGHT
};

namespace Geode
{
	AnchorBox::AnchorBox(Context* context, SceneView::Ptr sceneView) : Object(context)
	{
		sceneView_ = sceneView;
		editorScene_ = sceneView->GetEditorScene();
		enabled_ = true;
		editing_ = false;
		homothetics_ = false;
		transform_ = Matrix3x4::IDENTITY;

		boxNode_ = MakeShared<Node>(context_);
		boxNode_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
		boxNode_->SetScale(Vector3(1, 1, 1));
		boxGeometry_ = boxNode_->CreateComponent<CustomGeometry>();
		boxGeometry_->SetOccludee(false);
		boxGeometry_->SetEnabled(false);

		SubscribeToEvent(editorScene_, E_SCENEUPDATE, URHO3D_HANDLER(AnchorBox, HandleSceneUpdate));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGBEGIN, URHO3D_HANDLER(AnchorBox, HandleSceneViewDragBegin));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGMOVE, URHO3D_HANDLER(AnchorBox, HandleSceneViewDragMove));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGEND, URHO3D_HANDLER(AnchorBox, HandleSceneViewDragEnd));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGCANCEL, URHO3D_HANDLER(AnchorBox, HandleSceneViewDragCancel));
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void AnchorBox::SetEnabled(bool enabled)
	{
		if (enabled == enabled_)
		{
			return;
		}

		enabled_ = enabled;
	}

	void AnchorBox::SetHomothetics(bool homothetics)
	{
		homothetics_ = homothetics;
	}

	void AnchorBox::SetCenter(Vector2 center)
	{
		if (center == center_)
		{
			return;
		}

		center_ = center;

		VariantMap sendEventData;
		sendEventData[AnchorBoxCenterChanged::P_X] = center.x_;
		sendEventData[AnchorBoxCenterChanged::P_Y] = center.y_;
		SendEvent(E_ANCHORBOX_CENTERCHANGED, sendEventData);
	}

	void AnchorBox::SetSize(Vector2 size)
	{
		if (size == size_)
		{
			return;
		}

		size_ = size;

		VariantMap sendEventData;
		sendEventData[AnchorBoxSizeChanged::P_WIDTH] = size.x_;
		sendEventData[AnchorBoxSizeChanged::P_HEIGHT] = size.y_;
		SendEvent(E_ANCHORBOX_SIZECHANGED, sendEventData);
	}

	void AnchorBox::SetTransform(Matrix3x4 transform)
	{
		if (transform == transform_)
		{
			return;
		}

		transform_ = transform;
	}

	bool AnchorBox::IsEnabled()
	{
		return enabled_;
	}

	bool AnchorBox::IsEditing()
	{
		return editing_;
	}

	bool AnchorBox::IsHomothetics()
	{
		return homothetics_;
	}

	Vector2 AnchorBox::GetCenter()
	{
		return center_;
	}

	Vector2 AnchorBox::GetSize()
	{
		return size_;
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	void AnchorBox::AddAnchor(AnchorId anchorId)
	{
		anchorsIds_.Insert(anchorId);
	}

	void AnchorBox::RemoveAnchor(AnchorId anchorId)
	{
		if (anchorId == selectedAnchorId_)
		{
			selectedAnchorId_ = -1;
		}

		anchorsIds_.Erase(anchorId);
	}

	bool AnchorBox::HasAnchor(AnchorId anchorId)
	{
		return anchorsIds_.Contains(anchorId);
	}

	int AnchorBox::GetAnchorIdAt(Vector2 position)
	{
		for (auto anchorId : anchorsIds_)
		{
			auto anchorPosition = GetAnchorPosition(anchorId);

			auto left = anchorPosition.x_ - (ANCHOR_SIZE / 2);
			auto top = anchorPosition.y_ + (ANCHOR_SIZE / 2);
			auto right = anchorPosition.x_ + (ANCHOR_SIZE / 2);
			auto bottom = anchorPosition.y_ - (ANCHOR_SIZE / 2);

			if (position.x_ >= left && position.x_ <= right && position.y_ >= bottom && position.y_ <= top)
			{
				return anchorId;
			}
		}

		return -1;
	}

	Vector2 AnchorBox::GetAnchorPosition(int anchorId)
	{
		Vector2 position;

		if (anchorId == AnchorId::TOP)
		{
			position.x_ = center_.x_;
			position.y_ = center_.y_ + (size_.y_ / 2.f);
		}
		else if (anchorId == AnchorId::TOP_LEFT)
		{
			position.x_ = center_.x_ - (size_.x_ / 2.f);
			position.y_ = center_.y_ + (size_.y_ / 2.f);
		}
		else if (anchorId == AnchorId::TOP_RIGHT)
		{
			position.x_ = center_.x_ + (size_.x_ / 2.f);
			position.y_ = center_.y_ + (size_.y_ / 2.f);
		}
		else if (anchorId == AnchorId::MIDDLE)
		{
			position.x_ = center_.x_;
			position.y_ = center_.y_;
		}
		else if (anchorId == AnchorId::MIDDLE_LEFT)
		{
			position.x_ = center_.x_ - (size_.x_ / 2.f);
			position.y_ = center_.y_;
		}
		else if (anchorId == AnchorId::MIDDLE_RIGHT)
		{
			position.x_ = center_.x_ + (size_.x_ / 2.f);
			position.y_ = center_.y_;
		}
		else if (anchorId == AnchorId::BOTTOM)
		{
			position.x_ = center_.x_;
			position.y_ = center_.y_ - (size_.y_ / 2.f);
		}
		else if (anchorId == AnchorId::BOTTOM_LEFT)
		{
			position.x_ = center_.x_ - (size_.x_ / 2.f);
			position.y_ = center_.y_ - (size_.y_ / 2.f);
		}
		else if (anchorId == AnchorId::BOTTOM_RIGHT)
		{
			position.x_ = center_.x_ + (size_.x_ / 2.f);
			position.y_ = center_.y_ - (size_.y_ / 2.f);
		}

		return position;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void AnchorBox::HandleSceneUpdate(StringHash, VariantMap& eventData)
	{
		UpdateRender();
	}

	void AnchorBox::HandleSceneViewDragBegin(StringHash, VariantMap& eventData)
	{
		if (!enabled_)
		{
			return;
		}

		auto mousePosition = transform_.Inverse() * Vector3(eventData[SceneViewDragBegin::P_X].GetFloat(), eventData[SceneViewDragBegin::P_Y].GetFloat());
		auto anchorId = GetAnchorIdAt(Vector2(mousePosition.x_, mousePosition.y_));

		if (anchorId != -1)
		{
			selectedAnchorId_ = anchorId;
			editing_ = true;
			beginEditingCenter_ = center_;
			beginEditingSize_ = size_;
			beginEditingMousePosition_ = Vector2(mousePosition.x_, mousePosition.y_);
		}
		else
		{
			selectedAnchorId_ = -1;
		}
	}

	void AnchorBox::HandleSceneViewDragMove(StringHash, VariantMap& eventData)
	{
		if (!enabled_ || !editing_ || selectedAnchorId_ == -1)
		{
			return;
		}

		auto mousePosition = transform_.Inverse() * Vector3(eventData[SceneViewDragMove::P_X].GetFloat(), eventData[SceneViewDragMove::P_Y].GetFloat());
		auto mouseDelta = Vector2(mousePosition.x_, mousePosition.y_) - beginEditingMousePosition_;

		if (homothetics_)
		{
			size_ = beginEditingSize_ + mouseDelta * Vector2(ANCHOR_HOMOTHETICS_MATRICES[selectedAnchorId_][0]);
			center_ = beginEditingCenter_ + mouseDelta * Vector2(ANCHOR_HOMOTHETICS_MATRICES[selectedAnchorId_][1]);
		}
		else
		{
			size_ = beginEditingSize_ + mouseDelta * Vector2(ANCHOR_MATRICES[selectedAnchorId_][0]);
			center_ = beginEditingCenter_ + mouseDelta * Vector2(ANCHOR_MATRICES[selectedAnchorId_][1]);
		}

		if (size_ != beginEditingSize_)
		{
			VariantMap sendEventData;
			sendEventData[AnchorBoxSizeChanged::P_WIDTH] = size_.x_;
			sendEventData[AnchorBoxSizeChanged::P_HEIGHT] = size_.y_;
			sendEventData[AnchorBoxSizeChanged::P_BEGIN_WIDTH] = beginEditingSize_.x_;
			sendEventData[AnchorBoxSizeChanged::P_BEGIN_HEIGHT] = beginEditingSize_.y_;
			SendEvent(E_ANCHORBOX_SIZECHANGED, sendEventData);
		}

		if (center_ != beginEditingCenter_)
		{
			VariantMap sendEventData;
			sendEventData[AnchorBoxCenterChanged::P_X] = center_.x_;
			sendEventData[AnchorBoxCenterChanged::P_Y] = center_.y_;
			sendEventData[AnchorBoxCenterChanged::P_BEGIN_X] = beginEditingCenter_.x_;
			sendEventData[AnchorBoxCenterChanged::P_BEGIN_Y] = beginEditingCenter_.y_;
			sendEventData[AnchorBoxCenterChanged::P_DX] = beginEditingCenter_.x_ - center_.x_;
			sendEventData[AnchorBoxCenterChanged::P_DY] = beginEditingCenter_.y_ - center_.y_;
			SendEvent(E_ANCHORBOX_CENTERCHANGED, sendEventData);
		}
	}

	void AnchorBox::HandleSceneViewDragEnd(StringHash, VariantMap&)
	{
		if (!enabled_ || !editing_ || selectedAnchorId_ == -1)
		{
			return;
		}

		if (size_ != beginEditingSize_)
		{
			VariantMap sendEventData;
			sendEventData[AnchorBoxConfirmSizeChanged::P_WIDTH] = size_.x_;
			sendEventData[AnchorBoxConfirmSizeChanged::P_HEIGHT] = size_.y_;
			sendEventData[AnchorBoxConfirmSizeChanged::P_BEGIN_WIDTH] = beginEditingSize_.x_;
			sendEventData[AnchorBoxConfirmSizeChanged::P_BEGIN_HEIGHT] = beginEditingSize_.y_;
			SendEvent(E_ANCHORBOX_CONFIRM_SIZECHANGED, sendEventData);
		}

		if (center_ != beginEditingCenter_)
		{
			VariantMap sendEventData;
			sendEventData[AnchorBoxConfirmCenterChanged::P_X] = center_.x_;
			sendEventData[AnchorBoxConfirmCenterChanged::P_Y] = center_.y_;
			sendEventData[AnchorBoxConfirmCenterChanged::P_BEGIN_X] = beginEditingCenter_.x_;
			sendEventData[AnchorBoxConfirmCenterChanged::P_BEGIN_Y] = beginEditingCenter_.y_;
			sendEventData[AnchorBoxConfirmCenterChanged::P_DX] = beginEditingCenter_.x_ - center_.x_;
			sendEventData[AnchorBoxConfirmCenterChanged::P_DY] = beginEditingCenter_.y_ - center_.y_;
			SendEvent(E_ANCHORBOX_CONFIRM_CENTERCHANGED, sendEventData);
		}

		if (size_ != beginEditingSize_ || center_ != beginEditingCenter_)
		{
			VariantMap sendEventData;
			sendEventData[AnchorBoxConfirmChanged::P_X] = center_.x_;
			sendEventData[AnchorBoxConfirmChanged::P_Y] = center_.y_;
			sendEventData[AnchorBoxConfirmChanged::P_BEGIN_X] = beginEditingCenter_.x_;
			sendEventData[AnchorBoxConfirmChanged::P_BEGIN_Y] = beginEditingCenter_.y_;
			sendEventData[AnchorBoxConfirmChanged::P_WIDTH] = size_.x_;
			sendEventData[AnchorBoxConfirmChanged::P_HEIGHT] = size_.y_;
			sendEventData[AnchorBoxConfirmChanged::P_BEGIN_WIDTH] = beginEditingSize_.x_;
			sendEventData[AnchorBoxConfirmChanged::P_BEGIN_HEIGHT] = beginEditingSize_.y_;
			SendEvent(E_ANCHORBOX_CONFIRM_CHANGED, sendEventData);
		}

		editing_ = false;
		selectedAnchorId_ = -1;
	}

	void AnchorBox::HandleSceneViewDragCancel(StringHash, VariantMap&)
	{
		if (!enabled_ || !editing_ || selectedAnchorId_ == -1)
		{
			return;
		}

		size_ = beginEditingSize_;
		center_ = beginEditingCenter_;

		VariantMap sendEventData;
		sendEventData[AnchorBoxCanceled::P_X] = center_.x_;
		sendEventData[AnchorBoxCanceled::P_Y] = center_.y_;
		sendEventData[AnchorBoxCanceled::P_WIDTH] = size_.x_;
		sendEventData[AnchorBoxCanceled::P_HEIGHT] = size_.y_;
		SendEvent(E_ANCHORBOX_CANCELED, sendEventData);

		editing_ = false;
		selectedAnchorId_ = -1;
	}

	///------------------------------------------------------------------------------------------------
	///  UPDATE METHODS
	///------------------------------------------------------------------------------------------------

	void AnchorBox::UpdateRender()
	{
		if (!enabled_)
		{
			boxGeometry_->SetEnabled(false);
			return;
		}

		if (!boxGeometry_->IsEnabled() && editorScene_->GetComponent<Octree>() != nullptr)
		{
			boxGeometry_->SetEnabled(true);
			editorScene_->GetComponent<Octree>()->AddManualDrawable(boxGeometry_);
		}

		auto halfWidth = size_.x_ / 2.f;
		auto halfHeight = size_.y_ / 2.f;
		auto indexGeometry = 0;

		boxGeometry_->Clear();
		boxGeometry_->SetNumGeometries(1 + anchorsIds_.Size());

		boxGeometry_->BeginGeometry(indexGeometry, PrimitiveType::LINE_STRIP);
		boxGeometry_->DefineVertex(Vector3(-halfWidth, halfHeight, BOX_Z_INDEX)); // left-top
		boxGeometry_->DefineColor(BOX_COLOR);
		boxGeometry_->DefineVertex(Vector3(halfWidth, halfHeight, BOX_Z_INDEX)); // right-top
		boxGeometry_->DefineColor(BOX_COLOR);
		boxGeometry_->DefineVertex(Vector3(halfWidth, -halfHeight, BOX_Z_INDEX)); // right-bottom
		boxGeometry_->DefineColor(BOX_COLOR);
		boxGeometry_->DefineVertex(Vector3(-halfWidth, -halfHeight, BOX_Z_INDEX)); // left-bottom
		boxGeometry_->DefineColor(BOX_COLOR);
		boxGeometry_->DefineVertex(Vector3(-halfWidth, halfHeight, BOX_Z_INDEX)); // left-top
		boxGeometry_->DefineColor(BOX_COLOR);
		boxGeometry_->SetMaterial(indexGeometry, GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/VColUnlit.xml"));

		for (auto anchorId : anchorsIds_)
		{
			indexGeometry++;

			auto anchorColor = (anchorId == selectedAnchorId_) ? SELECTED_ANCHOR_COLOR : ANCHOR_COLOR;
			auto anchorHalfSize = ANCHOR_SIZE / 2.f;
			auto anchorCenter = Vector2();

			if (anchorId == AnchorId::TOP)
			{
				anchorCenter.x_ = 0;
				anchorCenter.y_ = halfHeight;
			}
			else if (anchorId == AnchorId::TOP_LEFT)
			{
				anchorCenter.x_ = -halfWidth;
				anchorCenter.y_ = halfHeight;
			}
			else if (anchorId == AnchorId::TOP_RIGHT)
			{
				anchorCenter.x_ = halfWidth;
				anchorCenter.y_ = halfHeight;
			}
			else if (anchorId == AnchorId::MIDDLE)
			{
				anchorCenter.x_ = 0;
				anchorCenter.y_ = 0;
			}
			else if (anchorId == AnchorId::MIDDLE_LEFT)
			{
				anchorCenter.x_ = -halfWidth;
				anchorCenter.y_ = 0;
			}
			else if (anchorId == AnchorId::MIDDLE_RIGHT)
			{
				anchorCenter.x_ = halfWidth;
				anchorCenter.y_ = 0;
			}
			else if (anchorId == AnchorId::BOTTOM)
			{
				anchorCenter.x_ = 0;
				anchorCenter.y_ = -halfHeight;
			}
			else if (anchorId == AnchorId::BOTTOM_LEFT)
			{
				anchorCenter.x_ = -halfWidth;
				anchorCenter.y_ = -halfHeight;
			}
			else if (anchorId == AnchorId::BOTTOM_RIGHT)
			{
				anchorCenter.x_ = halfWidth;
				anchorCenter.y_ = -halfHeight;
			}

			boxGeometry_->BeginGeometry(indexGeometry, PrimitiveType::TRIANGLE_STRIP);
			boxGeometry_->DefineVertex(Vector3(anchorCenter.x_ - anchorHalfSize, anchorCenter.y_ - anchorHalfSize, ANCHOR_Z_INDEX)); // left-bottom
			boxGeometry_->DefineColor(anchorColor);
			boxGeometry_->DefineVertex(Vector3(anchorCenter.x_ - anchorHalfSize, anchorCenter.y_ + anchorHalfSize, ANCHOR_Z_INDEX)); // left-top
			boxGeometry_->DefineColor(anchorColor);
			boxGeometry_->DefineVertex(Vector3(anchorCenter.x_ + anchorHalfSize, anchorCenter.y_ - anchorHalfSize, ANCHOR_Z_INDEX)); // right-bottom
			boxGeometry_->DefineColor(anchorColor);
			boxGeometry_->DefineVertex(Vector3(anchorCenter.x_ + anchorHalfSize, anchorCenter.y_ + anchorHalfSize, ANCHOR_Z_INDEX)); // right-top
			boxGeometry_->DefineColor(anchorColor);
			boxGeometry_->SetMaterial(indexGeometry, GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/VColUnlit.xml"));
		}

		boxGeometry_->Commit();

		boxNode_->SetTransform(transform_);
		boxNode_->Translate2D(center_, TS_LOCAL);
	}
}