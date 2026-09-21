# enumlite

A lightweight, header-only C++20 library for defining strongly-typed enums and bitmask flags with configurable string backends.

`enumlite` reduces the boilerplate required to define enums and flags while providing:

* Strongly-typed `enum class` types
* Automatic enum metadata
* String conversion
* String parsing
* Value validation
* Bitmask flag operators
* Flag string serialization and parsing
* Name/value collections
* String normalization
* Configurable string backends
* Standard C++ backend included
* Optional Qt backend
* Custom backend support
* No runtime registration
* Header-only design

The core library is completely independent from Qt or any other string framework.



## Features

### Enums

Define an enum with a single macro:

```
DEFINE_ENUM(
    Color,
    int,
    RED,   1,
    GREEN, 2,
    BLUE,  4
);
```

This generates:

```
enum class Color : int
{
    NONE = 0,
    RED = 1,
    GREEN = 2,
    BLUE = 4
};
```

as well as helper functions and metadata:

```
Color_names
Color_values
Color_all

Color_to_str(...)
Color_from_str(...)
Color_is_valid(...)
```

### Flags

Define a bitmask enum just as easily:

```
DEFINE_FLAGS(
    Permissions,
    unsigned,
    READ,  1u,
    WRITE, 2u,
    EXEC,  4u
);
```

The following operators are generated automatically:

```
|
&
^
~
|=
&=
^=
```

Example:

```
auto permissions =
    Permissions::READ |
    Permissions::WRITE;

if (Permissions_has_flag(
        permissions,
        Permissions::READ))
{
    // READ is enabled
}
```



## Requirements

* C++20 or newer
* A C++20-compatible compiler:

  * GCC
  * Clang
  * MSVC
* No external dependency when using the standard backend



# Installation

`enumlite` is header-only, so there is nothing to build or link.

## Option 1 — Copy the headers

Copy the `enumlite` directory into your project:

```
your_project/
├── include/
│   └── enumlite/
│       ├── enumlite.h
│       ├── enumlite_backend_std.h
│       └── enumlite_backend_qt.h
└── src/
```

Then include:

```
#include <enumlite/enumlite.h>
```

Make sure your compiler's include path contains the `include` directory.



## Option 2 — Git submodule

Add the repository as a Git submodule:

```
git submodule add https://github.com/<your-user>/enumlite.git external/enumlite
```

Then add:

```
external/enumlite
```

to your compiler include paths.

For example with CMake:

```
target_include_directories(
    MyProject
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/enumlite
)
```

Then:

```
#include <enumlite/enumlite.h>
```



## Option 3 — CMake

Because `enumlite` is header-only, it can be exposed as an `INTERFACE` target:

```
add_library(enumlite INTERFACE)

target_include_directories(
    enumlite
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(
    enumlite
    INTERFACE
        cxx_std_20
)
```

Then:

```
target_link_libraries(
    MyProject
    PRIVATE
        enumlite
)
```

No library needs to be linked at runtime.



# Basic Usage

Include the library:

```
#include <enumlite/enumlite.h>
```

Then define an enum:

```
DEFINE_ENUM(
    Color,
    int,
    RED,   1,
    GREEN, 2,
    BLUE,  4
);
```

## Enum to string

```
auto text = Color_to_str(Color::RED);
```

With the standard backend, the return type is:

```
std::string
```

For example:

```
std::cout << Color_to_str(Color::RED);
```

outputs:

```
RED
```



## String to enum

```
auto color = Color_from_str("GREEN");
```

The result is:

```
Color::GREEN
```

Unknown values return:

```
Color::NONE
```

Example:

```
auto color = Color_from_str("unknown");

if (color == Color::NONE)
{
    // Invalid or unknown value
}
```



# String Normalization

`enumlite` performs normalization when comparing enum names.

## Case insensitive

These values are equivalent:

```
RED
red
Red
rEd
```

All match:

```
Color::RED
```



## `_` and `-` are equivalent

For:

```
DEFINE_ENUM(
    MyEnum,
    int,
    MY_VALUE, 1
);
```

all of these match:

```
MY_VALUE
MY-VALUE
my_value
my-value
My_Value
```



## Leading and trailing `_`

A leading underscore is ignored:

```
_VALUE
VALUE
```

A trailing underscore is removed:

```
VALUE_
VALUE
```

This is useful for enum identifiers that conflict with C++ keywords or project naming conventions.

For example:

```
DEFINE_ENUM(
    Keyword,
    int,
    class_, 1,
    struct_, 2
);
```

The string representation is:

```
class
struct
```



# Enum Metadata

`DEFINE_ENUM` also generates collections describing the enum.

For:

```
DEFINE_ENUM(
    Color,
    int,
    RED,   1,
    GREEN, 2,
    BLUE,  4
);
```

the following are available:

```
Color_names
Color_all
Color_values
```



## `Color_names`

Contains the names of all enum values, including `NONE`:

```
for (const auto& name : Color_names)
{
    std::cout << name << '\n';
}
```

Output:

```
NONE
RED
GREEN
BLUE
```



## `Color_all`

Contains the corresponding enum values:

```
for (auto value : Color_all)
{
    std::cout << Color_to_str(value) << '\n';
}
```



## `Color_values`

Contains the comma-separated list of values:

```
std::cout << Color_values;
```

Output:

```
NONE, RED, GREEN, BLUE
```



# Validation

Use the generated validation function:

```
if (Color_is_valid(color))
{
    // Valid Color value
}
```

Example:

```
Color color =
    static_cast<Color>(123);

if (!Color_is_valid(color))
{
    // Invalid value
}
```



# Flags

Flags use the same basic API.

Define them with:

```
DEFINE_FLAGS(
    Permissions,
    unsigned,
    READ,  1u,
    WRITE, 2u,
    EXEC,  4u
);
```

This generates:

```
enum class Permissions : unsigned
{
    NONE = 0,
    READ  = 1,
    WRITE = 2,
    EXEC  = 4,
    ALL   = 7
};
```



## Combining Flags

```
auto permissions =
    Permissions::READ |
    Permissions::WRITE;
```

The result contains both flags.



## Checking a Flag

Use:

```
Permissions_has_flag(
    permissions,
    Permissions::READ
);
```

Example:

```
if (Permissions_has_flag(
        permissions,
        Permissions::WRITE))
{
    std::cout << "WRITE is enabled\n";
}
```



## Removing Flags

The generated operators also support:

```
permissions &= ~Permissions::WRITE;
```



## XOR

```
permissions ^= Permissions::EXEC;
```



## Flags to String

```
auto text =
    Permissions_to_str(permissions);
```

For:

```
Permissions::READ |
Permissions::WRITE
```

the result is:

```
READ|WRITE
```



## String to Flags

Flags can be reconstructed from their string representation:

```
auto permissions =
    Permissions_from_str("READ|WRITE");
```

The same normalization rules are applied:

```
auto permissions =
    Permissions_from_str("read|write");
```

and:

```
auto permissions =
    Permissions_from_str("READ|write");
```

are equivalent.



## Flags to String List

Use:

```
auto names =
    Permissions_to_vec_str(permissions);
```

Then:

```
for (const auto& name : names)
{
    std::cout << name << '\n';
}
```

For:

```
Permissions::READ |
Permissions::WRITE
```

the result is:

```
READ
WRITE
```



# Custom String Backends

One of the main design goals of `enumlite` is to keep enum and flag generation independent from the underlying string implementation.

The default backend uses:

```
std::string
```

but another backend can be selected.

A backend defines the types and string operations used by `enumlite`.



## Default Backend

By default:

```
enumlite::std_backend
```

is used.

It provides:

```
using string_type      = std::string;
using string_view_type = std::string_view;
using char_type        = char;
using size_type        = std::size_t;
```

Therefore:

```
DEFINE_ENUM(
    Color,
    int,
    RED,   1,
    GREEN, 2,
    BLUE,  4
);
```

uses `std::string`.



# Per-Enum Backend

A specific backend can be selected with:

```
DEFINE_ENUM_EX(...)
```

For example:

```
DEFINE_ENUM_EX(
    Color,
    enumlite::std_backend,
    int,
    RED,   1,
    GREEN, 2,
    BLUE,  4
);
```

Flags use:

```
DEFINE_FLAGS_EX(...)
```

Example:

```
DEFINE_FLAGS_EX(
    Permissions,
    enumlite::std_backend,
    unsigned,
    READ,  1u,
    WRITE, 2u,
    EXEC,  4u
);
```

This allows different enums in the same project to use different string backends.



# Global Default Backend

The default backend can be changed for an entire project.

Define:

```
#define enumlite_DEFAULT_BACKEND my_backend
```

before including `enumlite.h`:

```
#define enumlite_DEFAULT_BACKEND my_backend

#include <enumlite/enumlite.h>
```

Now:

```
DEFINE_ENUM(
    Color,
    int,
    RED,   1,
    GREEN, 2,
    BLUE,  4
);
```

automatically uses:

```
my_backend
```

An explicit backend can still be selected:

```
DEFINE_ENUM_EX(
    OtherColor,
    enumlite::std_backend,
    int,
    RED, 1,
    BLUE, 2
);
```



# Qt Backend

Qt support is optional and isolated from the core library.

Include:

```
#include <enumlite/enumlite_backend_qt.h>
```

Then select the Qt backend:

```
DEFINE_ENUM_EX(
    Color,
    enumlite::qt_backend,
    int,
    RED,   1,
    GREEN, 2,
    BLUE,  4
);
```

The generated string functions then use:

```
QString
```

instead of:

```
std::string
```

The core `enumlite.h` itself does not depend on Qt.



# Writing a Custom Backend

A custom backend needs to provide the string types and operations used by the library.

A minimal backend looks like:

```
struct my_backend
{
    using string_type      = MyString;
    using string_view_type = MyStringView;
    using char_type        = MyChar;
    using size_type        = std::size_t;

    static constexpr size_type npos = ...;

    static bool empty(
        const string_type& value);

    static size_type size(
        const string_type& value);

    static char_type front(
        const string_type& value);

    static char_type back(
        const string_type& value);

    static char_type at(
        const string_type& value,
        size_type position);

    static string_type substr(
        const string_type& value,
        size_type position,
        size_type count);

    static size_type find(
        const string_type& value,
        char_type character,
        size_type position);

    static void erase(
        string_type& value,
        size_type position,
        size_type count);

    static void append(
        string_type& value,
        const string_type& suffix);

    static void append(
        string_type& value,
        char_type character);

    static char_type to_lower(
        char_type character);

    static string_type from_literal(
        const char* value);
};
```

Then:

```
DEFINE_ENUM_EX(
    MyEnum,
    my_backend,
    int,
    FOO, 1,
    BAR, 2,
    BAZ, 3
);
```

The enum generation code does not need to know anything about `MyString`.



# Complete Example

```
#include <iostream>

#include <enumlite/enumlite.h>

DEFINE_ENUM(
    LogLevel,
    int,
    DEBUG, 1,
    INFO,  2,
    WARN,  3,
    ERROR, 4
);

DEFINE_FLAGS(
    Permissions,
    unsigned,
    READ,  1u,
    WRITE, 2u,
    EXEC,  4u
);

int main()
{
    // Enum -> string
    std::cout
        << LogLevel_to_str(LogLevel::ERROR)
        << '\n';

    // String -> enum
    const auto level =
        LogLevel_from_str("warn");

    if (level == LogLevel::WARN)
    {
        std::cout << "Warning\n";
    }

    // Combine flags
    const auto permissions =
        Permissions::READ |
        Permissions::WRITE;

    // Flags -> string
    std::cout
        << Permissions_to_str(permissions)
        << '\n';

    // Check a flag
    if (Permissions_has_flag(
            permissions,
            Permissions::WRITE))
    {
        std::cout << "Write enabled\n";
    }

    // String -> flags
    const auto parsed =
        Permissions_from_str("read|exec");

    std::cout
        << Permissions_to_str(parsed)
        << '\n';
}
```

Possible output:

```
ERROR
Warning
READ|WRITE
Write enabled
READ|EXEC
```



# API Summary

## Enum

```
DEFINE_ENUM(
    Type,
    Underlying,
    NAME, VALUE,
    ...
);
```

Generates:

```
Type
Type_names
Type_all
Type_values

Type_to_str()
Type_from_str()
Type_is_valid()
```



## Enum with Explicit Backend

```
DEFINE_ENUM_EX(
    Type,
    Backend,
    Underlying,
    NAME, VALUE,
    ...
);
```



## Flags

```
DEFINE_FLAGS(
    Type,
    Underlying,
    NAME, VALUE,
    ...
);
```

Generates:

```
Type
Type_names
Type_to_str()
Type_to_vec_str()
Type_from_str()
Type_is_valid()
Type_has_flag()
```

and:

```
operator|
operator&
operator^
operator~
operator|=
operator&=
operator^=
```



## Flags with Explicit Backend

```
DEFINE_FLAGS_EX(
    Type,
    Backend,
    Underlying,
    NAME, VALUE,
    ...
);
```



# Design

`enumlite` is split into two layers:

```
enumlite.h
     |
     +-+
     |                   |
  Enum logic         Flags logic
     |                   |
     +++
               |
            Backend
               |
     +++
     |         |         |
    std        Qt      Custom
```

The enum and flag implementation only relies on the backend interface.

This means the core library does not need to know whether strings are implemented using:

* `std::string`
* `QString`
* a game-engine string class
* an embedded-system string type
* a project-specific string class
* another string library



# Header-Only

`enumlite` is header-only.

There is no runtime registration and no library initialization step.

Simply include the headers and define your enums.

This makes the library suitable for:

* Libraries
* Applications
* Embedded projects
* Game engines
* Tools
* Qt applications
* Cross-platform C++ projects



# License

Copyright © 2026 Florian Foz - enumlite

This project is licensed under the Apache License, Version 2.0.
You may obtain a copy of the License at:

https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

See `LICENSE` for the complete license text.
