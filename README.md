
# Urho2D-Editor
Editor specially designed to create two-dimensions scenes with the Urho3D game engine.
It's quite handy if you want to edit your nodes and collision shapes in two-dimensions world.
The final goal is to propose an alternative of TMX file parsing and work directly with Urho3D scene format.

## Getting Started
These instructions will get you a copy of the project up and running on your local machine

### Install Urho3D
    git clone https://github.com/urho3d/Urho3D.git
    mkdir Urho3D-Build
    cmake -G "Visual Studio 16 2019" -DURHO3D_64BIT=0 -DURHO3D_HASH_DEBUG=TRUE -B"Urho3D-Build" -H"Urho3D"
    cmake --build Urho3D-Build

### Install Urho2D-Editor
    git clone https://github.com/AntiLoxy/Urho2D-Editor.git
    mkdir Urho2D-Editor-Build
    cmake -G "Visual Studio 16 2019" -DURHO3D_64BIT=0 -DURHO3D_HOME="Urho3D-Build" -B"Urho2D-Editor-Build" -H"Urho2D-Editor"
    cmake --build Urho2D-Editor-Build

## Implemented features
 - Scene opening & saving
 - Attributes editor
 - Scene hierarchy list
 - Create new node
 - Create 2D components
 - Delete scene object
 - Undo all actions
 - Quick tools undo
 - Anchor box resize helper
 - Move tool
 - Scale tool
 - Rotate tool
 - CollisionPolygon2D tool (Insert, Move, Delete)
 - CollisionBox2D tool
 - CollisionCircle2D tool
 
 ## Futur features
 
 - Move node in scene hierarchy list (by drag'n'drop)
 - Create CollisionChain2D tool
 - Create new node from prefab file

and mores !

## Issues know at this time

 - Memory leak issue wait to be fixed !
