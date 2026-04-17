# Axiom Game Engine

## Introduction

Axiom is a game engine project with an editor application called Theorem. The idea behind this project is to use minimal external libraries for development, such as ImGui, GLM, GLFW and others (currently only using external libs for font rendering, MSDFGen and shader compilation, shaderc | spirv-cross); everything will be done internally within this repository or other submodule repositories. This project is for recreational purposes only and is currently in early development stages.

## Build System

The project uses SCons as its build system. The build script has been designed to automatically detect your host operating system and select the most appropriate compiler and rendering backend by default. 

### Available Configurations

You can customize your build by passing the following arguments to the `scons` command:

* **`config`**: Build configuration. Allowed values: `debug`, `release`. *(Default: `debug`)*
* **`platform`**: Target platform. Allowed values: `windows`, `linux`, `macos`. *(Default: Auto-detected based on your host OS)*
* **`compiler`**: C++ compiler. Allowed values: `msvc`, `gcc`, `g++`, `clang`. *(Default: `msvc` for Windows, `clang` for macOS, `gcc` for Linux)*
* **`renderer`**: Graphics API backend. Allowed values: `vulkan`, `metal`. *(Default: `metal` for macOS, `vulkan` for Windows/Linux)*

### Build Flags

* **`vsproj`**: Generates Visual Studio project and solution files (`yes`/`no`). Note: This requires the MSVC compiler. *(Default: `no`)*
* **`verbose`**: Prints the full, raw command lines used during compilation instead of the formatted output (`yes`/`no`). *(Default: `no`)*
* **`no-color`**: Disables colored terminal output (`yes`/`no`). *(Default: `no`)*
* **`-j N`**: Standard SCons flag to run `N` parallel compilation jobs (e.g., `-j 8`). By default, the script will automatically utilize all available CPU cores.
* **`-c`**: Standard SCons flag to clean the build directory.
* **`-h` / `--help`**: Displays the built-in help menu outlining all available commands and current defaults.

> [!NOTE]  
> Currently the font renderer only supports fonts that do not contain self-intersecting outlines. If you notice rendering issues on fonts downloaded from websites such as Google Fonts, try downloading the font from the font author's official website instead.

> [!NOTE]  
> To enable Vulkan Validation Layers in Debug builds, ensure the Vulkan SDK is installed on your system. These layers provide essential error checking and debugging information that is not present in the standard driver.

### Example Build Commands

**Windows:**
Build a standard Debug version and generate Visual Studio project files (relies on defaults for platform, MSVC compiler, and Vulkan):
```bash
scons vsproj=yes
```

**macOS:**
Build a Release version using Clang and the Metal backend:
```bash
scons config=release platform=macos compiler=clang renderer=metal
```

**Linux:**
Build a Debug version using GCC with colored output disabled:
```bash
scons config=debug platform=linux compiler=gcc no-color=yes
```

**Clean Build Directory:**
```bash
scons -c
```