#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_SELECTEDOBJECTCHANGED, SelectedObjectChanged)
{
    URHO3D_PARAM(P_OBJECT, Object);     // Urho3D::Object
}

URHO3D_EVENT(E_SCENELOADED, SceneLoaded)
{}

URHO3D_EVENT(E_SCENESAVED, SceneSaved)
{}