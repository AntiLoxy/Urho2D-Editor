#include "SeparatorTool.h"

using namespace Urho3D;

namespace Geode
{
	SeparatorTool::SeparatorTool(Context* context) : ITool(context), separator_(nullptr)
	{
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	UIElement* SeparatorTool::PopulateToolBar(ToolBar* toolBar)
	{
		separator_ = toolBar->AddSeparator();
		return separator_;
	}
}