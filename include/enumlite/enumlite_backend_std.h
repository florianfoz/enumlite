#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace enumlite
{

struct std_backend {
  using string_type      = std::string;
  using string_view_type = std::string_view;
  using char_type        = char;
  using size_type        = std::size_t;

  static constexpr size_type npos = string_type::npos;

  static bool empty(const string_type& value) noexcept
  {
    return value.empty();
  }

  static size_type size(const string_type& value) noexcept
  {
    return value.size();
  }

  static char_type front(const string_type& value) noexcept
  {
    return value.front();
  }

  static char_type back(const string_type& value) noexcept
  {
    return value.back();
  }

  static char_type at(const string_type& value, size_type pos) noexcept
  {
    return value[pos];
  }

  static string_type substr(const string_type& value, size_type pos, size_type count = npos)
  {
    return value.substr(pos, count);
  }

  static size_type find(const string_type& value, char_type c, size_type pos = 0) noexcept
  {
    return value.find(c, pos);
  }

  static void erase(string_type& value, size_type pos, size_type count = npos)
  {
    value.erase(pos, count);
  }

  static void append(string_type& value, const string_type& suffix)
  {
    value += suffix;
  }

  static void append(string_type& value, char_type c)
  {
    value += c;
  }

  static char_type to_lower(char_type c) noexcept
  {
    if (c >= 'A' && c <= 'Z') return static_cast<char_type>(c - 'A' + 'a');

    return c;
  }

  static string_type from_literal(const char* value)
  {
    return string_type(value);
  }

  static string_view_type view_from_literal(const char* value) noexcept
  {
    return string_view_type(value);
  }
};

} // namespace enumlite

