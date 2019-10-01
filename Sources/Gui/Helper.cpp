#include "Helper.h"

#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/Text.h>

using namespace Urho3D;

namespace Geode
{
	BorderImage* Helper::CreateIcon(UIElement* parent, String iconName, HorizontalAlignment ha, VerticalAlignment va, unsigned int index)
	{
		auto icon = parent->CreateChild<BorderImage>("Icon", index);
		icon->SetStyle(iconName);
		icon->SetAlignment(ha, va);

		return icon;
	}

	ToolTip* Helper::CreateToolTip(UIElement* parent, String tooltipText, IntVector2 offset)
	{
		auto toolTip = parent->CreateChild<ToolTip>("ToolTip");
		toolTip->SetStyleAuto();
		toolTip->SetPosition(toolTip->GetPosition() + offset);

		auto toolTipLabel = toolTip->CreateChild<Text>("ToolTipLabel");
		toolTipLabel->SetStyleAuto();
		toolTipLabel->SetText(tooltipText);

		return toolTip;
	}

	void Helper::Sensitive(UIElement* element)
	{
		element->SetEnabled(true);
		element->SetEditable(true);
		element->SetOpacity(1);
	}

	void Helper::Unsensitive(UIElement* element)
	{
		element->SetEnabled(false);
		element->SetEditable(false);
		element->SetOpacity(0.5);
	}
}