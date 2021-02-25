![cmake build](https://github.com/BartSiwek/Gris/workflows/cmake%20build/badge.svg?branch=master)
![clang-tidy lint](https://github.com/BartSiwek/Gris/workflows/clang-tidy%20lint/badge.svg?branch=master)
![clang-format lint](https://github.com/BartSiwek/Gris/workflows/clang-format%20lint/badge.svg?branch=master)

# Gris
Small graphics engine to try stuff out

## Dependencies
Dependencies are mostly managed via [Conan](https://conan.io/) however there are few that are OS specific and need to be installed manually:
* [CMake](https://cmake.org/) (version 3.17+)
* [Conan](https://conan.io/)
* [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/) (version 1.2.154.0+)

## Optional dependencies
* [clang-format](https://clang.llvm.org/docs/ClangFormat.html) (version 11.0+) - needed for automatic code formatiing
* [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)

## Demos

### Vulkan Tutorial
A adapted version of the [vulkan-tutorial.com](https://vulkan-tutorial.com/) code.
![Vulkan Tutorial Screenshot](demos/vulkan_tutorial/screenshot.png?raw=true)
