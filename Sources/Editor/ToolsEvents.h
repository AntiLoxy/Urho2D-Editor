#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_TOOLSELECTED, ToolSelected)
{
	URHO3D_PARAM(P_NAME, Name);			// Urho3D::String
	URHO3D_PARAM(P_OBJECT, Object);     // Urho3D::Object
}