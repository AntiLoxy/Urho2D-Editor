# Urho2D-Editor
Editor specially designed to create two-dimensions scenes with the Urho3D game engine.

## Compile engine
    git clone https://github.com/urho3d/Urho3D.git
    mkdir Urho3D-Build
    cmake -G "Visual Studio 16 2019" -DURHO3D_64BIT=0 -DURHO3D_HASH_DEBUG=TRUE -B"Urho3D-Build" -H"Urho3D"
    cmake --build Urho3D-Build

## Compile editor
    git clone https://github.com/AntiLoxy/Urho2D-Editor.git
    mkdir Urho2D-Editor-Build
    cmake -G "Visual Studio 16 2019" -DURHO3D_64BIT=0 -DURHO3D_HOME="Urho3D-Build" -B"Urho2D-Editor-Build" -H"Urho2D-Editor"
    cmake --build Urho2D-Editor-Build
