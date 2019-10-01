#include "ITool.h"

using namespace Urho3D;

namespace Geode
{
	ITool::ITool(Context* context) : Object(context)
	{
		enabled_ = true;
	}

	///------------------------------------------------------------------------------------------------
	///  ACCESSORS & MUTATORS
	///------------------------------------------------------------------------------------------------

	void ITool::SetEnabled(bool enabled)
	{
		if (enabled != enabled_)
		{
			enabled_ = enabled;
		}
	}

	bool ITool::IsEnabled() const
	{
		return enabled_;
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	UIElement* ITool::PopulateToolBar(ToolBar* toolBar)
	{
		return nullptr;
	}

	bool ITool::IsActive()
	{
		return false;
	}
}