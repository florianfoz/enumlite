#pragma once

#include "enumlite_backend_std.h"

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

#ifndef ENUMLITE_DEFAULT_BACKEND
#define ENUMLITE_DEFAULT_BACKEND ::enumlite::std_backend
#endif

#ifndef ENUMLITE_CONSTEXPR
#define ENUMLITE_CONSTEXPR
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

    if (c != ' ' && c != '\t') {
      break;
    }

    ++first;
  }

  // Trim trailing spaces/tabs.
  while (last > first) {
    const auto c = Backend::at(input, last - 1);

    if (c != ' ' && c != '\t') {
      break;
    }

    --last;
  }

  // Ignore leading underscore.
  if (first < last && Backend::at(input, first) == '_') {
    ++first;
  }

  // Ignore trailing underscore.
  if (last > first && Backend::at(input, last - 1) == '_') {
    --last;
  }

  for (size_type i = first; i < last; ++i) {
    auto c = Backend::at(input, i);

    // '-' and '_' are equivalent.
    if (c == '-') {
      c = '_';
    }

    c = Backend::to_lower(c);

    Backend::append(result, c);
  }

  return result;
}

template <typename Backend>
bool string_equal(const typename Backend::string_type& lhs, const typename Backend::string_type& rhs)
{
  if (Backend::size(lhs) != Backend::size(rhs)) {
    return false;
  }

  const auto size = Backend::size(lhs);

  for (typename Backend::size_type i = 0; i < size; ++i) {
    if (Backend::at(lhs, i) != Backend::at(rhs, i)) {
      return false;
    }
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
bool ENUMLITE_CONSTEXPR enum_string_equal_literal(const typename Backend::string_type& value, const char* literal)
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
    if (size_ < Capacity) {
      values_[size_++] = std::move(value);
    }
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
// Recursive macro expansion
//
// C++20 __VA_OPT__ is used to detect the end of the argument list.
//
// The recursive expansion supports large lists without the previous
// artificial limit of 16 arguments.
// ============================================================================

#define ENUMLITE_PP_EMPTY()

#define ENUMLITE_PP_DEFER(id) id ENUMLITE_PP_EMPTY()

#define ENUMLITE_PP_OBSTRUCT(...) __VA_ARGS__ ENUMLITE_PP_DEFER(ENUMLITE_PP_EMPTY)()

// ----------------------------------------------------------------------------
// Expansion depth
//
// Enough expansion depth for several hundred pairs.
// ----------------------------------------------------------------------------

#define ENUMLITE_PP_EVAL0(...) __VA_ARGS__

#define ENUMLITE_PP_EVAL1(...) ENUMLITE_PP_EVAL0(ENUMLITE_PP_EVAL0(ENUMLITE_PP_EVAL0(__VA_ARGS__)))

#define ENUMLITE_PP_EVAL2(...) ENUMLITE_PP_EVAL1(ENUMLITE_PP_EVAL1(ENUMLITE_PP_EVAL1(__VA_ARGS__)))

#define ENUMLITE_PP_EVAL3(...) ENUMLITE_PP_EVAL2(ENUMLITE_PP_EVAL2(ENUMLITE_PP_EVAL2(__VA_ARGS__)))

#define ENUMLITE_PP_EVAL4(...) ENUMLITE_PP_EVAL3(ENUMLITE_PP_EVAL3(ENUMLITE_PP_EVAL3(__VA_ARGS__)))

#define ENUMLITE_PP_EVAL5(...) ENUMLITE_PP_EVAL4(ENUMLITE_PP_EVAL4(ENUMLITE_PP_EVAL4(__VA_ARGS__)))

#define ENUMLITE_PP_EVAL6(...) ENUMLITE_PP_EVAL5(ENUMLITE_PP_EVAL5(ENUMLITE_PP_EVAL5(__VA_ARGS__)))

#define ENUMLITE_PP_EVAL(...) ENUMLITE_PP_EVAL6(__VA_ARGS__)

// ============================================================================
// Count pairs
//
// Input:
//
//   A, 1, B, 2, C, 3
//
// Expands to:
//
//   1 + 1 + 1
//
// Therefore it can be used directly as a constant expression:
//
//   static_string_list<Backend, 1 + 1 + 1>
//
// No fixed argument-count macro is required.
// ============================================================================

#define ENUMLITE_PP_PAIR_COUNT(...) ENUMLITE_PP_EVAL(ENUMLITE_PP_PAIR_COUNT_I(__VA_ARGS__))

#define ENUMLITE_PP_PAIR_COUNT_I(a, b, ...)                                                                            \
  1 __VA_OPT__(+ENUMLITE_PP_OBSTRUCT(ENUMLITE_PP_PAIR_COUNT_INDIRECT)()(__VA_ARGS__))

#define ENUMLITE_PP_PAIR_COUNT_INDIRECT() ENUMLITE_PP_PAIR_COUNT_I

#define ENUMLITE_MAX_ENUM_ELEMENTS 255

#define ENUMLITE_CHECK_ENUM_SIZE(...)                                                                                  \
  static_assert(ENUMLITE_PP_PAIR_COUNT(__VA_ARGS__) <= ENUMLITE_MAX_ENUM_ELEMENTS,                                     \
                "enumlite: too many enum elements (maximum is 255)")

// ============================================================================
// Apply macro to pairs
// ============================================================================

#define ENUMLITE_PP_FOR_EACH_PAIR(M, ...) ENUMLITE_PP_EVAL(ENUMLITE_PP_FOR_EACH_PAIR_I(M, __VA_ARGS__))

#define ENUMLITE_PP_FOR_EACH_PAIR_I(M, a, b, ...)                                                                      \
  M(a, b)                                                                                                              \
  __VA_OPT__(ENUMLITE_PP_OBSTRUCT(ENUMLITE_PP_FOR_EACH_PAIR_INDIRECT)()(M, __VA_ARGS__))

#define ENUMLITE_PP_FOR_EACH_PAIR_INDIRECT() ENUMLITE_PP_FOR_EACH_PAIR_I

// ============================================================================
// Apply macro to pairs + one context
// ============================================================================

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX(M, C, ...) ENUMLITE_PP_EVAL(ENUMLITE_PP_FOR_EACH_PAIR_CTX_I(M, C, __VA_ARGS__))

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_I(M, C, a, b, ...)                                                               \
  M(C, a, b)                                                                                                           \
  __VA_OPT__(ENUMLITE_PP_OBSTRUCT(ENUMLITE_PP_FOR_EACH_PAIR_CTX_INDIRECT)()(M, C, __VA_ARGS__))

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_INDIRECT() ENUMLITE_PP_FOR_EACH_PAIR_CTX_I

// ============================================================================
// Apply macro to pairs + two contexts
// ============================================================================

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2(M, C1, C2, ...)                                                                 \
  ENUMLITE_PP_EVAL(ENUMLITE_PP_FOR_EACH_PAIR_CTX2_I(M, C1, C2, __VA_ARGS__))

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_I(M, C1, C2, a, b, ...)                                                         \
  M(C1, C2, a, b)                                                                                                      \
  __VA_OPT__(ENUMLITE_PP_OBSTRUCT(ENUMLITE_PP_FOR_EACH_PAIR_CTX2_INDIRECT)()(M, C1, C2, __VA_ARGS__))

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_INDIRECT() ENUMLITE_PP_FOR_EACH_PAIR_CTX2_I

// ============================================================================
// Enum helpers
// ============================================================================

#define ENUMLITE_DETAIL_ENUM_DECL(name, value) name = value,

#define ENUMLITE_DETAIL_ENUM_NAME(Backend, name, value) Backend::from_literal(#name),

#define ENUMLITE_DETAIL_ENUM_VALUE(Type, name, value) Type::name,

#define ENUMLITE_DETAIL_ENUM_VALUE_STRING(Backend, name, value) ", " #name

#define ENUMLITE_DETAIL_ENUM_TO_STRING(Backend, Type, name, value_)                                                    \
  if (value == Type::name) {                                                                                           \
    return Backend::from_literal(#name);                                                                               \
  }

#define ENUMLITE_DETAIL_ENUM_FROM_STRING(Backend, Type, name, value_)                                                  \
  if (enumlite::enum_string_equal_literal<Backend>(str, #name)) {                                                      \
    return Type::name;                                                                                                 \
  }

// ============================================================================
// DEFINE_ENUM
// ============================================================================

#define DEFINE_ENUM(Type, Underlying, ...) DEFINE_ENUM_EX(Type, ENUMLITE_DEFAULT_BACKEND, Underlying, __VA_ARGS__)

#define DEFINE_ENUM_EX(Type, Backend, Underlying, ...)                                                                 \
                                                                                                                       \
  ENUMLITE_CHECK_ENUM_SIZE(__VA_ARGS__);                                                                               \
                                                                                                                       \
  enum class Type : Underlying { NONE = 0, ENUMLITE_PP_FOR_EACH_PAIR(ENUMLITE_DETAIL_ENUM_DECL, __VA_ARGS__) };        \
                                                                                                                       \
  inline auto Type##_names = std::array{                                                                               \
      Backend::from_literal("NONE"), ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_ENUM_NAME, Backend, __VA_ARGS__)};  \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR auto Type##_all =                                                                          \
      std::array{Type::NONE, ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_ENUM_VALUE, Type, __VA_ARGS__)};            \
                                                                                                                       \
  inline auto Type##_values = Backend::from_literal(                                                                   \
      "NONE" ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_ENUM_VALUE_STRING, Backend, __VA_ARGS__));                  \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Backend::string_type Type##_to_str(Type value)                                             \
  {                                                                                                                    \
    if (value == Type::NONE) {                                                                                         \
      return Backend::from_literal("NONE");                                                                            \
    }                                                                                                                  \
                                                                                                                       \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_ENUM_TO_STRING, Backend, Type, __VA_ARGS__)                         \
                                                                                                                       \
    return {};                                                                                                         \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Type Type##_from_str(const Backend::string_type& str)                                      \
  {                                                                                                                    \
    if (enumlite::enum_string_equal_literal<Backend>(str, "NONE")) {                                                   \
      return Type::NONE;                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_ENUM_FROM_STRING, Backend, Type, __VA_ARGS__)                       \
                                                                                                                       \
    return Type::NONE;                                                                                                 \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR bool Type##_is_valid(Type value) noexcept                                                  \
  {                                                                                                                    \
    for (const auto v : Type##_all) {                                                                                  \
      if (v == value) {                                                                                                \
        return true;                                                                                                   \
      }                                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
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
      if (!Backend::empty(result)) {                                                                                   \
        Backend::append(result, Backend::from_literal("|"));                                                           \
      }                                                                                                                \
                                                                                                                       \
      Backend::append(result, Backend::from_literal(#name));                                                           \
    }                                                                                                                  \
  } while (false);

#define ENUMLITE_DETAIL_FLAG_TO_VEC_STR(Backend, Type, name, value_)                                                   \
  do {                                                                                                                 \
    if (Type##_has_flag(value, Type::name)) {                                                                          \
      result.push_back(Backend::from_literal(#name));                                                                  \
    }                                                                                                                  \
  } while (false);

#define ENUMLITE_DETAIL_FLAG_FROM_STRING(Backend, Type, name, value_)                                                  \
  if (enumlite::enum_string_equal_literal<Backend>(token, #name)) {                                                    \
    result |= Type::name;                                                                                              \
  }

// ============================================================================
// DEFINE_FLAGS
// ============================================================================

#define DEFINE_FLAGS(Type, Underlying, ...) DEFINE_FLAGS_EX(Type, ENUMLITE_DEFAULT_BACKEND, Underlying, __VA_ARGS__)

#define DEFINE_FLAGS_EX(Type, Backend, Underlying, ...)                                                                \
                                                                                                                       \
  ENUMLITE_CHECK_ENUM_SIZE(__VA_ARGS__);                                                                               \
                                                                                                                       \
  enum class Type : Underlying {                                                                                       \
    NONE                                                                  = 0,                                         \
    ENUMLITE_PP_FOR_EACH_PAIR(ENUMLITE_DETAIL_FLAG_DECL, __VA_ARGS__) ALL = static_cast<Underlying>(0)                 \
        ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_FLAG_OR_VALUE, Underlying, __VA_ARGS__)                          \
  };                                                                                                                   \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR bool Type##_has_flag(Type value, Type flag) noexcept                                       \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    return (static_cast<U>(value) & static_cast<U>(flag)) == static_cast<U>(flag);                                     \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Type operator|(Type lhs, Type rhs) noexcept                                                \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    return static_cast<Type>(static_cast<U>(lhs) | static_cast<U>(rhs));                                               \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Type operator&(Type lhs, Type rhs) noexcept                                                \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    return static_cast<Type>(static_cast<U>(lhs) & static_cast<U>(rhs));                                               \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Type operator^(Type lhs, Type rhs) noexcept                                                \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    return static_cast<Type>(static_cast<U>(lhs) ^ static_cast<U>(rhs));                                               \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Type operator~(Type value) noexcept                                                        \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    return static_cast<Type>(~static_cast<U>(value));                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Type& operator|=(Type& lhs, Type rhs) noexcept                                             \
  {                                                                                                                    \
    lhs = lhs | rhs;                                                                                                   \
    return lhs;                                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Type& operator&=(Type& lhs, Type rhs) noexcept                                             \
  {                                                                                                                    \
    lhs = lhs & rhs;                                                                                                   \
    return lhs;                                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Type& operator^=(Type& lhs, Type rhs) noexcept                                             \
  {                                                                                                                    \
    lhs = lhs ^ rhs;                                                                                                   \
    return lhs;                                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Backend::string_type Type##_to_str(Type value)                                             \
  {                                                                                                                    \
    if (value == Type::NONE) {                                                                                         \
      return Backend::from_literal("NONE");                                                                            \
    }                                                                                                                  \
                                                                                                                       \
    Backend::string_type result;                                                                                       \
                                                                                                                       \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_FLAG_TO_STRING, Backend, Type, __VA_ARGS__)                         \
                                                                                                                       \
    return result;                                                                                                     \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR enumlite::static_string_list<Backend, ENUMLITE_PP_PAIR_COUNT(__VA_ARGS__)>                 \
                            Type##_to_vec_str(Type value)                                                              \
  {                                                                                                                    \
    enumlite::static_string_list<Backend, ENUMLITE_PP_PAIR_COUNT(__VA_ARGS__)> result;                                 \
                                                                                                                       \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_FLAG_TO_VEC_STR, Backend, Type, __VA_ARGS__)                        \
                                                                                                                       \
    return result;                                                                                                     \
  }                                                                                                                    \
                                                                                                                       \
  inline ENUMLITE_CONSTEXPR Type Type##_from_str(const Backend::string_type& str)                                      \
  {                                                                                                                    \
    if (enumlite::enum_string_equal_literal<Backend>(str, "NONE")) {                                                   \
      return Type::NONE;                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    Type result = Type::NONE;                                                                                          \
                                                                                                                       \
    typename Backend::size_type token_start = 0;                                                                       \
    const auto                  length      = Backend::size(str);                                                      \
                                                                                                                       \
    for (typename Backend::size_type i = 0; i <= length; ++i) {                                                        \
      if (i != length && Backend::at(str, i) != '|') {                                                                 \
        continue;                                                                                                      \
      }                                                                                                                \
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
  inline ENUMLITE_CONSTEXPR bool Type##_is_valid(Type value) noexcept                                                  \
  {                                                                                                                    \
    using U = std::underlying_type_t<Type>;                                                                            \
                                                                                                                       \
    const U raw = static_cast<U>(value);                                                                               \
    const U all = static_cast<U>(Type::ALL);                                                                           \
                                                                                                                       \
    return (raw | all) == all;                                                                                         \
  }
