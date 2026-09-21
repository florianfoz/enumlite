#pragma once

#include "enumlite_backend_std.h"

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

#ifndef ENUMLITE_DEFAULT_BACKEND
#define ENUMLITE_DEFAULT_BACKEND ::enumlite::std_backend
#endif

namespace enumlite
{

// ============================================================================
// String helpers
// ============================================================================

template <typename Backend>
typename Backend::string_type normalize(const typename Backend::string_type& input)
{
  using size_type = typename Backend::size_type;

  typename Backend::string_type result;

  const size_type size = Backend::size(input);

  size_type first = 0;
  size_type last  = size;

  // Trim leading spaces/tabs.
  while (first < last) {
    const auto c = Backend::at(input, first);

    if (c != ' ' && c != '\t') break;

    ++first;
  }

  // Trim trailing spaces/tabs.
  while (last > first) {
    const auto c = Backend::at(input, last - 1);

    if (c != ' ' && c != '\t') break;

    --last;
  }

  // Ignore leading underscore.
  if (first < last && Backend::at(input, first) == '_') ++first;

  // Ignore trailing underscore.
  if (last > first && Backend::at(input, last - 1) == '_') --last;

  for (size_type i = first; i < last; ++i) {
    auto c = Backend::at(input, i);

    // '-' and '_' are equivalent.
    if (c == '-') c = '_';

    c = Backend::to_lower(c);

    Backend::append(result, c);
  }

  return result;
}

template <typename Backend>
bool string_equal(const typename Backend::string_type& lhs, const typename Backend::string_type& rhs)
{
  if (Backend::size(lhs) != Backend::size(rhs)) return false;

  const auto size = Backend::size(lhs);

  for (typename Backend::size_type i = 0; i < size; ++i) {
    if (Backend::at(lhs, i) != Backend::at(rhs, i)) return false;
  }

  return true;
}

template <typename Backend>
bool enum_string_equal(const typename Backend::string_type& lhs, const typename Backend::string_type& rhs)
{
  const auto lhs_normalized = normalize<Backend>(lhs);
  const auto rhs_normalized = normalize<Backend>(rhs);

  return string_equal<Backend>(lhs_normalized, rhs_normalized);
}

template <typename Backend>
bool enum_string_equal_literal(const typename Backend::string_type& value, const char* literal)
{
  return enum_string_equal<Backend>(value, Backend::from_literal(literal));
}

// ============================================================================
// Static string list
// ============================================================================

template <typename Backend, std::size_t Capacity>
class static_string_list
{
public:
  using string_type = typename Backend::string_type;
  using size_type   = typename Backend::size_type;

  void push_back(string_type value)
  {
    if (size_ < Capacity) values_[size_++] = std::move(value);
  }

  [[nodiscard]]
  size_type size() const noexcept
  {
    return size_;
  }

  [[nodiscard]]
  bool empty() const noexcept
  {
    return size_ == 0;
  }

  const string_type& operator[](size_type index) const noexcept
  {
    return values_[index];
  }

  string_type& operator[](size_type index) noexcept
  {
    return values_[index];
  }

  auto begin() const noexcept
  {
    return values_.begin();
  }

  auto end() const noexcept
  {
    return values_.begin() + size_;
  }

  auto begin() noexcept
  {
    return values_.begin();
  }

  auto end() noexcept
  {
    return values_.begin() + size_;
  }

private:
  std::array<string_type, Capacity> values_{};
  size_type                         size_ = 0;
};

} // namespace enumlite

// ============================================================================
// Preprocessor utilities
// ============================================================================

#define ENUMLITE_PP_CAT_IMPL(a, b) a##b
#define ENUMLITE_PP_CAT(a, b)      ENUMLITE_PP_CAT_IMPL(a, b)

// ============================================================================
// Count variadic arguments
//
// Supported argument counts: 2, 4, 6, ..., 16
// ============================================================================

#define ENUMLITE_PP_NARG_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N

#define ENUMLITE_PP_NARG(...)                                                                                          \
  ENUMLITE_PP_NARG_IMPL(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// ============================================================================
// Apply macro to pairs
// ============================================================================

#define ENUMLITE_PP_FOR_EACH_PAIR_2(M, a, b) M(a, b)

#define ENUMLITE_PP_FOR_EACH_PAIR_4(M, a, b, ...)                                                                      \
  M(a, b)                                                                                                              \
  ENUMLITE_PP_FOR_EACH_PAIR_2(M, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_6(M, a, b, ...)                                                                      \
  M(a, b)                                                                                                              \
  ENUMLITE_PP_FOR_EACH_PAIR_4(M, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_8(M, a, b, ...)                                                                      \
  M(a, b)                                                                                                              \
  ENUMLITE_PP_FOR_EACH_PAIR_6(M, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_10(M, a, b, ...)                                                                     \
  M(a, b)                                                                                                              \
  ENUMLITE_PP_FOR_EACH_PAIR_8(M, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_12(M, a, b, ...)                                                                     \
  M(a, b)                                                                                                              \
  ENUMLITE_PP_FOR_EACH_PAIR_10(M, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_14(M, a, b, ...)                                                                     \
  M(a, b)                                                                                                              \
  ENUMLITE_PP_FOR_EACH_PAIR_12(M, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_16(M, a, b, ...)                                                                     \
  M(a, b)                                                                                                              \
  ENUMLITE_PP_FOR_EACH_PAIR_14(M, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR(M, ...)                                                                              \
  ENUMLITE_PP_CAT(ENUMLITE_PP_FOR_EACH_PAIR_, ENUMLITE_PP_NARG(__VA_ARGS__))(M, __VA_ARGS__)

// ============================================================================
// Apply macro to pairs + one context
// ============================================================================

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_2(M, C, a, b) M(C, a, b)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_4(M, C, a, b, ...)                                                               \
  M(C, a, b)                                                                                                           \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX_2(M, C, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_6(M, C, a, b, ...)                                                               \
  M(C, a, b)                                                                                                           \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX_4(M, C, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_8(M, C, a, b, ...)                                                               \
  M(C, a, b)                                                                                                           \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX_6(M, C, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_10(M, C, a, b, ...)                                                              \
  M(C, a, b)                                                                                                           \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX_8(M, C, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_12(M, C, a, b, ...)                                                              \
  M(C, a, b)                                                                                                           \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX_10(M, C, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_14(M, C, a, b, ...)                                                              \
  M(C, a, b)                                                                                                           \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX_12(M, C, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_16(M, C, a, b, ...)                                                              \
  M(C, a, b)                                                                                                           \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX_14(M, C, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX(M, C, ...)                                                                       \
  ENUMLITE_PP_CAT(ENUMLITE_PP_FOR_EACH_PAIR_CTX_, ENUMLITE_PP_NARG(__VA_ARGS__))(M, C, __VA_ARGS__)

// ============================================================================
// Apply macro to pairs + two contexts
// ============================================================================

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_2(M, C1, C2, a, b) M(C1, C2, a, b)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_4(M, C1, C2, a, b, ...)                                                         \
  M(C1, C2, a, b)                                                                                                      \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX2_2(M, C1, C2, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_6(M, C1, C2, a, b, ...)                                                         \
  M(C1, C2, a, b)                                                                                                      \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX2_4(M, C1, C2, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_8(M, C1, C2, a, b, ...)                                                         \
  M(C1, C2, a, b)                                                                                                      \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX2_6(M, C1, C2, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_10(M, C1, C2, a, b, ...)                                                        \
  M(C1, C2, a, b)                                                                                                      \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX2_8(M, C1, C2, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_12(M, C1, C2, a, b, ...)                                                        \
  M(C1, C2, a, b)                                                                                                      \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX2_10(M, C1, C2, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_14(M, C1, C2, a, b, ...)                                                        \
  M(C1, C2, a, b)                                                                                                      \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX2_12(M, C1, C2, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_16(M, C1, C2, a, b, ...)                                                        \
  M(C1, C2, a, b)                                                                                                      \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX2_14(M, C1, C2, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2(M, C1, C2, ...)                                                                 \
  ENUMLITE_PP_CAT(ENUMLITE_PP_FOR_EACH_PAIR_CTX2_, ENUMLITE_PP_NARG(__VA_ARGS__))(M, C1, C2, __VA_ARGS__)

// ============================================================================
// Enum helpers
// ============================================================================

#define ENUMLITE_DETAIL_ENUM_DECL(name, value) name = value,

#define ENUMLITE_DETAIL_ENUM_NAME(Backend, name, value) Backend::from_literal(#name),

#define ENUMLITE_DETAIL_ENUM_VALUE(Type, name, value) Type::name,

#define ENUMLITE_DETAIL_ENUM_VALUE_STRING(Backend, name, value)                                                        \
  Backend::append(result, Backend::from_literal(", "));                                                                \
  Backend::append(result, Backend::from_literal(#name));

#define ENUMLITE_DETAIL_ENUM_TO_STRING(Backend, Type, name, value_)                                                    \
  if (value == Type::name) return Backend::from_literal(#name);

#define ENUMLITE_DETAIL_ENUM_FROM_STRING(Backend, Type, name, value_)                                                  \
  if (enumlite::enum_string_equal_literal<Backend>(str, #name)) return Type::name;

// ============================================================================
// DEFINE_ENUM
// ============================================================================

#define DEFINE_ENUM(Type, Underlying, ...) DEFINE_ENUM_EX(Type, ENUMLITE_DEFAULT_BACKEND, Underlying, __VA_ARGS__)

#define DEFINE_ENUM_EX(Type, Backend, Underlying, ...)                                                                 \
  enum class Type : Underlying { NONE = 0, ENUMLITE_PP_FOR_EACH_PAIR(ENUMLITE_DETAIL_ENUM_DECL, __VA_ARGS__) };        \
                                                                                                                       \
  inline const auto Type##_names = [] {                                                                                \
    return std::array{Backend::from_literal("NONE"),                                                                   \
                      ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_ENUM_NAME, Backend, __VA_ARGS__)};                 \
  }();                                                                                                                 \
                                                                                                                       \
  inline const auto Type##_all = [] {                                                                                  \
    return std::array{Type::NONE, ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_ENUM_VALUE, Type, __VA_ARGS__)};       \
  }();                                                                                                                 \
                                                                                                                       \
  inline const auto Type##_values = [] {                                                                               \
    auto result = Backend::from_literal("NONE");                                                                       \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_ENUM_VALUE_STRING, Backend, __VA_ARGS__)                             \
    return result;                                                                                                     \
  }();                                                                                                                 \
                                                                                                                       \
  inline Backend::string_type Type##_to_str(Type value)                                                                \
  {                                                                                                                    \
    if (value == Type::NONE) return Backend::from_literal("NONE");                                                     \
                                                                                                                       \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_ENUM_TO_STRING, Backend, Type, __VA_ARGS__)                         \
                                                                                                                       \
    return {};                                                                                                         \
  }                                                                                                                    \
                                                                                                                       \
  inline Type Type##_from_str(const Backend::string_type& str)                                                         \
  {                                                                                                                    \
                                                                                                                       \
    if (enumlite::enum_string_equal_literal<Backend>(str, "NONE")) return Type::NONE;                                  \
                                                                                                                       \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_ENUM_FROM_STRING, Backend, Type, __VA_ARGS__)                       \
                                                                                                                       \
    return Type::NONE;                                                                                                 \
  }                                                                                                                    \
                                                                                                                       \
  inline bool Type##_is_valid(Type value) noexcept                                                                     \
  {                                                                                                                    \
    for (const auto v : Type##_all) {                                                                                  \
      if (v == value) return true;                                                                                     \
    }                                                                                                                  \
    return false;                                                                                                      \
  }

// ============================================================================
// Flag helpers
// ============================================================================

#define ENUMLITE_DETAIL_FLAG_DECL(name, value) name = value,

#define ENUMLITE_DETAIL_FLAG_OR_VALUE(Underlying, name, value) | static_cast<Underlying>(value)

#define ENUMLITE_DETAIL_FLAG_TO_STRING(Backend, Type, name, value_)                                                    \
  do {                                                                                                                 \
    if (Type##_has_flag(value, Type::name)) {                                                                          \
      if (!Backend::empty(result)) Backend::append(result, Backend::from_literal("|"));                                \
      Backend::append(result, Backend::from_literal(#name));                                                           \
    }                                                                                                                  \
  } while (false);

#define ENUMLITE_DETAIL_FLAG_TO_VEC_STR(Backend, Type, name, value_)                                                   \
  do {                                                                                                                 \
    if (Type##_has_flag(value, Type::name)) result.push_back(Backend::from_literal(#name));                            \
  } while (false);

#define ENUMLITE_DETAIL_FLAG_FROM_STRING(Backend, Type, name, value_)                                                  \
  if (enumlite::enum_string_equal_literal<Backend>(token, #name)) result |= Type::name;

// ============================================================================
// DEFINE_FLAGS
// ============================================================================

#define DEFINE_FLAGS(Type, Underlying, ...) DEFINE_FLAGS_EX(Type, ENUMLITE_DEFAULT_BACKEND, Underlying, __VA_ARGS__)

#define DEFINE_FLAGS_EX(Type, Backend, Underlying, ...)                                                                \
  enum class Type : Underlying {                                                                                       \
    NONE                                                                  = 0,                                         \
    ENUMLITE_PP_FOR_EACH_PAIR(ENUMLITE_DETAIL_FLAG_DECL, __VA_ARGS__) ALL = static_cast<Underlying>(0)                 \
        ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_FLAG_OR_VALUE, Underlying, __VA_ARGS__)                          \
  };                                                                                                                   \
                                                                                                                       \
  inline bool Type##_has_flag(Type value, Type flag) noexcept                                                          \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    return (static_cast<U>(value) & static_cast<U>(flag)) == static_cast<U>(flag);                                     \
  }                                                                                                                    \
                                                                                                                       \
  inline Type operator|(Type lhs, Type rhs) noexcept                                                                   \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    return static_cast<Type>(static_cast<U>(lhs) | static_cast<U>(rhs));                                               \
  }                                                                                                                    \
                                                                                                                       \
  inline Type operator&(Type lhs, Type rhs) noexcept                                                                   \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    return static_cast<Type>(static_cast<U>(lhs) & static_cast<U>(rhs));                                               \
  }                                                                                                                    \
                                                                                                                       \
  inline Type operator^(Type lhs, Type rhs) noexcept                                                                   \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    return static_cast<Type>(static_cast<U>(lhs) ^ static_cast<U>(rhs));                                               \
  }                                                                                                                    \
                                                                                                                       \
  inline Type operator~(Type value) noexcept                                                                           \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    return static_cast<Type>(~static_cast<U>(value));                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  inline Type& operator|=(Type& lhs, Type rhs) noexcept                                                                \
  {                                                                                                                    \
    lhs = lhs | rhs;                                                                                                   \
    return lhs;                                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  inline Type& operator&=(Type& lhs, Type rhs) noexcept                                                                \
  {                                                                                                                    \
    lhs = lhs & rhs;                                                                                                   \
    return lhs;                                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  inline Type& operator^=(Type& lhs, Type rhs) noexcept                                                                \
  {                                                                                                                    \
    lhs = lhs ^ rhs;                                                                                                   \
    return lhs;                                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  inline Backend::string_type Type##_to_str(Type value)                                                                \
  {                                                                                                                    \
    if (value == Type::NONE) return Backend::from_literal("NONE");                                                     \
                                                                                                                       \
    Backend::string_type result;                                                                                       \
                                                                                                                       \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_FLAG_TO_STRING, Backend, Type, __VA_ARGS__)                         \
                                                                                                                       \
    return result;                                                                                                     \
  }                                                                                                                    \
                                                                                                                       \
  inline enumlite::static_string_list<Backend, ENUMLITE_PP_NARG(__VA_ARGS__) / 2> Type##_to_vec_str(Type value)        \
  {                                                                                                                    \
    enumlite::static_string_list<Backend, ENUMLITE_PP_NARG(__VA_ARGS__) / 2> result;                                   \
                                                                                                                       \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_FLAG_TO_VEC_STR, Backend, Type, __VA_ARGS__)                        \
                                                                                                                       \
    return result;                                                                                                     \
  }                                                                                                                    \
                                                                                                                       \
  inline Type Type##_from_str(const Backend::string_type& str)                                                         \
  {                                                                                                                    \
                                                                                                                       \
    if (enumlite::enum_string_equal_literal<Backend>(str, "NONE")) return Type::NONE;                                  \
                                                                                                                       \
    Type result = Type::NONE;                                                                                          \
                                                                                                                       \
    typename Backend::size_type token_start = 0;                                                                       \
    const auto                  length      = Backend::size(str);                                                      \
                                                                                                                       \
    for (typename Backend::size_type i = 0; i <= length; ++i) {                                                        \
      if (i != length && Backend::at(str, i) != '|') continue;                                                         \
                                                                                                                       \
      auto token = Backend::substr(str, token_start, i - token_start);                                                 \
                                                                                                                       \
      ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_FLAG_FROM_STRING, Backend, Type, __VA_ARGS__)                     \
                                                                                                                       \
      token_start = i + 1;                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    return result;                                                                                                     \
  }                                                                                                                    \
                                                                                                                       \
  inline bool Type##_is_valid(Type value) noexcept                                                                     \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    const U raw = static_cast<U>(value);                                                                               \
    const U all = static_cast<U>(Type::ALL);                                                                           \
                                                                                                                       \
    return (raw | all) == all;                                                                                         \
  }