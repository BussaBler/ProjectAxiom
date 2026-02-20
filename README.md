# Axiom Game Engine

## Introduction

Axiom is a game engine project with an accompanying editor application called Theorem. The project is currently in early development stages.

## Build System

The project uses SCons as its build system. It supports the following configurations:

- **platform**: Target platform
- **config**: Build configuration (e.g., Debug, Release)
- **compiler**: C++ compiler
- **vsproj**: Visual Studio project generation
- **verbose**: Additional building information

On Windows, it is recommended to use the MSVC compiler (default). Visual Studio project file generation is only available with the MSVC compiler.

Example build command:
```bash
scons config=Debug platform=Windows compiler=msvc vsproj=yes
```