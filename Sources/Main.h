#pragma once

#include "Editor/EditorView.h"

#include <Urho3D/Engine/Application.h>

class Main : public Urho3D::Application
{
    URHO3D_OBJECT(Main, Urho3D::Application)

public:
    explicit Main(Urho3D::Context* context);

    void Setup() override;
    void Start() override;
    void Stop() override;

private:
    void RegisterObjects();

    void InitWindowTitleAndIcon();
	void InitCursor();

private:
	Urho3D::SharedPtr<Urho3D::Cursor> cursor_;
	Urho3D::SharedPtr<Geode::EditorView> editorView_;
};
