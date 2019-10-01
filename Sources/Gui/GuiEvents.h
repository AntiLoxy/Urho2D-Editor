#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_CONTROLCHANGED, ControlChanged)
{
    URHO3D_PARAM(P_ELEMENT, Element);			   // UIElement pointer
	URHO3D_PARAM(P_OLDVALUE, OldValue);			   // Variant
	URHO3D_PARAM(P_VALUE, Value);				   // Variant
	URHO3D_PARAM(P_VALIDATED, Validate);		   // Boolean
}

URHO3D_EVENT(E_FLYMENU_CLOSED, FlyMenuClosed)
{
	URHO3D_PARAM(P_ELEMENT, Element);			   // UIElement pointer
}

URHO3D_EVENT(E_FLYMENU_ITEM_CLICKED, FlyMenuItemClicked)
{
	URHO3D_PARAM(P_ELEMENT, Element);			   // UIElement pointer
	URHO3D_PARAM(P_ITEM, Item);					   // UIElement pointer
}

URHO3D_EVENT(E_TAB_ADDED, TabAdded)
{
	URHO3D_PARAM(P_INDEX, Index);				   // unsigned int
}

URHO3D_EVENT(E_TAB_REMOVED, TabRemoved)
{
	URHO3D_PARAM(P_INDEX, Index);				   // unsigned int
}

URHO3D_EVENT(E_TAB_CLOSE_RELEASED, TabCloseReleased)
{
	URHO3D_PARAM(P_INDEX, Index);				   // unsigned int
}

URHO3D_EVENT(E_TAB_MOVED, TabMoved)
{
	URHO3D_PARAM(P_INDEX_FROM, IndexFrom);		   // unsigned int
	URHO3D_PARAM(P_INDEX_TO, IndexTo);			   // unsigned int
}

URHO3D_EVENT(E_TAB_SELECTION_CHANGED, TabSelectionChanged)
{
	URHO3D_PARAM(P_INDEX, Index);				   // unsigned int
}

URHO3D_EVENT(E_WINDOWVIEW_OPENED, WindowViewOpened)
{
	URHO3D_PARAM(P_ELEMENT, Element);			   // UIElement pointer
}

URHO3D_EVENT(E_WINDOWVIEW_CLOSED, WindowViewClosed)
{
	URHO3D_PARAM(P_ELEMENT, Element);			   // UIElement pointer
}

URHO3D_EVENT(E_WINDOWVIEW_DRAGBEGIN, WindowViewDragBegin)
{
	URHO3D_PARAM(P_ELEMENT, Element);              // UIElement pointer
	URHO3D_PARAM(P_X, X);                          // int
	URHO3D_PARAM(P_Y, Y);                          // int
	URHO3D_PARAM(P_ELEMENTX, ElementX);            // int
	URHO3D_PARAM(P_ELEMENTY, ElementY);            // int
	URHO3D_PARAM(P_BUTTONS, Buttons);              // int
	URHO3D_PARAM(P_NUMBUTTONS, NumButtons);        // int
}

URHO3D_EVENT(E_WINDOWVIEW_DRAGMOVE, WindowViewDragMove)
{
	URHO3D_PARAM(P_ELEMENT, Element);              // UIElement pointer
	URHO3D_PARAM(P_X, X);                          // int
	URHO3D_PARAM(P_Y, Y);                          // int
	URHO3D_PARAM(P_DX, DX);                        // int
	URHO3D_PARAM(P_DY, DY);                        // int
	URHO3D_PARAM(P_ELEMENTX, ElementX);            // int
	URHO3D_PARAM(P_ELEMENTY, ElementY);            // int
	URHO3D_PARAM(P_BUTTONS, Buttons);              // int
	URHO3D_PARAM(P_NUMBUTTONS, NumButtons);        // int
}

URHO3D_EVENT(E_WINDOWVIEW_DRAGEND, WindowViewDragEnd)
{
	URHO3D_PARAM(P_ELEMENT, Element);              // UIElement pointer
	URHO3D_PARAM(P_X, X);                          // int
	URHO3D_PARAM(P_Y, Y);                          // int
	URHO3D_PARAM(P_ELEMENTX, ElementX);            // int
	URHO3D_PARAM(P_ELEMENTY, ElementY);            // int
	URHO3D_PARAM(P_BUTTONS, Buttons);              // int
	URHO3D_PARAM(P_NUMBUTTONS, NumButtons);        // int
}

URHO3D_EVENT(E_ATTRIBUTEFIELD_DATACHANGED, AttributeFieldDataChanged)
{
	URHO3D_PARAM(P_ELEMENT, Element);			   // UIElement pointer
	URHO3D_PARAM(P_ATTRIBUTE_NAME, AttributeName); // UIElement pointer
	URHO3D_PARAM(P_OLDVALUE, OldValue);			   // Variant
	URHO3D_PARAM(P_VALUE, Value);				   // Variant
	URHO3D_PARAM(P_VALIDATED, Validate);		   // Boolean
}

URHO3D_EVENT(E_REACTIVEFORM_DATACHANGED, ReactiveFormDataChanged)
{
	URHO3D_PARAM(P_ELEMENT, Element);			   // UIElement pointer
	URHO3D_PARAM(P_ATTRIBUTE_NAME, AttributeName); // UIElement pointer
	URHO3D_PARAM(P_OLDVALUE, OldValue);			   // Variant
	URHO3D_PARAM(P_VALUE, Value);				   // Variant
	URHO3D_PARAM(P_VALIDATED, Validate);		   // Boolean
}