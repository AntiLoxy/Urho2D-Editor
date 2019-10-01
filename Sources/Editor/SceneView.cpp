#include "SceneView.h"
#include "SceneViewEvents.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Urho2D/Drawable2D.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>

using namespace Urho3D;

static const float CAMERA_MOVE_SPEED = 10.f;
static const float CAMERA_DISTANCE = -10.f;

namespace Geode
{
	SceneView::SceneView(Context* context, UIElement* parent, EditorScene::Ptr editorScene) : IView(context, "UI/SceneView.xml", parent)
	{
		editorScene_ = editorScene;
		debugGeometryEnabled_ = true;
		nodeSelectionEnabled_ = true;
		dragging_ = false;

		// Init camera.
		// ----------------------------------------------------------------------------------------------------------------
		cameraNode_ = MakeShared<Node>(context_);
		cameraNode_->SetPosition(Vector3(0.0f, 0.0f, CAMERA_DISTANCE));
		
		camera_ = cameraNode_->CreateComponent<Camera>();
		camera_->SetOrthographic(true);
		camera_->SetOrthoSize((float)elRoot_->GetHeight() * PIXEL_SIZE);

		// Init viewport.
		// ----------------------------------------------------------------------------------------------------------------
		IntRect viewportRect;
		viewportRect.left_ = elRoot_->GetScreenPosition().x_;
		viewportRect.top_ = elRoot_->GetScreenPosition().y_;
		viewportRect.right_ = viewportRect.left_ + elRoot_->GetWidth();
		viewportRect.bottom_ = viewportRect.top_ + elRoot_->GetHeight();
		
		viewport_ = MakeShared<Viewport>(context_);
		viewport_->SetCamera(camera_);
		viewport_->SetRect(viewportRect);
		viewport_->SetScene(editorScene_);

		auto renderer = GetSubsystem<Renderer>();
		renderer->SetViewport(0, viewport_);

		// Init node position gizmo.
		// ----------------------------------------------------------------------------------------------------------------
		gizmo_ = MakeShared<Gizmo>(context_, editorScene_);

		// Init grid.
		// ----------------------------------------------------------------------------------------------------------------
		grid_ = MakeShared<Grid>(context_, editorScene_);

		// Init all events.
		// ----------------------------------------------------------------------------------------------------------------
		SubscribeToEvent(elRoot_, E_RESIZED, URHO3D_HANDLER(SceneView, HandleResized));
		SubscribeToEvent(editorScene_, E_SELECTEDOBJECTCHANGED, URHO3D_HANDLER(SceneView, HandleSelectedObjectChanged));
		SubscribeToEvent(editorScene_, E_SCENEUPDATE, URHO3D_HANDLER(SceneView, HandleSceneUpdate));
		SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(SceneView, HandleScenePostRender));
		SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(SceneView, HandleMouseButtonDown));
		SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(SceneView, HandleMouseMove));
		SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(SceneView, HandleMouseButtonUp));
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void SceneView::SetDebugGeometryEnabled(bool enabled)
	{
		if (enabled != debugGeometryEnabled_)
		{
			debugGeometryEnabled_ = enabled;
		}
	}

	void SceneView::SetNodeSelectionEnabled(bool enabled)
	{
		if (enabled != nodeSelectionEnabled_)
		{
			nodeSelectionEnabled_ = enabled;
		}
	}

	void SceneView::SetNodePositionGizmoEnabled(bool enabled)
	{
		gizmo_->SetEnabled(enabled);
	}

	void SceneView::SetGridEnabled(bool enabled)
	{
		grid_->SetEnabled(enabled);
	}

	EditorScene::Ptr SceneView::GetEditorScene()
	{
		return editorScene_;
	}

	bool SceneView::GetDebugGeometryEnabled()
	{
		return debugGeometryEnabled_;
	}

	bool SceneView::GetNodeSelectionEnabled()
	{
		return nodeSelectionEnabled_;
	}

	bool SceneView::GetNodePositionGizmoEnabled()
	{
		return gizmo_->IsEnabled();
	}

	bool SceneView::GetGridEnabled()
	{
		return grid_->IsEnabled();
	}

	Vector2 SceneView::GetMouseMove()
	{
		return mouseWorldMove_;
	}

	bool SceneView::IsDragging()
	{
		return dragging_;
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	Vector2 SceneView::GetMouseWorldPosition()
	{
		auto ui = GetSubsystem<UI>();
		auto mousePosition = ui->GetCursorPosition();
		auto viewportRect = viewport_->GetRect();
		auto viewportWidth = viewportRect.Width();
		auto viewportHeight = viewportRect.Height();
		auto relX = static_cast<float>(mousePosition.x_ - viewportRect.left_) / viewportWidth;
		auto relY = static_cast<float>(mousePosition.y_ - viewportRect.top_) / viewportHeight;
		auto pos = camera_->ScreenToWorldPoint(Vector3(relX, relY, 0));

		return Vector2(pos.x_, pos.y_);
	}

	bool SceneView::GetMouseButtonDown(MouseButtonFlags button)
	{
		auto ui = GetSubsystem<UI>();
		auto input = GetSubsystem<Input>();

		if (!IsMouseInsideViewport() || ui->GetFocusElement())
		{
			return false;
		}

		return input->GetMouseButtonDown(button);
	}

	bool SceneView::GetMouseButtonPress(MouseButtonFlags button)
	{
		auto ui = GetSubsystem<UI>();
		auto input = GetSubsystem<Input>();

		if (!IsMouseInsideViewport() || ui->GetFocusElement())
		{
			return false;
		}

		return input->GetMouseButtonPress(button);
	}

	bool SceneView::IsMouseInsideNode(Node* node)
	{
		auto nodeRect = editorScene_->GetNodeWorldRect(node);
		auto mouseWorldPosition = GetMouseWorldPosition();

		return nodeRect.IsInside(mouseWorldPosition);
	}

	bool SceneView::IsMouseInsideViewport()
	{
		auto ui = GetSubsystem<UI>();
		auto viewportRect = viewport_->GetRect();
		auto mousePosition = ui->GetCursorPosition();

		return viewportRect.IsInside(mousePosition);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void SceneView::HandleResized(StringHash, VariantMap&)
	{
		IntRect viewportRect;
		viewportRect.left_ = elRoot_->GetScreenPosition().x_;
		viewportRect.top_ = elRoot_->GetScreenPosition().y_;
		viewportRect.right_ = viewportRect.left_ + elRoot_->GetWidth();
		viewportRect.bottom_ = viewportRect.top_ + elRoot_->GetHeight();

		viewport_->SetRect(viewportRect);
		camera_->SetOrthoSize((float)elRoot_->GetHeight() * PIXEL_SIZE);
	}

	void SceneView::HandleSceneUpdate(StringHash, VariantMap& eventData)
	{
		float ts = eventData[SceneUpdate::P_TIMESTEP].GetFloat();

		UpdateCameraMoves(ts);
		UpdateNodeSelection(ts);
		UpdateMouseMove(ts);
	}

	void SceneView::HandleSelectedObjectChanged(StringHash, VariantMap&)
	{
		auto selectedObject = editorScene_->GetSelectedObject();

		if (selectedObject == nullptr)
		{
			gizmo_->SetAttachedNode(nullptr);
		}
		else if (selectedObject->IsInstanceOf<Node>())
		{
			gizmo_->SetAttachedNode(static_cast<Node*>(selectedObject));
		}
		else if (selectedObject->IsInstanceOf<Component>())
		{
			gizmo_->SetAttachedNode(static_cast<Component*>(selectedObject)->GetNode());
		}
	}

	void SceneView::HandleScenePostRender(StringHash, VariantMap&)
	{
		if (debugGeometryEnabled_ && editorScene_->HasComponent("DebugRenderer") && editorScene_->HasComponent("PhysicsWorld2D"))
		{
			editorScene_->GetComponent<PhysicsWorld2D>()->DrawDebugGeometry(editorScene_->GetComponent<DebugRenderer>(), false);
		}
	}

	void SceneView::HandleMouseButtonDown(StringHash, VariantMap& eventData)
	{
		auto ui = GetSubsystem<UI>();

		if (!IsMouseInsideViewport() || ui->GetFocusElement())
		{
			return;
		}

		auto mouseWorldPosition = GetMouseWorldPosition();
		auto button = eventData[MouseButtonDown::P_BUTTON].GetInt();

		if (button == MOUSEB_LEFT || button == MOUSEB_RIGHT)
		{
			auto sendEventData = eventData;
			sendEventData[SceneViewClick::P_X] = mouseWorldPosition.x_;
			sendEventData[SceneViewClick::P_Y] = mouseWorldPosition.y_;
			SendEvent(E_SCENEVIEW_CLICK, sendEventData);
		}

		if (!dragging_ && button == MOUSEB_LEFT)
		{
			dragging_ = true;
			beginDragMouseWorldPosition_ = mouseWorldPosition;
			auto sendEventData = eventData;
			sendEventData[SceneViewDragBegin::P_X] = mouseWorldPosition.x_;
			sendEventData[SceneViewDragBegin::P_Y] = mouseWorldPosition.y_;
			SendEvent(E_SCENEVIEW_DRAGBEGIN, sendEventData);
		}

		if (dragging_ && button == MOUSEB_RIGHT)
		{
			dragging_ = false;
			auto sendEventData = eventData;
			sendEventData[SceneViewDragCancel::P_X] = mouseWorldPosition.x_;
			sendEventData[SceneViewDragCancel::P_Y] = mouseWorldPosition.y_;
			SendEvent(E_SCENEVIEW_DRAGCANCEL, sendEventData);
		}
	}

	void SceneView::HandleMouseMove(StringHash, VariantMap& eventData)
	{
		auto ui = GetSubsystem<UI>();

		if (!IsMouseInsideViewport() || ui->GetFocusElement())
		{
			return;
		}

		auto mouseWorldPosition = GetMouseWorldPosition();
		auto dragMouseDelta = mouseWorldPosition - beginDragMouseWorldPosition_;

		if (dragging_)
		{
			auto sendEventData = eventData;
			sendEventData[SceneViewDragMove::P_X] = mouseWorldPosition.x_;
			sendEventData[SceneViewDragMove::P_Y] = mouseWorldPosition.y_;
			sendEventData[SceneViewDragMove::P_DX] = dragMouseDelta.x_;
			sendEventData[SceneViewDragMove::P_DY] = dragMouseDelta.y_;
			SendEvent(E_SCENEVIEW_DRAGMOVE, sendEventData);
		}
	}

	void SceneView::HandleMouseButtonUp(StringHash, VariantMap& eventData)
	{
		auto ui = GetSubsystem<UI>();

		if (!IsMouseInsideViewport() || ui->GetFocusElement())
		{
			return;
		}

		auto mouseWorldPosition = GetMouseWorldPosition();
		auto button = eventData[MouseButtonUp::P_BUTTON].GetInt();

		if (button == MOUSEB_LEFT || button == MOUSEB_RIGHT)
		{
			auto sendEventData = eventData;
			sendEventData[SceneViewClick::P_X] = mouseWorldPosition.x_;
			sendEventData[SceneViewClick::P_Y] = mouseWorldPosition.y_;
			SendEvent(E_SCENEVIEW_CLICKEND, sendEventData);
		}

		if (dragging_ && button == MOUSEB_LEFT)
		{
			dragging_ = false;
			auto sendEventData = eventData;
			sendEventData[SceneViewDragCancel::P_X] = mouseWorldPosition.x_;
			sendEventData[SceneViewDragCancel::P_Y] = mouseWorldPosition.y_;
			SendEvent(E_SCENEVIEW_DRAGEND, sendEventData);
		}
	}

	///------------------------------------------------------------------------------------------------
	///  UPDATE METHODS
	///------------------------------------------------------------------------------------------------

	void SceneView::UpdateCameraMoves(float ts)
	{
		auto ui = GetSubsystem<UI>();
		auto input = GetSubsystem<Input>();

		if (ui->GetFocusElement())
		{
			return;
		}

		if (input->GetKeyDown(KEY_LEFT))
		{
			cameraNode_->Translate(Vector3::LEFT * CAMERA_MOVE_SPEED * ts);
		}

		if (input->GetKeyDown(KEY_RIGHT))
		{
			cameraNode_->Translate(Vector3::RIGHT * CAMERA_MOVE_SPEED * ts);
		}

		if (input->GetKeyDown(KEY_UP))
		{
			cameraNode_->Translate(Vector3::UP * CAMERA_MOVE_SPEED * ts);
		}

		if (input->GetKeyDown(KEY_DOWN))
		{
			cameraNode_->Translate(Vector3::DOWN * CAMERA_MOVE_SPEED * ts);
		}
	}

	void SceneView::UpdateNodeSelection(float ts)
	{
		if (!GetMouseButtonDown(MOUSEB_LEFT))
		{
			return;
		}

		if (!nodeSelectionEnabled_)
		{
			return;
		}

		if (editorScene_->GetSelectedComponent() != nullptr)
		{
			return;
		}

		auto node = editorScene_->GetNodeAt(GetMouseWorldPosition());

		if (node == nullptr)
		{
			return;
		}

		editorScene_->SetSelectedObject(node);
	}

	void SceneView::UpdateMouseMove(float ts)
	{
		mouseWorldMove_ = GetMouseWorldPosition() - lastMouseWorldPosition_;
		lastMouseWorldPosition_ = GetMouseWorldPosition();
	}
}
