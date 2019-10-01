#include "Radio.h"

#include <Urho3D/Math/Vector2.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/UI/UIEvents.h>

using namespace Urho3D;

namespace Geode
{
	Radio::Radio(Context* context) :
		BorderImage(context),
		checkedOffset_(IntVector2::ZERO),
		checked_(false)
	{
		SetEnabled(true);
		focusMode_ = FM_FOCUSABLE_DEFOCUSABLE;
	}

	Radio::~Radio() = default;

	void Radio::RegisterObject(Context* context)
	{
		context->RegisterFactory<Radio>();

		URHO3D_COPY_BASE_ATTRIBUTES(BorderImage);
		URHO3D_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Is Enabled", true);
		URHO3D_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Focus Mode", FM_FOCUSABLE_DEFOCUSABLE);
		URHO3D_ACCESSOR_ATTRIBUTE("Is Checked", IsChecked, SetChecked, bool, false, AM_FILE);
		URHO3D_ACCESSOR_ATTRIBUTE("Checked Image Offset", GetCheckedOffset, SetCheckedOffset, IntVector2, IntVector2::ZERO, AM_FILE);
		URHO3D_ACCESSOR_ATTRIBUTE("Group Name", GetGroupName, SetGroupName, String, String::EMPTY, AM_FILE);
	}

	void Radio::GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor)
	{
		IntVector2 offset(IntVector2::ZERO);
		if (hovering_ || selected_ || HasFocus())
			offset += hoverOffset_;
		if (checked_)
			offset += checkedOffset_;

		BorderImage::GetBatches(batches, vertexData, currentScissor, offset);
	}

	void Radio::OnClickBegin(const IntVector2& position, const IntVector2& screenPosition, int button, int buttons, int qualifiers, Cursor* cursor)
	{
		if (button == MOUSEB_LEFT && editable_ && !checked_)
		{
			SetChecked(true);
		}
	}

	void Radio::OnKey(Key key, MouseButtonFlags buttons, QualifierFlags qualifiers)
	{
		if (HasFocus() && key == KEY_SPACE)
		{
			OnClickBegin(IntVector2(), IntVector2(), MOUSEB_LEFT, 0, 0, nullptr);
		}
	}

	void Radio::SetChecked(bool enable)
	{
		if (enable != checked_)
		{
			if (enable)
			{
				for (auto radio : GetAllRadioFromGroup())
				{
					if (radio != this)
					{
						radio->SetChecked(false);
					}
				}
			}

			checked_ = enable;

			VariantMap sendEventData;
			using namespace Toggled;
			sendEventData[P_ELEMENT] = this;
			sendEventData[P_STATE] = checked_;
			SendEvent(E_TOGGLED, sendEventData);
		}
	}

	void Radio::SetCheckedOffset(const IntVector2& offset)
	{
		checkedOffset_ = offset;
	}

	void Radio::SetCheckedOffset(int x, int y)
	{
		checkedOffset_ = IntVector2(x, y);
	}

	void Radio::SetGroupName(const String& groupName)
	{
		groupName_ = groupName;
	}

	const String& Radio::GetGroupName() const
	{
		return groupName_;
	}

	PODVector<Radio*> Radio::GetAllRadioFromGroup()
	{
		PODVector<UIElement*> elements;
		PODVector<Radio*> radios;

		GetRoot()->GetChildren(elements, true);

		for (auto element : elements)
		{
			if (element->IsInstanceOf<Radio>())
			{
				Radio* radio = static_cast<Radio*>(element);

				if (radio && radio->GetGroupName() == groupName_)
				{
					radios.Push(radio);
				}
			}
		}

		return radios;
	}
}
