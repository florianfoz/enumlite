#pragma once

#include <QChar>
#include <QString>
#include <QStringView>

namespace enumlite
{

struct qt_backend {
  using string_type      = QString;
  using string_view_type = QStringView;
  using char_type        = QChar;
  using size_type        = qsizetype;

  static constexpr size_type npos = -1;

  static bool empty(const string_type& value) noexcept
  {
    return value.isEmpty();
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
    return value.at(pos);
  }

  static string_type substr(const string_type& value, size_type pos, size_type count = npos)
  {
    return value.mid(pos, count);
  }

  static size_type find(const string_type& value, char_type c, size_type pos = 0) noexcept
  {
    return value.indexOf(c, pos);
  }

  static void erase(string_type& value, size_type pos, size_type count = npos) noexcept
  {
    value.remove(pos, count);
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
    return c.toLower();
  }

  static string_type from_literal(const char* value)
  {
    return QString::fromUtf8(value);
  }

  static string_view_type view_from_literal(const char* value)
  {
    return QStringView(QString::fromUtf8(value));
  }
};

} // namespace enumlite

