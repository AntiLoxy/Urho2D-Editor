#include "Main.h"
#include "Gui/ContextMenu.h"
#include "Gui/Controls.h"
#include "Gui/FlyMenu.h"
#include "Gui/GuiEvents.h"
#include "Gui/IView.h"
#include "Gui/IWindowView.h"
#include "Gui/MenuBar.h"
#include "Gui/Radio.h"
#include "Gui/ReactiveForm.h"
#include "Gui/TabBar.h"
#include "Gui/ToolBar.h"

#include <Urho3D/UI/UI.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/Input/Input.h>

using namespace Urho3D;
using namespace Geode;

static const String WINDOW_TITLE = "Geode";

Main::Main(Context* context) : Application(context)
{}

void Main::Setup()
{
    engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_[EP_FULL_SCREEN] = false;
    engineParameters_[EP_HEADLESS] = false;
    engineParameters_[EP_SOUND] = false;

    if (!engineParameters_.Contains(EP_RESOURCE_PREFIX_PATHS))
    {
        engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";Resources";
    }
}

void Main::Start()
{
	auto uiRoot = GetSubsystem<UI>()->GetRoot();
	uiRoot->SetDefaultStyle(GetSubsystem<ResourceCache>()->GetResource<XMLFile>("UI/DefaultStyle.xml"));

	InitWindowTitleAndIcon();
	InitCursor();

	RegisterObjects();

	editorView_ = MakeShared<EditorView>(context_);
}

void Main::Stop()
{
    engine_->DumpResources(true);
}

void Main::RegisterObjects()
{
    FloatControl::RegisterObject(context_);
    QuaternionControl::RegisterObject(context_);
    Vector2Control::RegisterObject(context_);
    Vector3Control::RegisterObject(context_);
    Vector4Control::RegisterObject(context_);
    RectControl::RegisterObject(context_);
    ColorControl::RegisterObject(context_);
    IntegerControl::RegisterObject(context_);
    IntVector2Control::RegisterObject(context_);
    IntVector3Control::RegisterObject(context_);
    IntRectControl::RegisterObject(context_);
    CheckboxControl::RegisterObject(context_);
    RadioControl::RegisterObject(context_);
    StringControl::RegisterObject(context_);
    StringListControl::RegisterObject(context_);
    FileControl::RegisterObject(context_);
    ResourceRefControl::RegisterObject(context_);
    SelectControl::RegisterObject(context_);
    VariantControl::RegisterObject(context_);
    VariantMapControl::RegisterObject(context_);

    FlyMenu::RegisterObject(context_);
    MenuBar::RegisterObject(context_);
    Radio::RegisterObject(context_);
    AttributeField::RegisterObject(context_);
    ReactiveForm::RegisterObject(context_);
    TabBar::RegisterObject(context_);
    ToolBar::RegisterObject(context_);
}

void Main::InitWindowTitleAndIcon()
{
    auto cache = GetSubsystem<ResourceCache>();
    auto graphics = GetSubsystem<Graphics>();

    graphics->SetWindowIcon(cache->GetResource<Image>("Textures/UrhoIcon.png"));
    graphics->SetWindowTitle(WINDOW_TITLE);
}

void Main::InitCursor()
{
	auto ui = GetSubsystem<UI>();
	auto cache = GetSubsystem<ResourceCache>();
	
	cursor_ = MakeShared<Cursor>(context_);
	cursor_->SetStyleAuto(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

	ui->SetCursor(cursor_);
	ui->GetCursor()->SetVisible(true);
}

URHO3D_DEFINE_APPLICATION_MAIN(Main)
