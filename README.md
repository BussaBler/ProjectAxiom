# Axiom Game Engine

## Introduction

Axiom is a game engine project with an accompanying editor application called Theorem. The idea behind this project is to use no external libraries for development, such as ImGui, GLM, GLFW and others; everything will be done internally within this repository or other submodule repositories. This project is for recreational purposes only and is currently in early development stages.

## Build System

The project uses SCons as its build system. It supports the following configurations:

- **platform**: Target platform (Windows or Linux)
- **config**: Build configuration (e.g., Debug, Release)
- **compiler**: C++ compiler
- **vsproj**: Visual Studio project generation
- **verbose**: Additional building information

On Windows, it is recommended to use the MSVC compiler (default). Visual Studio project file generation is only available with the MSVC compiler.

Examples of build commands:
```bash
scons config=Debug platform=Windows compiler=msvc vsproj=yes
```
```bash
scons config=Release platform=Linux compiler=gcc
```