#include <cassert>
#include <enumlite/enumlite.h>
#include <string>
#include <type_traits>

// ============================================================================
// Test enums
// ============================================================================

DEFINE_ENUM(Color, int, RED, 1, GREEN, 2, BLUE, 3)

DEFINE_ENUM(Status, unsigned char, IDLE, 0x01, RUNNING, 0x02, STOPPED, 0x04)

// ============================================================================
// Test flags
// ============================================================================

DEFINE_FLAGS(Permissions, unsigned, READ, 1U, WRITE, 2U, EXECUTE, 4U)

DEFINE_FLAGS(Features, unsigned char, A, 0x01, B, 0x02, C, 0x04)

// ============================================================================
// Helpers
// ============================================================================

static void test_enum_basic()
{
  static_assert(std::is_enum_v<Color>);
  static_assert(std::is_same_v<std::underlying_type_t<Color>, int>);

  assert(Color::NONE == static_cast<Color>(0));
  assert(Color::RED == static_cast<Color>(1));
  assert(Color::GREEN == static_cast<Color>(2));
  assert(Color::BLUE == static_cast<Color>(3));
}

static void test_enum_metadata()
{
  assert(Color_names.size() == 4);
  assert(Color_names[0] == "NONE");
  assert(Color_names[1] == "RED");
  assert(Color_names[2] == "GREEN");
  assert(Color_names[3] == "BLUE");

  assert(Color_all.size() == 4);
  assert(Color_all[0] == Color::NONE);
  assert(Color_all[1] == Color::RED);
  assert(Color_all[2] == Color::GREEN);
  assert(Color_all[3] == Color::BLUE);

  assert(Color_values == "NONE, RED, GREEN, BLUE");
}

static void test_enum_to_string()
{
  assert(Color_to_str(Color::NONE) == "NONE");
  assert(Color_to_str(Color::RED) == "RED");
  assert(Color_to_str(Color::GREEN) == "GREEN");
  assert(Color_to_str(Color::BLUE) == "BLUE");

  // Unknown enum value.
  assert(Color_to_str(static_cast<Color>(42)).empty());
}

static void test_enum_from_string()
{
  assert(Color_from_str("NONE") == Color::NONE);
  assert(Color_from_str("RED") == Color::RED);
  assert(Color_from_str("GREEN") == Color::GREEN);
  assert(Color_from_str("BLUE") == Color::BLUE);

  // Case insensitive.
  assert(Color_from_str("red") == Color::RED);
  assert(Color_from_str("GrEeN") == Color::GREEN);

  // '-' and '_' are equivalent.
  assert(Color_from_str("red") == Color::RED);
  assert(Color_from_str("_RED") == Color::RED);
  assert(Color_from_str("RED_") == Color::RED);
  assert(Color_from_str(" _RED_ ") == Color::RED);

  // Spaces and tabs.
  assert(Color_from_str(" \t BLUE \t ") == Color::BLUE);

  // Unknown value.
  assert(Color_from_str("UNKNOWN") == Color::NONE);
}

static void test_enum_validation()
{
  assert(Color_is_valid(Color::NONE));
  assert(Color_is_valid(Color::RED));
  assert(Color_is_valid(Color::GREEN));
  assert(Color_is_valid(Color::BLUE));

  assert(!Color_is_valid(static_cast<Color>(42)));
  assert(!Color_is_valid(static_cast<Color>(255)));
}

// ============================================================================
// Flags
// ============================================================================

static void test_flags_underlying()
{
  static_assert(std::is_enum_v<Permissions>);
  static_assert(std::is_same_v<std::underlying_type_t<Permissions>, unsigned>);

  static_assert(std::is_enum_v<Features>);
  static_assert(std::is_same_v<std::underlying_type_t<Features>, unsigned char>);
}

static void test_flags_all()
{
  constexpr auto all = static_cast<unsigned>(Permissions::ALL);

  constexpr auto expected = static_cast<unsigned>(Permissions::READ) | static_cast<unsigned>(Permissions::WRITE)
                            | static_cast<unsigned>(Permissions::EXECUTE);

  static_assert(all == expected);
  static_assert(all == 7U);

  assert(static_cast<unsigned>(Permissions::ALL) == 7U);

  constexpr auto features_all = static_cast<unsigned char>(Features::ALL);

  static_assert(features_all == 0x07);
  assert(static_cast<unsigned char>(Features::ALL) == 0x07);
}

static void test_flags_operators()
{
  const auto read_write = Permissions::READ | Permissions::WRITE;

  assert(static_cast<unsigned>(read_write) == 3U);

  const auto write_execute = Permissions::WRITE | Permissions::EXECUTE;

  assert(static_cast<unsigned>(write_execute) == 6U);

  const auto intersection = read_write & Permissions::WRITE;

  assert(intersection == Permissions::WRITE);

  const auto xor_value = read_write ^ Permissions::WRITE;

  assert(xor_value == Permissions::READ);

  const auto inverted = ~Permissions::NONE;

  assert(static_cast<unsigned>(inverted) == ~static_cast<unsigned>(Permissions::NONE));
}

static void test_flags_assignment_operators()
{
  Permissions value = Permissions::NONE;

  value |= Permissions::READ;
  assert(value == Permissions::READ);

  value |= Permissions::WRITE;
  assert(value == (Permissions::READ | Permissions::WRITE));

  value &= Permissions::WRITE;
  assert(value == Permissions::WRITE);

  value ^= Permissions::WRITE;
  assert(value == Permissions::NONE);

  value ^= Permissions::EXECUTE;
  assert(value == Permissions::EXECUTE);
}

static void test_flags_has_flag()
{
  const auto value = Permissions::READ | Permissions::WRITE;

  assert(Permissions_has_flag(value, Permissions::READ));
  assert(Permissions_has_flag(value, Permissions::WRITE));
  assert(!Permissions_has_flag(value, Permissions::EXECUTE));

  assert(Permissions_has_flag(Permissions::ALL, Permissions::READ));

  assert(Permissions_has_flag(Permissions::ALL, Permissions::WRITE));

  assert(Permissions_has_flag(Permissions::ALL, Permissions::EXECUTE));

  assert(Permissions_has_flag(Permissions::NONE, Permissions::NONE));
}

static void test_flags_to_string()
{
  assert(Permissions_to_str(Permissions::NONE) == "NONE");

  assert(Permissions_to_str(Permissions::READ) == "READ");

  assert(Permissions_to_str(Permissions::READ | Permissions::WRITE) == "READ|WRITE");

  assert(Permissions_to_str(Permissions::READ | Permissions::WRITE | Permissions::EXECUTE) == "READ|WRITE|EXECUTE");

  assert(Permissions_to_str(Permissions::ALL) == "READ|WRITE|EXECUTE");
}

static void test_flags_to_vec_string()
{
  {
    const auto result = Permissions_to_vec_str(Permissions::NONE);

    assert(result.size() == 0);
    assert(result.empty());
  }

  {
    const auto result = Permissions_to_vec_str(Permissions::READ);

    assert(result.size() == 1);
    assert(result[0] == "READ");
  }

  {
    const auto result = Permissions_to_vec_str(Permissions::READ | Permissions::WRITE);

    assert(result.size() == 2);
    assert(result[0] == "READ");
    assert(result[1] == "WRITE");
  }

  {
    const auto result = Permissions_to_vec_str(Permissions::ALL);

    assert(result.size() == 3);
    assert(result[0] == "READ");
    assert(result[1] == "WRITE");
    assert(result[2] == "EXECUTE");
  }
}

static void test_flags_from_string()
{
  assert(Permissions_from_str("NONE") == Permissions::NONE);

  assert(Permissions_from_str("READ") == Permissions::READ);

  assert(Permissions_from_str("READ|WRITE") == (Permissions::READ | Permissions::WRITE));

  assert(Permissions_from_str("READ|WRITE|EXECUTE") == Permissions::ALL);

  // Case insensitive.
  assert(Permissions_from_str("read|WRITE") == (Permissions::READ | Permissions::WRITE));

  // Leading/trailing spaces.
  assert(Permissions_from_str(" READ | WRITE ") == (Permissions::READ | Permissions::WRITE));

  // Leading/trailing underscores.
  assert(Permissions_from_str("_READ_|_WRITE_") == (Permissions::READ | Permissions::WRITE));

  // '-' and '_' are equivalent.
  assert(Permissions_from_str("READ|WRITE") == (Permissions::READ | Permissions::WRITE));

  // Unknown token is ignored by the current API.
  assert(Permissions_from_str("UNKNOWN") == Permissions::NONE);

  assert(Permissions_from_str("READ|UNKNOWN") == Permissions::READ);
}

static void test_flags_validation()
{
  assert(Permissions_is_valid(Permissions::NONE));
  assert(Permissions_is_valid(Permissions::READ));
  assert(Permissions_is_valid(Permissions::WRITE));
  assert(Permissions_is_valid(Permissions::EXECUTE));
  assert(Permissions_is_valid(Permissions::READ | Permissions::WRITE));
  assert(Permissions_is_valid(Permissions::ALL));

  // A value containing a bit outside ALL is invalid.
  assert(!Permissions_is_valid(static_cast<Permissions>(8U)));

  assert(!Permissions_is_valid(static_cast<Permissions>(0x80U)));
}

// ============================================================================
// Smaller underlying type
// ============================================================================

static void test_small_underlying_flags()
{
  assert(static_cast<unsigned char>(Features::ALL) == static_cast<unsigned char>(0x07));

  const auto value = Features::A | Features::C;

  assert(static_cast<unsigned char>(value) == 0x05);

  assert(Features_has_flag(value, Features::A));
  assert(!Features_has_flag(value, Features::B));
  assert(Features_has_flag(value, Features::C));

  assert(Features_to_str(value) == "A|C");

  assert(Features_from_str("A|C") == value);

  assert(Features_is_valid(value));
}

// ============================================================================
// Main
// ============================================================================

int main()
{
  test_enum_basic();
  test_enum_metadata();
  test_enum_to_string();
  test_enum_from_string();
  test_enum_validation();

  test_flags_underlying();
  test_flags_all();
  test_flags_operators();
  test_flags_assignment_operators();
  test_flags_has_flag();
  test_flags_to_string();
  test_flags_to_vec_string();
  test_flags_from_string();
  test_flags_validation();

  test_small_underlying_flags();

  return 0;
}