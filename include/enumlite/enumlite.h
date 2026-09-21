#pragma once

#include "enumlite_backend_std.h"

#include <array>
#include <cstddef>
#include <type_traits>

#ifndef ENUMLITE_DEFAULT_BACKEND
#define ENUMLITE_DEFAULT_BACKEND ::enumlite::std_backend
#endif

namespace enumlite {

// ============================================================================
// Backend-independent helpers
// ============================================================================

template <typename Backend> struct basic_enumlite {
  using backend_type = Backend;
  using string_type = typename Backend::string_type;
  using string_view_type = typename Backend::string_view_type;
  using char_type = typename Backend::char_type;
  using size_type = typename Backend::size_type;
};

// ============================================================================
// String normalization
//
// Rules:
//
//   "Foo"   == "foo"
//   "foo_"  == "foo"
//   "_foo"  == "foo"
//   "foo-bar" == "foo_bar"
//   spaces/tabs are ignored around the value
// ============================================================================

template <typename Backend>
typename Backend::string_type
normalize_enum_string(typename Backend::string_type value) {
  using string_type = typename Backend::string_type;
  using size_type = typename Backend::size_type;
  using char_type = typename Backend::char_type;

  // Trim leading spaces/tabs.
  while (!Backend::empty(value)) {
    const char_type c = Backend::front(value);

    if (c == ' ' || c == '\t')
      Backend::erase(value, 0, 1);
    else
      break;
  }

  // Trim trailing spaces/tabs.
  while (!Backend::empty(value)) {
    const char_type c = Backend::back(value);

    if (c == ' ' || c == '\t')
      Backend::erase(value, Backend::size(value) - 1, 1);
    else
      break;
  }

  // Leading '_' is ignored.
  if (!Backend::empty(value) && Backend::front(value) == '_') {
    Backend::erase(value, 0, 1);
  }

  // Trailing '_' is ignored.
  if (!Backend::empty(value) && Backend::back(value) == '_') {
    Backend::erase(value, Backend::size(value) - 1, 1);
  }

  // Normalize '-' to '_'.
  for (size_type i = 0; i < Backend::size(value); ++i) {
    if (Backend::at(value, i) == '-') {
      // We cannot directly replace through the backend interface,
      // so rebuild the string.
      string_type result;

      for (size_type j = 0; j < Backend::size(value); ++j) {
        char_type c = Backend::at(value, j);

        if (c == '-')
          c = '_';

        Backend::append(result, c);
      }

      value = result;
      break;
    }
  }

  // ASCII lowercase.
  for (size_type i = 0; i < Backend::size(value); ++i) {
    // Rebuild because the backend interface intentionally does not
    // require mutable character references.
    string_type result;

    for (size_type j = 0; j < Backend::size(value); ++j) {
      const char_type c = Backend::to_lower(Backend::at(value, j));

      Backend::append(result, c);
    }

    value = result;
    break;
  }

  return value;
}

template <typename Backend>
bool enum_string_equal(const typename Backend::string_type &lhs,
                       const typename Backend::string_type &rhs) {
  return normalize_enum_string<Backend>(lhs) ==
         normalize_enum_string<Backend>(rhs);
}

template <typename Backend>
bool enum_string_equal_literal(const typename Backend::string_type &lhs,
                               const char *rhs) {
  return enum_string_equal<Backend>(lhs, Backend::from_literal(rhs));
}

// ============================================================================
// Remove enum suffix from generated names.
//
// Example:
//
//   Foo_ -> Foo
//   Foo  -> Foo
// ============================================================================

template <typename Backend>
typename Backend::string_type
strip_enum_suffix(typename Backend::string_type value) {
  if (!Backend::empty(value) && Backend::back(value) == '_') {
    Backend::erase(value, Backend::size(value) - 1, 1);
  }

  return value;
}

// ============================================================================
// static_string_list
// ============================================================================

template <typename Backend, std::size_t N> class static_string_list {
public:
  using backend_type = Backend;
  using string_type = typename Backend::string_type;
  using size_type = typename Backend::size_type;

  constexpr static_string_list() = default;

  constexpr std::size_t count() const noexcept { return N; }

  constexpr bool empty() const noexcept { return N == 0; }

  const string_type &operator[](std::size_t index) const noexcept {
    return values_[index];
  }

  const string_type *begin() const noexcept { return values_.data(); }

  const string_type *end() const noexcept { return values_.data() + N; }

  std::array<string_type, N> &data() noexcept { return values_; }

  const std::array<string_type, N> &data() const noexcept { return values_; }

private:
  std::array<string_type, N> values_{};
};

} // namespace enumlite

// ============================================================================
// Preprocessor utilities
// ============================================================================

#define ENUMLITE_DETAIL_EXPAND(x) x

#define ENUMLITE_DETAIL_EMPTY(...)

#define ENUMLITE_DETAIL_DEFER(id) id ENUMLITE_DETAIL_EMPTY()

#define ENUMLITE_DETAIL_OBSTRUCT(...)                                          \
  __VA_ARGS__                                                                  \
  ENUMLITE_DETAIL_DEFER(ENUMLITE_DETAIL_EMPTY)()

#define ENUMLITE_DETAIL_PARENTHESIS(...) (__VA_ARGS__)

// ----------------------------------------------------------------------------
// FOR_EACH_PAIR
//
// Input:
//   A, 1, B, 2, C, 3
//
// Expands:
//   macro(A,1) macro(B,2) macro(C,3)
// ----------------------------------------------------------------------------

#define ENUMLITE_PP_FOR_EACH_PAIR(macro, ...)                                  \
  ENUMLITE_PP_FOR_EACH_PAIR_I(macro, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_I(macro, name, value, ...)                   \
  macro(name, value)                                                           \
      __VA_OPT__(ENUMLITE_PP_FOR_EACH_PAIR_AGAIN()(macro, __VA_ARGS__))

#define ENUMLITE_PP_FOR_EACH_PAIR_AGAIN() ENUMLITE_PP_FOR_EACH_PAIR_I

// ----------------------------------------------------------------------------
// FOR_EACH_PAIR_CTX
//
// Input:
//   macro, context, A,1,B,2
//
// Expands:
//   macro(context,A,1) macro(context,B,2)
// ----------------------------------------------------------------------------

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX(macro, ctx, ...)                         \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX_I(macro, ctx, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_I(macro, ctx, name, value, ...)          \
  macro(ctx, name, value) __VA_OPT__(                                          \
      ENUMLITE_PP_FOR_EACH_PAIR_CTX_AGAIN()(macro, ctx, __VA_ARGS__))

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX_AGAIN() ENUMLITE_PP_FOR_EACH_PAIR_CTX_I

// ----------------------------------------------------------------------------
// FOR_EACH_PAIR_CTX2
// ----------------------------------------------------------------------------

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2(macro, ctx1, ctx2, ...)                 \
  ENUMLITE_PP_FOR_EACH_PAIR_CTX2_I(macro, ctx1, ctx2, __VA_ARGS__)

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_I(macro, ctx1, ctx2, name, value, ...)  \
  macro(ctx1, ctx2, name, value) __VA_OPT__(                                   \
      ENUMLITE_PP_FOR_EACH_PAIR_CTX2_AGAIN()(macro, ctx1, ctx2, __VA_ARGS__))

#define ENUMLITE_PP_FOR_EACH_PAIR_CTX2_AGAIN() ENUMLITE_PP_FOR_EACH_PAIR_CTX2_I

// ============================================================================
// Internal generators
// ============================================================================

#define ENUMLITE_DETAIL_COUNT(name, value) +1

#define ENUMLITE_DETAIL_ENUM_NAME(Backend, name, value)                        \
  Backend::from_literal(#name),

#define ENUMLITE_DETAIL_ENUM_VALUE(Type, name, value) Type::name,

#define ENUMLITE_DETAIL_ENUM_TO_STRING(Backend, Type, name, value)             \
  if (value == Type::name)                                                     \
    return Backend::from_literal(#name);

#define ENUMLITE_DETAIL_ENUM_FROM_STRING(Backend, Type, name, value)           \
  if (enumlite::enum_string_equal_literal<Backend>(str, #name))                \
    return Type::name;

#define ENUMLITE_DETAIL_ENUM_APPEND_VALUE(Backend, name, value)                \
  enumlite::BackendAppendHelper<Backend>::append_name(result, #name, value);

// ============================================================================
// Internal backend helper
// ============================================================================

namespace enumlite {

template <typename Backend> struct BackendAppendHelper {
  static void append_name(typename Backend::string_type &result,
                          const char *name, auto) {
    if (!Backend::empty(result))
      Backend::append(result, Backend::from_literal(", "));

    Backend::append(result, Backend::from_literal(name));
  }
};

} // namespace enumlite

// ============================================================================
// DEFINE_ENUM
// ============================================================================

#define ENUMLITE_DEFINE_ENUM(Type, Backend, Underlying, ...)                   \
                                                                               \
  enum class Type : Underlying {                                               \
    NONE = 0,                                                                  \
    ENUMLITE_PP_FOR_EACH_PAIR(ENUMLITE_DETAIL_ENUM_DECL, __VA_ARGS__)          \
  };                                                                           \
                                                                               \
  inline const auto Type##_names =                                             \
      std::array<typename Backend::string_type,                                \
                 1 ENUMLITE_PP_FOR_EACH_PAIR(ENUMLITE_DETAIL_COUNT,            \
                                             __VA_ARGS__)>{                    \
          Backend::from_literal("NONE"),                                       \
          ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_ENUM_NAME, Backend,    \
                                        __VA_ARGS__)};                         \
                                                                               \
  inline const auto Type##_all =                                               \
      std::array<Type, 1 ENUMLITE_PP_FOR_EACH_PAIR(ENUMLITE_DETAIL_COUNT,      \
                                                   __VA_ARGS__)>{              \
          Type::NONE, ENUMLITE_PP_FOR_EACH_PAIR_CTX(                           \
                          ENUMLITE_DETAIL_ENUM_VALUE, Type, __VA_ARGS__)};     \
                                                                               \
  inline const auto Type##_values = [] {                                       \
    typename Backend::string_type result = Backend::from_literal("NONE");      \
                                                                               \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_ENUM_VALUE_STRING, Backend,  \
                                  __VA_ARGS__)                                 \
                                                                               \
    return result;                                                             \
  }();                                                                         \
                                                                               \
  inline typename Backend::string_type Type##_to_str(Type value) {             \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_ENUM_TO_STRING, Backend,    \
                                   Type, __VA_ARGS__)                          \
                                                                               \
    if (value == Type::NONE)                                                   \
      return Backend::from_literal("NONE");                                    \
                                                                               \
    return {};                                                                 \
  }                                                                            \
                                                                               \
  inline Type Type##_from_str(const typename Backend::string_type &str) {      \
    if (enumlite::enum_string_equal_literal<Backend>(str, "NONE"))             \
      return Type::NONE;                                                       \
                                                                               \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_ENUM_FROM_STRING, Backend,  \
                                   Type, __VA_ARGS__)                          \
                                                                               \
    return Type::NONE;                                                         \
  }                                                                            \
                                                                               \
  inline bool Type##_is_valid(Type value) {                                    \
    for (const auto v : Type##_all) {                                          \
      if (v == value)                                                          \
        return true;                                                           \
    }                                                                          \
                                                                               \
    return false;                                                              \
  }

// ============================================================================
// Missing internal enum macros
// ============================================================================

#define ENUMLITE_DETAIL_ENUM_DECL(name, value) name = value,

#define ENUMLITE_DETAIL_ENUM_VALUE_STRING(Backend, name, value)                \
  do {                                                                         \
    Backend::append(result, Backend::from_literal(", "));                      \
    Backend::append(result, Backend::from_literal(#name));                     \
  } while (false)

// ============================================================================
// Public enum macros
// ============================================================================

#define DEFINE_ENUM_EX(Type, Backend, Underlying, ...)                         \
  ENUMLITE_DEFINE_ENUM(Type, Backend, Underlying, __VA_ARGS__)

#define DEFINE_ENUM(Type, Underlying, ...)                                     \
  DEFINE_ENUM_EX(Type, ENUMLITE_DEFAULT_BACKEND, Underlying, __VA_ARGS__)

// ============================================================================
// Flags generators
// ============================================================================

#define ENUMLITE_DETAIL_FLAG_DECL(name, value) name = value,

#define ENUMLITE_DETAIL_FLAG_VALUE(Type, name, value) Type::name,

#define ENUMLITE_DETAIL_FLAG_NAME(Backend, name, value)                        \
  Backend::from_literal(#name),

#define ENUMLITE_DETAIL_FLAG_TO_STRING(Backend, Type, name, value)             \
  if (Type##_has_flag(value, Type::name)) {                                    \
    if (!Backend::empty(result))                                               \
      Backend::append(result, Backend::from_literal("|"));                     \
                                                                               \
    Backend::append(result, Backend::from_literal(#name));                     \
  }

#define ENUMLITE_DETAIL_FLAG_FROM_STRING(Backend, Type, name, value)           \
  if (enumlite::enum_string_equal_literal<Backend>(token, #name))              \
    result = static_cast<Type>(static_cast<Underlying>(result) |               \
                               static_cast<Underlying>(Type::name));

// ============================================================================
// DEFINE_FLAGS
// ============================================================================

#define ENUMLITE_DEFINE_FLAGS(Type, Backend, Underlying, ...)                  \
                                                                               \
  enum class Type : Underlying {                                               \
    NONE = 0,                                                                  \
    ENUMLITE_PP_FOR_EACH_PAIR(ENUMLITE_DETAIL_FLAG_DECL, __VA_ARGS__) ALL =    \
        ENUMLITE_PP_FOR_EACH_PAIR(ENUMLITE_DETAIL_FLAG_OR_VALUE, __VA_ARGS__)  \
  };                                                                           \
                                                                               \
  inline constexpr Type operator|(Type lhs, Type rhs) noexcept {               \
    return static_cast<Type>(static_cast<Underlying>(lhs) |                    \
                             static_cast<Underlying>(rhs));                    \
  }                                                                            \
                                                                               \
  inline constexpr Type operator&(Type lhs, Type rhs) noexcept {               \
    return static_cast<Type>(static_cast<Underlying>(lhs) &                    \
                             static_cast<Underlying>(rhs));                    \
  }                                                                            \
                                                                               \
  inline constexpr Type operator^(Type lhs, Type rhs) noexcept {               \
    return static_cast<Type>(static_cast<Underlying>(lhs) ^                    \
                             static_cast<Underlying>(rhs));                    \
  }                                                                            \
                                                                               \
  inline constexpr Type operator~(Type value) noexcept {                       \
    return static_cast<Type>(~static_cast<Underlying>(value));                 \
  }                                                                            \
                                                                               \
  inline Type &operator|=(Type &lhs, Type rhs) noexcept {                      \
    lhs = lhs | rhs;                                                           \
    return lhs;                                                                \
  }                                                                            \
                                                                               \
  inline Type &operator&=(Type &lhs, Type rhs) noexcept {                      \
    lhs = lhs & rhs;                                                           \
    return lhs;                                                                \
  }                                                                            \
                                                                               \
  inline Type &operator^=(Type &lhs, Type rhs) noexcept {                      \
    lhs = lhs ^ rhs;                                                           \
    return lhs;                                                                \
  }                                                                            \
                                                                               \
  inline constexpr bool Type##_has_flag(Type value, Type flag) noexcept {      \
    return (static_cast<Underlying>(value) & static_cast<Underlying>(flag)) == \
           static_cast<Underlying>(flag);                                      \
  }                                                                            \
                                                                               \
  inline const auto Type##_names =                                             \
      std::array<typename Backend::string_type,                                \
                 1 ENUMLITE_PP_FOR_EACH_PAIR(ENUMLITE_DETAIL_COUNT,            \
                                             __VA_ARGS__)>{                    \
          Backend::from_literal("NONE"),                                       \
          ENUMLITE_PP_FOR_EACH_PAIR_CTX(ENUMLITE_DETAIL_FLAG_NAME, Backend,    \
                                        __VA_ARGS__)};                         \
                                                                               \
  inline typename Backend::string_type Type##_to_str(Type value) {             \
    if (value == Type::NONE)                                                   \
      return Backend::from_literal("NONE");                                    \
                                                                               \
    typename Backend::string_type result;                                      \
                                                                               \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_FLAG_TO_STRING, Backend,    \
                                   Type, __VA_ARGS__)                          \
                                                                               \
    return result;                                                             \
  }                                                                            \
                                                                               \
  inline enumlite::static_string_list<Backend,                                 \
                                      1 ENUMLITE_PP_FOR_EACH_PAIR(             \
                                          ENUMLITE_DETAIL_COUNT, __VA_ARGS__)> \
      Type##_to_vec_str(Type value) {                                          \
    enumlite::static_string_list<Backend,                                      \
                                 1 ENUMLITE_PP_FOR_EACH_PAIR(                  \
                                     ENUMLITE_DETAIL_COUNT, __VA_ARGS__)>      \
        result;                                                                \
                                                                               \
    std::size_t index = 0;                                                     \
    result.data()[index++] = Backend::from_literal("NONE");                    \
                                                                               \
    ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_FLAG_VEC_STRING, Backend,   \
                                   Type, __VA_ARGS__)                          \
                                                                               \
    return result;                                                             \
  }                                                                            \
                                                                               \
  inline Type Type##_from_str(const typename Backend::string_type &str) {      \
    if (enumlite::enum_string_equal_literal<Backend>(str, "NONE"))             \
      return Type::NONE;                                                       \
                                                                               \
    Type result = Type::NONE;                                                  \
    typename Backend::string_type token;                                       \
                                                                               \
    for (typename Backend::size_type i = 0; i <= Backend::size(str); ++i) {    \
      const bool end = i == Backend::size(str) || Backend::at(str, i) == '|';  \
                                                                               \
      if (!end) {                                                              \
        Backend::append(token, Backend::at(str, i));                           \
        continue;                                                              \
      }                                                                        \
                                                                               \
      ENUMLITE_PP_FOR_EACH_PAIR_CTX2(ENUMLITE_DETAIL_FLAG_PARSE_TOKEN,         \
                                     Backend, Type, __VA_ARGS__)               \
                                                                               \
      token = {};                                                              \
    }                                                                          \
                                                                               \
    return result;                                                             \
  }                                                                            \
                                                                               \
  inline bool Type##_is_valid(Type value) noexcept {                           \
    return (static_cast<Underlying>(value) &                                   \
            ~static_cast<Underlying>(Type::ALL)) == 0;                         \
  }

// ============================================================================
// Flag internal helpers
// ============================================================================

#define ENUMLITE_DETAIL_FLAG_OR_VALUE(name, value)                             \
  | static_cast<Underlying>(value)

#define ENUMLITE_DETAIL_FLAG_VEC_STRING(Backend, Type, name, value)            \
  if (Type##_has_flag(value, Type::name)) {                                    \
    result.data()[index++] = Backend::from_literal(#name);                     \
  }

#define ENUMLITE_DETAIL_FLAG_PARSE_TOKEN(Backend, Type, name, value)           \
  if (enumlite::enum_string_equal_literal<Backend>(token, #name)) {            \
    result = static_cast<Type>(static_cast<Underlying>(result) |               \
                               static_cast<Underlying>(Type::name));           \
  }

// ============================================================================
// Public flags macros
// ============================================================================

#define DEFINE_FLAGS_EX(Type, Backend, Underlying, ...)                        \
  ENUMLITE_DEFINE_FLAGS(Type, Backend, Underlying, __VA_ARGS__)

#define DEFINE_FLAGS(Type, Underlying, ...)                                    \
  DEFINE_FLAGS_EX(Type, ENUMLITE_DEFAULT_BACKEND, Underlying, __VA_ARGS__)