#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_ANCHORBOX_CENTERCHANGED, AnchorBoxCenterChanged)
{
	URHO3D_PARAM(P_X, X);                          // float
	URHO3D_PARAM(P_Y, Y);                          // float
	URHO3D_PARAM(P_BEGIN_X, BeginX);               // float
	URHO3D_PARAM(P_BEGIN_Y, BeginY);               // float
	URHO3D_PARAM(P_DX, DX);                        // float
	URHO3D_PARAM(P_DY, DY);                        // float
}

URHO3D_EVENT(E_ANCHORBOX_SIZECHANGED, AnchorBoxSizeChanged)
{
	URHO3D_PARAM(P_WIDTH, Width);                  // float
	URHO3D_PARAM(P_HEIGHT, Height);                // float
	URHO3D_PARAM(P_BEGIN_WIDTH, BeginWidth);       // float
	URHO3D_PARAM(P_BEGIN_HEIGHT, BeginHeight);     // float
}

URHO3D_EVENT(E_ANCHORBOX_CONFIRM_CENTERCHANGED, AnchorBoxConfirmCenterChanged)
{
	URHO3D_PARAM(P_X, X);                          // float
	URHO3D_PARAM(P_Y, Y);                          // float
	URHO3D_PARAM(P_BEGIN_X, BeginX);               // float
	URHO3D_PARAM(P_BEGIN_Y, BeginY);               // float
	URHO3D_PARAM(P_DX, DX);                        // float
	URHO3D_PARAM(P_DY, DY);                        // float
}

URHO3D_EVENT(E_ANCHORBOX_CONFIRM_SIZECHANGED, AnchorBoxConfirmSizeChanged)
{
	URHO3D_PARAM(P_WIDTH, Width);                  // float
	URHO3D_PARAM(P_HEIGHT, Height);                // float
	URHO3D_PARAM(P_BEGIN_WIDTH, BeginWidth);       // float
	URHO3D_PARAM(P_BEGIN_HEIGHT, BeginHeight);     // float
}

URHO3D_EVENT(E_ANCHORBOX_CONFIRM_CHANGED, AnchorBoxConfirmChanged)
{
	URHO3D_PARAM(P_X, X);                          // float
	URHO3D_PARAM(P_Y, Y);                          // float
	URHO3D_PARAM(P_BEGIN_X, BeginX);               // float
	URHO3D_PARAM(P_BEGIN_Y, BeginY);               // float
	URHO3D_PARAM(P_WIDTH, Width);                  // float
	URHO3D_PARAM(P_HEIGHT, Height);                // float
	URHO3D_PARAM(P_BEGIN_WIDTH, BeginWidth);       // float
	URHO3D_PARAM(P_BEGIN_HEIGHT, BeginHeight);     // float
}

URHO3D_EVENT(E_ANCHORBOX_CANCELED, AnchorBoxCanceled)
{
	URHO3D_PARAM(P_X, X);                          // float
	URHO3D_PARAM(P_Y, Y);                          // float
	URHO3D_PARAM(P_WIDTH, Width);                  // float
	URHO3D_PARAM(P_HEIGHT, Height);                // float
}