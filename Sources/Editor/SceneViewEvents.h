#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_SCENEVIEW_CLICK, SceneViewClick)
{
	URHO3D_PARAM(P_X, X);                          // float
	URHO3D_PARAM(P_Y, Y);                          // float
	URHO3D_PARAM(P_BUTTON, Button);                // int
	URHO3D_PARAM(P_BUTTONS, Buttons);              // int
	URHO3D_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

URHO3D_EVENT(E_SCENEVIEW_CLICKEND, SceneViewClickEnd)
{
	URHO3D_PARAM(P_X, X);                          // float
	URHO3D_PARAM(P_Y, Y);                          // float
	URHO3D_PARAM(P_BUTTON, Button);                // int
	URHO3D_PARAM(P_BUTTONS, Buttons);              // int
	URHO3D_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

URHO3D_EVENT(E_SCENEVIEW_DRAGBEGIN, SceneViewDragBegin)
{
	URHO3D_PARAM(P_X, X);                          // float
	URHO3D_PARAM(P_Y, Y);                          // float
	URHO3D_PARAM(P_BUTTON, Button);                // int
	URHO3D_PARAM(P_BUTTONS, Buttons);              // int
	URHO3D_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

URHO3D_EVENT(E_SCENEVIEW_DRAGMOVE, SceneViewDragMove)
{
	URHO3D_PARAM(P_X, X);                          // float
	URHO3D_PARAM(P_Y, Y);                          // float
	URHO3D_PARAM(P_DX, DX);                        // float
	URHO3D_PARAM(P_DY, DY);                        // float
	URHO3D_PARAM(P_BUTTON, Button);                // int
	URHO3D_PARAM(P_BUTTONS, Buttons);              // int
	URHO3D_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

URHO3D_EVENT(E_SCENEVIEW_DRAGEND, SceneViewDragEnd)
{
	URHO3D_PARAM(P_X, X);                          // float
	URHO3D_PARAM(P_Y, Y);                          // float
	URHO3D_PARAM(P_BUTTON, Button);                // int
	URHO3D_PARAM(P_BUTTONS, Buttons);              // int
	URHO3D_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

URHO3D_EVENT(E_SCENEVIEW_DRAGCANCEL, SceneViewDragCancel)
{
	URHO3D_PARAM(P_X, X);                          // float
	URHO3D_PARAM(P_Y, Y);                          // float
	URHO3D_PARAM(P_BUTTON, Button);                // int
	URHO3D_PARAM(P_BUTTONS, Buttons);              // int
	URHO3D_PARAM(P_QUALIFIERS, Qualifiers);        // int
}