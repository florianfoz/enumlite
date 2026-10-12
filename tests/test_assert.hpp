#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

namespace test
{
template <typename T>
void print_value(const T& value)
{
  if constexpr (std::is_enum_v<T>) {
    using underlying_type = std::underlying_type_t<T>;
    std::cerr << static_cast<underlying_type>(value);
  } else if constexpr (requires { std::cerr << value; }) {
    std::cerr << value;
  } else {
    std::cerr << "<value not printable>";
  }
}

template <typename A, typename B>
void assert_eq(const A& actual, const B& expected, std::string_view actual_expr, std::string_view expected_expr,
               std::string_view file, int line)
{
  if (actual == expected) return;

  std::cerr << file << ':' << line << ": assertion failed\n"
            << "  expression: " << actual_expr << " == " << expected_expr << '\n'
            << "  actual:    ";

  print_value(actual);

  std::cerr << "\n  expected:  ";

  print_value(expected);

  std::cerr << '\n';
  std::abort();
}
} // namespace test

#define ASSERT_EQ(actual, expected) ::test::assert_eq((actual), (expected), #actual, #expected, __FILE__, __LINE__)