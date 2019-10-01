#include "Gizmo.h"

#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>

using namespace Urho3D;

namespace Geode
{
	Gizmo::Gizmo(Context* context, EditorScene::Ptr editorScene) : Object(context)
	{
		auto cache = GetSubsystem<ResourceCache>();

		editorScene_ = editorScene;
		enabled_ = true;
		attachedNode_ = nullptr;

		gizmoNode_ = MakeShared<Node>(context_);
		gizmoNode_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
		gizmoNode_->SetScale(Vector3(1, 1, 1));

		xAxisModel_ = gizmoNode_->CreateComponent<StaticModel>();
		xAxisModel_->SetModel(cache->GetResource<Model>("Models/xAxis.mdl"));
		xAxisModel_->SetMaterial(0, cache->GetResource<Material>("Materials/RedUnlit.xml"));
		xAxisModel_->SetOccludee(false);
		xAxisModel_->SetEnabled(false);

		yAxisModel_ = gizmoNode_->CreateComponent<StaticModel>();
		yAxisModel_->SetModel(cache->GetResource<Model>("Models/yAxis.mdl"));
		yAxisModel_->SetMaterial(0, cache->GetResource<Material>("Materials/GreenUnlit.xml"));
		yAxisModel_->SetOccludee(false);
		yAxisModel_->SetEnabled(false);

		SubscribeToEvent(editorScene_, E_SCENEUPDATE, URHO3D_HANDLER(Gizmo, HandleSceneUpdate));
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void Gizmo::SetEnabled(bool enabled)
	{
		if (enabled != enabled_)
		{
			enabled_ = enabled;
		}
	}

	void Gizmo::SetAttachedNode(Node* node)
	{
		if (node != attachedNode_)
		{
			attachedNode_ = node;
		}
	}

	bool Gizmo::IsEnabled()
	{
		return enabled_;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void Gizmo::HandleSceneUpdate(StringHash, VariantMap&)
	{
		UpdateRender();
	}

	///------------------------------------------------------------------------------------------------
	///  UPDATE METHODS
	///------------------------------------------------------------------------------------------------

	void Gizmo::UpdateRender()
	{
		if (!enabled_ || attachedNode_ == nullptr)
		{
			xAxisModel_->SetEnabled(false);
			yAxisModel_->SetEnabled(false);
			return;
		}

		if (!xAxisModel_->IsEnabled() && !yAxisModel_->IsEnabled() && editorScene_->GetComponent<Octree>() != nullptr)
		{
			xAxisModel_->SetEnabled(true);
			yAxisModel_->SetEnabled(true);
			editorScene_->GetComponent<Octree>()->AddManualDrawable(xAxisModel_);
			editorScene_->GetComponent<Octree>()->AddManualDrawable(yAxisModel_);
		}

		gizmoNode_->SetPosition2D(attachedNode_->GetPosition2D());
		gizmoNode_->SetRotation2D(attachedNode_->GetRotation2D());
	}
}