/**
 * @file    Grid.h
 * @ingroup Editor
 * @brief   Scene 2D grid.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "EditorScene.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Graphics/Octree.h>

namespace Geode
{
	class Grid : public Urho3D::Object
	{
		URHO3D_OBJECT(Grid, Urho3D::Object)

		enum Layers {
			SUBDIVISION,
			MAIN,
			CENTER,
			LENGTH
		};

		struct Line {
			Urho3D::Vector3 from;
			Urho3D::Vector3 dest;
			Urho3D::Color color;
		};

	public:
		/// Constructors.
		explicit Grid(Urho3D::Context* context, Geode::EditorScene::Ptr editorScene);

		/// Accessors & Mutators.
		void SetEnabled(bool enabled_);
		bool IsEnabled();

	private:
		/// Event handlers.
		void HandleSceneUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleSceneLoaded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		/// Update methods.
		void UpdateRender();

	private:
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::SharedPtr<Urho3D::Node> gridNode_;
		Urho3D::CustomGeometry* gridGeometry_;
		bool enabled_;
		unsigned int extend_;
		float spacing_;
		unsigned int subdivision_;
	};
}