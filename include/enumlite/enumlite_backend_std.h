#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#ifndef ENUMLITE_CONSTEXPR
#define ENUMLITE_CONSTEXPR constexpr
#endif

namespace enumlite
{

struct std_backend {
  using string_type      = std::string;
  using string_view_type = std::string_view;
  using char_type        = char;
  using size_type        = std::size_t;

  static constexpr size_type npos = string_type::npos;

  static constexpr bool empty(const string_type& value) noexcept
  {
    return value.empty();
  }

  static constexpr size_type size(const string_type& value) noexcept
  {
    return value.size();
  }

  static constexpr char_type front(const string_type& value) noexcept
  {
    return value.front();
  }

  static constexpr char_type back(const string_type& value) noexcept
  {
    return value.back();
  }

  static constexpr char_type at(const string_type& value, size_type pos) noexcept
  {
    return value[pos];
  }

  static constexpr string_type substr(const string_type& value, size_type pos, size_type count = npos)
  {
    return value.substr(pos, count);
  }

  static constexpr size_type find(const string_type& value, char_type c, size_type pos = 0) noexcept
  {
    return value.find(c, pos);
  }

  static constexpr void erase(string_type& value, size_type pos, size_type count = npos)
  {
    value.erase(pos, count);
  }

  static constexpr void append(string_type& value, const string_type& suffix)
  {
    value += suffix;
  }

  static constexpr void append(string_type& value, char_type c)
  {
    value += c;
  }

  static constexpr char_type to_lower(char_type c) noexcept
  {
    if (c >= 'A' && c <= 'Z') return static_cast<char_type>(c - 'A' + 'a');

    return c;
  }

  static constexpr string_type from_literal(const char* value)
  {
    return string_type(value);
  }

  static constexpr string_view_type view_from_literal(const char* value) noexcept
  {
    return string_view_type(value);
  }
};

} // namespace enumlite
