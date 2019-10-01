#include "ToolsView.h"

using namespace Urho3D;

namespace Geode
{
	ToolsView::ToolsView(Context* context, UIElement* parent) : IView(context, "UI/ToolsView.xml", parent)
	{
		toolBar_ = elRoot_->GetChildDynamicCast<ToolBar>("ToolBar", true);
	}

	///------------------------------------------------------------------------------------------------
	///  OTHER METHODS
	///------------------------------------------------------------------------------------------------

	void ToolsView::AddTool(ITool::Ptr tool)
	{
		tool->PopulateToolBar(toolBar_);
		tools_.Push(tool);

		SubscribeToEvent(tool, E_TOOLSELECTED, URHO3D_HANDLER(ToolsView, HandleSelectedTool));
	}

	void ToolsView::AddSeparator()
	{
		auto separator = MakeShared<SeparatorTool>(context_);
		separator->PopulateToolBar(toolBar_);
		tools_.Push(separator);
	}

	///------------------------------------------------------------------------------------------------
	///  EVENT METHODS
	///------------------------------------------------------------------------------------------------

	void ToolsView::HandleSelectedTool(StringHash, VariantMap& eventData)
	{
		SendEvent(E_TOOLSELECTED, eventData);
	}
}
