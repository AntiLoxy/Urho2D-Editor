/**
 * @file    Radio.h
 * @ingroup Gui
 * @brief   Provide radio.
 *
 * Copyright (c) 2018 AntiLoxy (rootofgeno@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/BorderImage.h>

namespace Geode
{
	class Radio : public Urho3D::BorderImage
	{
		URHO3D_OBJECT(Radio, Urho3D::BorderImage);

	public:
		/// Construct.
		explicit Radio(Urho3D::Context* context);
		/// Destruct.
		~Radio() override;
		/// Register object factory.
		static void RegisterObject(Urho3D::Context* context);

		/// Return UI rendering batches.
		void GetBatches(Urho3D::PODVector<Urho3D::UIBatch>& batches, Urho3D::PODVector<float>& vertexData, const Urho3D::IntRect& currentScissor) override;
		/// React to mouse click begin.
		void OnClickBegin(const Urho3D::IntVector2& position, const Urho3D::IntVector2& screenPosition, int button, int buttons, int qualifiers, Urho3D::Cursor* cursor) override;
		/// React to a key press.
		void OnKey(Urho3D::Key key, Urho3D::MouseButtonFlags buttons, Urho3D::QualifierFlags qualifiers) override;

		/// Set checked state.
		void SetChecked(bool enable);
		/// Set checked image offset.
		void SetCheckedOffset(const Urho3D::IntVector2& offset);
		/// Set checked image offset.
		void SetCheckedOffset(int x, int y);
		/// Set group name.
		void SetGroupName(const Urho3D::String& groupName);

		/// Return whether is checked.
		bool IsChecked() const { return checked_; }

		/// Return checked image offset.
		const Urho3D::IntVector2& GetCheckedOffset() const { return checkedOffset_; }
		/// Get group name.
		const Urho3D::String& GetGroupName() const;

	private:
		/// Get all radio from group (match with group name).
		Urho3D::PODVector<Radio*> GetAllRadioFromGroup();

	protected:
		/// Checked image offset.
		Urho3D::IntVector2 checkedOffset_;
		/// Current checked state.
		bool checked_;
		/// Group name.
		Urho3D::String groupName_;
	};
}