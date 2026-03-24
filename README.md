# Axiom Game Engine

## Introduction

Axiom is a game engine project with an accompanying editor application called Theorem. The idea behind this project is to use minimal external libraries for development, such as ImGui, GLM, GLFW and others (currently only using lib for font rendering, MSDFGen); everything will be done internally within this repository or other submodule repositories. This project is for recreational purposes only and is currently in early development stages.

## Build System

The project uses SCons as it's build system. It supports the following configurations:

- **platform**: Target platform (Windows or Linux)
- **config**: Build configuration (e.g., Debug, Release)
- **compiler**: C++ compiler (MSVC, G++, Clang)
- **vsproj**: Visual Studio project generation
- **verbose**: Additional building information

On Windows, it is recommended to use the MSVC compiler (default). Visual Studio project file generation is only available with the MSVC compiler.

> [!NOTE]
> Currently the font renderer only supports fonts that do not contain self-intersecting outline. If you notice rendering issues on fonts downloaded from websites such as Google Fonts, try downloading the font from the font author's official website instead.

> [!NOTE]
> To enable Vulkan Validation Layers in Debug builds, ensure the Vulkan SDK is installed on your system. These layers provide essential error checking and debugging information that is not present in the standard driver.

Examples of build commands:
```bash
scons config=Debug platform=Windows compiler=msvc vsproj=yes
```
```bash
scons config=Release platform=Linux compiler=gcc
```