#include "CollisionPolygon2DTool.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Input/InputEvents.h>

using namespace Urho3D;

static const String INSERT_CURSOR_SHAPE = "Cross";
static const unsigned int VERTEX_CIRCLE_STEPS = 10;
static const int VERTEX_CIRCLE_Z_INDEX = -3;
static const float VERTEX_CIRCLE_RADIUS = 0.05f;
static const Color VERTEX_CIRCLE_COLOR = Color(0, 0, 1);
static const float SELECTED_VERTEX_CIRCLE_RADIUS = 0.07f;
static const Color SELECTED_VERTEX_CIRCLE_COLOR = Color(1, 0, 0);

namespace Geode
{
	CollisionPolygon2DTool::CollisionPolygon2DTool(Context* context, CommandHistory::Ptr commandHistory, SceneView::Ptr sceneView) : ITool(context)
	{
		commandHistory_ = commandHistory;
		sceneView_ = sceneView;
		editorScene_ = sceneView->GetEditorScene();
		cursor_ = GetSubsystem<UI>()->GetCursor();
		selectedCollisionPolygon2D_ = nullptr;
		mode_ = Mode::INSERTION;
		selectedVertexIndex_ = -1;

		pathNode_ = MakeShared<Node>(context_);
		pathGeometry_ = pathNode_->CreateComponent<CustomGeometry>();
		pathGeometry_->SetEnabled(false);

		SetSelectedCollisionPolygon2D(editorScene_->GetSelectedObjectAs<CollisionPolygon2D>());

		SubscribeToEvent(editorScene_, E_SCENEUPDATE, URHO3D_HANDLER(CollisionPolygon2DTool, HandleSceneUpdate));
		SubscribeToEvent(editorScene_, E_SELECTEDOBJECTCHANGED, URHO3D_HANDLER(CollisionPolygon2DTool, HandleSelectedObjectChanged));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGBEGIN, URHO3D_HANDLER(CollisionPolygon2DTool, HandleSceneViewDragBegin));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGMOVE, URHO3D_HANDLER(CollisionPolygon2DTool, HandleSceneViewDragMove));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGEND, URHO3D_HANDLER(CollisionPolygon2DTool, HandleSceneViewDragEnd));
		SubscribeToEvent(sceneView_, E_SCENEVIEW_DRAGCANCEL, URHO3D_HANDLER(CollisionPolygon2DTool, HandleSceneViewDragCancel));
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(CollisionPolygon2DTool, HandleKeyDown));
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void CollisionPolygon2DTool::SetMode(Mode mode)
	{
		if (mode != mode_)
		{
			mode_ = mode;
		}
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	UIElement* CollisionPolygon2DTool::PopulateToolBar(ToolBar* toolBar)
	{
		return nullptr;
	}

	bool CollisionPolygon2DTool::IsActive()
	{
		return IsEnabled() && selectedCollisionPolygon2D_ != nullptr;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void CollisionPolygon2DTool::HandleSceneUpdate(StringHash, VariantMap&)
	{
		UpdateCursor();
		UpdateComponent();
		UpdateRender();
	}

	void CollisionPolygon2DTool::HandleSelectedObjectChanged(StringHash, VariantMap&)
	{
		SetSelectedCollisionPolygon2D(editorScene_->GetSelectedObjectAs<CollisionPolygon2D>());
	}

	void CollisionPolygon2DTool::HandleKeyDown(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto key = eventData[KeyDown::P_KEY].GetInt();

		if (key == KEY_I)
		{
			mode_ = Mode::INSERTION;
		}
		else if (key == KEY_E)
		{
			mode_ = Mode::EDITION;
		}
		else if (key == KEY_DELETE && mode_ == Mode::EDITION && selectedVertexIndex_ != -1)
		{
			CommandInvoker(MakeShared<DeleteVertexCollisionPolygon2DCommand>(editorScene_, selectedVertexIndex_), commandHistory_).Exec();
			selectedVertexIndex_ = -1;
		}
	}

	void CollisionPolygon2DTool::HandleSceneViewDragBegin(StringHash, VariantMap& eventData)
	{
		if (!IsActive())
		{
			return;
		}

		auto transform = selectedCollisionPolygon2D_->GetNode()->GetTransform();
		auto mousePosition = transform.Inverse() * Vector3(eventData[SceneViewDragBegin::P_X].GetFloat(), eventData[SceneViewDragBegin::P_Y].GetFloat());

		if (mode_ == Mode::EDITION)
		{
			auto vertexIndex = GetVertexIndexAt(Vector2(mousePosition.x_, mousePosition.y_));

			if (vertexIndex != -1)
			{
				selectedVertexIndex_ = vertexIndex;
				beginEditingVertexPosition_ = selectedCollisionPolygon2D_->GetVertex(vertexIndex);
				beginEditingMousePosition_ = Vector2(mousePosition.x_, mousePosition.y_);
			}
			else
			{
				selectedVertexIndex_ = -1;
			}
		}
		else if (mode_ == Mode::INSERTION)
		{
			auto vertex = Vector2(mousePosition.x_, mousePosition.y_);
			CommandInvoker(MakeShared<AddVertexCollisionPolygon2DCommand>(editorScene_, vertex), commandHistory_).Exec();
		}
	}

	void CollisionPolygon2DTool::HandleSceneViewDragMove(StringHash, VariantMap& eventData)
	{
		if (!IsActive() || selectedVertexIndex_ == -1)
		{
			return;
		}

		auto transform = selectedCollisionPolygon2D_->GetNode()->GetTransform();
		auto mousePosition = transform.Inverse() * Vector3(eventData[SceneViewDragMove::P_X].GetFloat(), eventData[SceneViewDragMove::P_Y].GetFloat());
		auto mouseDelta = Vector2(mousePosition.x_, mousePosition.y_) - beginEditingMousePosition_;
		auto vertex = beginEditingVertexPosition_ + mouseDelta;

		auto vertices = selectedCollisionPolygon2D_->GetVertices();
		vertices[selectedVertexIndex_] = vertex;
		selectedCollisionPolygon2D_->SetVertices(vertices);
	}

	void CollisionPolygon2DTool::HandleSceneViewDragEnd(StringHash, VariantMap& eventData)
	{
		if (!IsActive() || selectedVertexIndex_ == -1)
		{
			return;
		}

		auto transform = selectedCollisionPolygon2D_->GetNode()->GetTransform();
		auto mousePosition = transform.Inverse() * Vector3(eventData[SceneViewDragEnd::P_X].GetFloat(), eventData[SceneViewDragEnd::P_Y].GetFloat());
		auto mouseDelta = Vector2(mousePosition.x_, mousePosition.y_) - beginEditingMousePosition_;

		if (mode_ == Mode::EDITION && mouseDelta != Vector2::ZERO)
		{
			auto vertex = beginEditingVertexPosition_ + mouseDelta;
			CommandInvoker(MakeShared<MoveVertexCollisionPolygon2DCommand>(editorScene_, selectedVertexIndex_, beginEditingVertexPosition_, vertex), commandHistory_).Exec();
		}
	}

	void CollisionPolygon2DTool::HandleSceneViewDragCancel(StringHash, VariantMap&)
	{
		if (!IsActive() || selectedVertexIndex_ == -1)
		{
			return;
		}

		auto vertices = selectedCollisionPolygon2D_->GetVertices();
		vertices[selectedVertexIndex_] = beginEditingVertexPosition_;
		selectedCollisionPolygon2D_->SetVertices(vertices);

		selectedVertexIndex_ = -1;
	}

	///------------------------------------------------------------------------------------------------
	///  UPDATE METHODS
	///------------------------------------------------------------------------------------------------

	void CollisionPolygon2DTool::UpdateCursor()
	{
		if (!IsActive())
		{
			return;
		}

		if (sceneView_->IsMouseInsideViewport() && mode_ == Mode::INSERTION)
		{
			cursor_->SetShape(INSERT_CURSOR_SHAPE);
		}
	}

	void CollisionPolygon2DTool::UpdateComponent()
	{
		if (!IsActive())
		{
			return;
		}

		if (selectedCollisionPolygon2D_->GetVertexCount() >= 3)
		{
			selectedCollisionPolygon2D_->SetEnabled(true);
		}
		else
		{
			selectedCollisionPolygon2D_->SetEnabled(false);
		}
	}

	void CollisionPolygon2DTool::UpdateRender()
	{
		if (!IsActive())
		{
			pathGeometry_->SetEnabled(false);
			return;
		}

		if (!pathGeometry_->IsEnabled() && editorScene_->GetComponent<Octree>() != nullptr)
		{
			pathGeometry_->SetEnabled(true);
			editorScene_->GetComponent<Octree>()->AddManualDrawable(pathGeometry_);
		}

		auto vertices = selectedCollisionPolygon2D_->GetVertices();

		pathGeometry_->Clear();
		pathGeometry_->SetNumGeometries(vertices.Size());

		for (int i = 0; i < vertices.Size(); i++)
		{
			auto vertex = Vector3(vertices[i], VERTEX_CIRCLE_Z_INDEX);
			auto color = (i == selectedVertexIndex_) ? SELECTED_VERTEX_CIRCLE_COLOR : VERTEX_CIRCLE_COLOR;
			auto radius = (i == selectedVertexIndex_) ? SELECTED_VERTEX_CIRCLE_RADIUS : VERTEX_CIRCLE_RADIUS;

			pathGeometry_->BeginGeometry(i, PrimitiveType::TRIANGLE_STRIP);
			pathGeometry_->DefineVertex(vertex);

			for (int j = VERTEX_CIRCLE_STEPS; j >= 0; j--)
			{
				auto angle = ((float)j / (float)VERTEX_CIRCLE_STEPS) * 360.0f;
				auto p = Vector3(radius * Cos<float>(angle), radius * Sin<float>(angle));

				pathGeometry_->DefineVertex(vertex + p);
				pathGeometry_->DefineColor(color);
				pathGeometry_->DefineVertex(vertex);
				pathGeometry_->DefineColor(color);
			}

			pathGeometry_->SetMaterial(i, GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/VColUnlit.xml"));
		}

		pathGeometry_->Commit();

		auto transform = selectedCollisionPolygon2D_->GetNode()->GetTransform();
		pathNode_->SetTransform(transform);
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void CollisionPolygon2DTool::SetSelectedCollisionPolygon2D(CollisionPolygon2D* selectedCollisionPolygon2D)
	{
		if (selectedCollisionPolygon2D == nullptr)
		{
			SetEnabled(false);
		}
		else
		{
			SetEnabled(true);
		}

		selectedCollisionPolygon2D_ = selectedCollisionPolygon2D;
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	int CollisionPolygon2DTool::GetVertexIndexAt(Vector2 position)
	{
		if (!IsActive())
		{
			return -1;
		}

		auto vertices = selectedCollisionPolygon2D_->GetVertices();

		for (unsigned int i = 0; i < vertices.Size(); i++)
		{
			auto delta = vertices[i] - position;
			auto radius = (i == selectedVertexIndex_) ? SELECTED_VERTEX_CIRCLE_RADIUS : VERTEX_CIRCLE_RADIUS;

			if (sqrt((delta.x_ * delta.x_) + (delta.y_ * delta.y_)) <= radius)
			{
				return i;
			}
		}

		return -1;
	}
}