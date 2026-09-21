# Installation

`enum_lite` is a header-only library. There is no library to build or link.

## CMake — Fetch from GitHub

```cmake
include(FetchContent)

FetchContent_Declare(
    enum_lite
    GIT_REPOSITORY https://github.com/YOUR_USERNAME/enum_lite.git
    GIT_TAG v1.0.0
)

FetchContent_MakeAvailable(enum_lite)

target_link_libraries(your_target
    PRIVATE
        enum_lite::enum_lite
)
```

Then:

```cpp
#include <enum_lite/enum_lite.h>
```

## CMake — Git submodule

Add the repository to your project:

```bash
git submodule add https://github.com/YOUR_USERNAME/enum_lite.git external/enum_lite
```

Then in `CMakeLists.txt`:

```cmake
add_subdirectory(
    external/enum_lite
)

target_link_libraries(your_target
    PRIVATE
        enum_lite::enum_lite
)
```

## CMake — Local copy

Copy the `include/enum_lite` directory into your project and add the include directory:

```cmake
target_include_directories(your_target
    PRIVATE
        path/to/enum_lite/include
)
```

Then:

```cpp
#include <enum_lite/enum_lite.h>
```

## Without CMake

Copy:

```text
enum_lite/include/enum_lite/
```

into your project's include directory.

For example:

```text
my_project/
├── include/
│   └── enum_lite/
└── src/
    └── main.cpp
```

Compile with:

```bash
g++ -std=c++20 -Iinclude src/main.cpp
```
