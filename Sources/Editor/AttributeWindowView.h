/**
 * @file    AttributeWindowView.h
 * @ingroup Editor
 * @brief   WindowView to edit attributes of a serializable.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include "EditorScene.h"
#include "Commands.h"
#include "../Gui/IWindowView.h"
#include "../Gui/ReactiveForm.h"

#include <Urho3D/Core/Context.h>

namespace Geode
{
	class AttributeWindowView : public Geode::IWindowView
	{
		URHO3D_OBJECT(AttributeWindowView, Geode::IWindowView)

	public:
		/// Constructors.
		explicit AttributeWindowView(Urho3D::Context* context, Geode::CommandHistory::Ptr commandHistory, Geode::EditorScene::Ptr editorScene);

	private:
		/// Event handlers.
		void HandleSelectedObjectChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
		void HandleReactiveFormDataChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

		/// Accessors & Mutators.
		void SetSelectedSerializable(Urho3D::Serializable* serializable);

	private:
		Geode::CommandHistory::Ptr commandHistory_;
		Geode::EditorScene::Ptr editorScene_;
		Urho3D::Serializable* selectedSerializable_;
		Geode::ReactiveForm* attributesForm_;
	};
}
