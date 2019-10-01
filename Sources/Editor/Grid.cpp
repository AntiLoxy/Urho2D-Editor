#include "Grid.h"

#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Material.h>

using namespace Urho3D;

static const Color SUBDIVISION_LINE_COLOR = Color(0, 0, 0);
static const Color MAIN_LINE_COLOR = Color(150, 150, 0);
static const Color CENTER_LINE_COLOR = Color(255, 0, 0);
static const int LINE_Z_INDEX = 0;

namespace Geode
{
	Grid::Grid(Context* context, EditorScene::Ptr editorScene) : Object(context)
	{
		editorScene_ = editorScene;
		enabled_ = true;
		extend_ = 7;
		spacing_ = 1;
		subdivision_ = 2;

		gridNode_ = MakeShared<Node>(context_);
		gridNode_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
		gridNode_->SetScale(Vector3(1, 1, 1));

		gridGeometry_ = gridNode_->CreateComponent<CustomGeometry>();
		gridGeometry_->SetOccludee(false);
		gridGeometry_->SetEnabled(false);

		SubscribeToEvent(editorScene_, E_SCENEUPDATE, URHO3D_HANDLER(Grid, HandleSceneUpdate));
		SubscribeToEvent(editorScene_, E_SCENELOADED, URHO3D_HANDLER(Grid, HandleSceneLoaded));
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void Grid::SetEnabled(bool enabled)
	{
		if (enabled != enabled_)
		{
			enabled_ = enabled;
		}
	}

	bool Grid::IsEnabled()
	{
		return enabled_;
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT HANDLERS
	///------------------------------------------------------------------------------------------------

	void Grid::HandleSceneUpdate(StringHash, VariantMap&)
	{
		UpdateRender();
	}

	void Grid::HandleSceneLoaded(StringHash, VariantMap&)
	{
		gridGeometry_->SetEnabled(false);
	}

	///------------------------------------------------------------------------------------------------
	///  UPDATE METHODS
	///------------------------------------------------------------------------------------------------

	void Grid::UpdateRender()
	{
		if (!enabled_)
		{
			gridGeometry_->SetEnabled(false);
			return;
		}

		if (!gridGeometry_->IsEnabled() && editorScene_->GetComponent<Octree>() != nullptr)
		{
			gridGeometry_->SetEnabled(true);
			editorScene_->GetComponent<Octree>()->AddManualDrawable(gridGeometry_);
		}

		float nbCells = extend_ * subdivision_ * 2;
		float cellSize = spacing_ / subdivision_;
		float gridSize = nbCells * cellSize;
		float left = -gridSize / 2;
		float top = -gridSize / 2;
		float offset = 0;

		Vector<Vector<Line>> layers(Layers::LENGTH);

		for (unsigned int i = 0; i <= nbCells; i++)
		{
			bool isSubdivision = i % subdivision_ != 0;
			bool isCenter = i == nbCells / 2;
			Color color = isSubdivision ? SUBDIVISION_LINE_COLOR : (isCenter ? CENTER_LINE_COLOR : MAIN_LINE_COLOR);

			Line vLine;
			vLine.from = Vector3(left + offset, top, LINE_Z_INDEX);
			vLine.dest = Vector3(left + offset, top + gridSize, LINE_Z_INDEX);
			vLine.color = color;

			Line hLine;
			hLine.from = Vector3(left, top + offset, LINE_Z_INDEX);
			hLine.dest = Vector3(left + gridSize, top + offset, LINE_Z_INDEX);
			hLine.color = color;

			if (isCenter)
			{
				layers[Layers::CENTER].Push(vLine);
				layers[Layers::CENTER].Push(hLine);
			}
			else if (isSubdivision)
			{
				layers[Layers::SUBDIVISION].Push(vLine);
				layers[Layers::SUBDIVISION].Push(hLine);
			}
			else
			{
				layers[Layers::MAIN].Push(vLine);
				layers[Layers::MAIN].Push(hLine);
			}

			offset += cellSize;
		}

		gridGeometry_->Clear();
		gridGeometry_->SetNumGeometries(layers.Size());

		for (int i = 0; i < Layers::LENGTH; i++)
		{
			gridGeometry_->BeginGeometry(i, PrimitiveType::LINE_LIST);

			for (int j = 0; j < layers[i].Size(); j++)
			{
				auto line = layers[i][j];

				gridGeometry_->DefineVertex(line.from);
				gridGeometry_->DefineVertex(line.dest);
				gridGeometry_->DefineColor(line.color);
			}

			gridGeometry_->SetMaterial(i, GetSubsystem<ResourceCache>()->GetResource<Material>("Materials/VColUnlit.xml"));
		}

		gridGeometry_->Commit();
	}
}