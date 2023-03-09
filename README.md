<h1 align="center" style="border-bottom: none;">
  <a href="https://github.com/Seeseejava/Flame">Flame Engine</a>
</h1>
<h3 align="center">Windows 2D and 3D Game Engine written in C++ that supports OpenGL.</h3>
<p align="center">
  <a href="#screenshots">Screenshots</a> |
  <a href="#building-🔨">Building</a> |
  <a href="#features">Features</a> |
  <a href="#dependencies">Dependencies</a>
<br/>
<br/>
<img alt="Build" src="https://img.shields.io/endpoint.svg?url=https%3A%2F%2Factions-badge.atrox.dev%2Fjmorton06%2FLumos%2Fbadge&style=flat-square&label=build&branch=master&event=push"/>
<img alt="platforms" src="https://img.shields.io/badge/Platforms-Windows-blue?style=flat-square"/>
<img alt="license" src="https://img.shields.io/github/license/jmorton06/Lumos?style=flat-square"/>
<br/>

</p>

#

## Screenshots
![image](https://github.com/Seeseejava/Flame/blob/main/image/Flame%20Engine.png)

## Prerequisites

To build Flame Engine, you must first install the following tools.

### Windows 10/11
- Visual Studio 2022
- CMake 3.15 (or more recent)
#
## Building 🔨

```
git clone https://github.com/jmorton06/Lumos.git
```


### Windows 
```
Run Scripts/CmakeProjects.bat to generate a visual studio project.
```


#
## Features

* Support for Windows now.
* Support for OpenGL.
* Audio using FMOD.
* Rendering 3D models with PBR shading.
* Debug gui using ImGui
* 3D collision detection - bullet3.
* 2D collision detection - Box2D.

#
## Dependencies
 * [imgui](https://github.com/ocornut/imgui) : Dear ImGui: Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies.
 * [imguizmo](https://github.com/CedricGuillemet/ImGuizmo) : Immediate mode 3D gizmo for scene editing and other controls based on Dear Imgui.
 * [entt](https://github.com/skypjack/entt) : Fast and reliable entity-component system (ECS) 
 * [glfw](https://github.com/glfw/glfw) : A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input.
 * [spdlog](https://github.com/gabime/spdlog) : Fast C++ logging library.
 * [stb](https://github.com/nothings/stb) : Single-file public domain (or MIT licensed) libraries for C/C++.
 * [glad](https://github.com/Dav1dde/glad) : Meta loader for OpenGL API.
 * [Box2D](https://github.com/erincatto/Box2D) : 2D physics engine.
 * [FMOD](https://fmod.com/) : Solution for adaptive audio
* [assimp](https://github.com/assimp/assimp) : A library to import and export various 3d-model-formats including scene-post-processing to generate missing render data.
 * [bullet3](https://github.com/bulletphysics/bullet3) : Real-time collision detection and multi-physics simulation for VR, games, visual effects, robotics, machine learning etc.
 * [magic_enum](https://github.com/Neargye/magic_enum) : Header-only C++17 library provides static reflection for enums, work with any enum type without any macro or boilerplate code.
  * [yaml-cpp](https://github.com/jbeder/yaml-cpp) : A YAML parser and emitter in C++ matching the YAML 1.2 spec.
