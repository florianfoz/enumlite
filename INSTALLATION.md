# Installation

`enumlite` is a header-only library. There is no library to build or link.

## CMake — Fetch from GitHub

```cmake
include(FetchContent)

FetchContent_Declare(
    enumlite
    GIT_REPOSITORY https://github.com/florianfoz/enumlite.git
    GIT_TAG v1.0.0
)

FetchContent_MakeAvailable(enumlite)

target_link_libraries(your_target
    PRIVATE
        enumlite::enumlite
)
```

Then:

```cpp
#include <enumlite/enumlite.h>
```

## CMake — Git submodule

Add the repository to your project:

```bash
git submodule add https://github.com/florianfoz/enumlite.git external/enumlite
```

Then in `CMakeLists.txt`:

```cmake
add_subdirectory(
    external/enumlite
)

target_link_libraries(your_target
    PRIVATE
        enumlite::enumlite
)
```

## CMake — Local copy

Copy the `include/enumlite` directory into your project and add the include directory:

```cmake
target_include_directories(your_target
    PRIVATE
        path/to/enumlite/include
)
```

Then:

```cpp
#include <enumlite/enumlite.h>
```

## Without CMake

Copy:

```text
enumlite/include/enumlite/
```

into your project's include directory.

For example:

```text
my_project/
├── include/
│   └── enumlite/
└── src/
    └── main.cpp
```

Compile with:

```bash
g++ -std=c++20 -Iinclude src/main.cpp
```
