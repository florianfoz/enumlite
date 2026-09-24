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


// ============================================================================
// Test big enum
// ============================================================================

DEFINE_ENUM(BigEnum, int, //
            ITEM_0, 0, ITEM_1, 1, ITEM_2, 2, ITEM_3, 3, ITEM_4, 4, ITEM_5, 5, ITEM_6, 6, ITEM_7, 7, ITEM_8, 8, ITEM_9,
            9, ITEM_10, 10, ITEM_11, 11, ITEM_12, 12, ITEM_13, 13, ITEM_14, 14, ITEM_15, 15, ITEM_16, 16, ITEM_17, 17,
            ITEM_18, 18, ITEM_19, 19, ITEM_20, 20, ITEM_21, 21, ITEM_22, 22, ITEM_23, 23, ITEM_24, 24, ITEM_25, 25,
            ITEM_26, 26, ITEM_27, 27, ITEM_28, 28, ITEM_29, 29, ITEM_30, 30, ITEM_31, 31, ITEM_32, 32, ITEM_33, 33,
            ITEM_34, 34, ITEM_35, 35, ITEM_36, 36, ITEM_37, 37, ITEM_38, 38, ITEM_39, 39, ITEM_40, 40, ITEM_41, 41,
            ITEM_42, 42, ITEM_43, 43, ITEM_44, 44, ITEM_45, 45, ITEM_46, 46, ITEM_47, 47, ITEM_48, 48, ITEM_49, 49,
            ITEM_50, 50, ITEM_51, 51, ITEM_52, 52, ITEM_53, 53, ITEM_54, 54, ITEM_55, 55, ITEM_56, 56, ITEM_57, 57,
            ITEM_58, 58, ITEM_59, 59, ITEM_60, 60, ITEM_61, 61, ITEM_62, 62, ITEM_63, 63, ITEM_64, 64, ITEM_65, 65,
            ITEM_66, 66, ITEM_67, 67, ITEM_68, 68, ITEM_69, 69, ITEM_70, 70, ITEM_71, 71, ITEM_72, 72, ITEM_73, 73,
            ITEM_74, 74, ITEM_75, 75, ITEM_76, 76, ITEM_77, 77, ITEM_78, 78, ITEM_79, 79, ITEM_80, 80, ITEM_81, 81,
            ITEM_82, 82, ITEM_83, 83, ITEM_84, 84, ITEM_85, 85, ITEM_86, 86, ITEM_87, 87, ITEM_88, 88, ITEM_89, 89,
            ITEM_90, 90, ITEM_91, 91, ITEM_92, 92, ITEM_93, 93, ITEM_94, 94, ITEM_95, 95, ITEM_96, 96, ITEM_97, 97,
            ITEM_98, 98, ITEM_99, 99, ITEM_100, 100, ITEM_101, 101, ITEM_102, 102, ITEM_103, 103, ITEM_104, 104,
            ITEM_105, 105, ITEM_106, 106, ITEM_107, 107, ITEM_108, 108, ITEM_109, 109, ITEM_110, 110, ITEM_111, 111,
            ITEM_112, 112, ITEM_113, 113, ITEM_114, 114, ITEM_115, 115, ITEM_116, 116, ITEM_117, 117, ITEM_118, 118,
            ITEM_119, 119, ITEM_120, 120, ITEM_121, 121, ITEM_122, 122, ITEM_123, 123, ITEM_124, 124, ITEM_125, 125,
            ITEM_126, 126, ITEM_127, 127, ITEM_128, 128, ITEM_129, 129, ITEM_130, 130, ITEM_131, 131, ITEM_132, 132,
            ITEM_133, 133, ITEM_134, 134, ITEM_135, 135, ITEM_136, 136, ITEM_137, 137, ITEM_138, 138, ITEM_139, 139,
            ITEM_140, 140, ITEM_141, 141, ITEM_142, 142, ITEM_143, 143, ITEM_144, 144, ITEM_145, 145, ITEM_146, 146,
            ITEM_147, 147, ITEM_148, 148, ITEM_149, 149, ITEM_150, 150, ITEM_151, 151, ITEM_152, 152, ITEM_153, 153,
            ITEM_154, 154, ITEM_155, 155, ITEM_156, 156, ITEM_157, 157, ITEM_158, 158, ITEM_159, 159, ITEM_160, 160,
            ITEM_161, 161, ITEM_162, 162, ITEM_163, 163, ITEM_164, 164, ITEM_165, 165, ITEM_166, 166, ITEM_167, 167,
            ITEM_168, 168, ITEM_169, 169, ITEM_170, 170, ITEM_171, 171, ITEM_172, 172, ITEM_173, 173, ITEM_174, 174,
            ITEM_175, 175, ITEM_176, 176, ITEM_177, 177, ITEM_178, 178, ITEM_179, 179, ITEM_180, 180, ITEM_181, 181,
            ITEM_182, 182, ITEM_183, 183, ITEM_184, 184, ITEM_185, 185, ITEM_186, 186, ITEM_187, 187, ITEM_188, 188,
            ITEM_189, 189, ITEM_190, 190, ITEM_191, 191, ITEM_192, 192, ITEM_193, 193, ITEM_194, 194, ITEM_195, 195,
            ITEM_196, 196, ITEM_197, 197, ITEM_198, 198, ITEM_199, 199, ITEM_200, 200, ITEM_201, 201, ITEM_202, 202,
            ITEM_203, 203, ITEM_204, 204, ITEM_205, 205, ITEM_206, 206, ITEM_207, 207, ITEM_208, 208, ITEM_209, 209,
            ITEM_210, 210, ITEM_211, 211, ITEM_212, 212, ITEM_213, 213, ITEM_214, 214, ITEM_215, 215, ITEM_216, 216,
            ITEM_217, 217, ITEM_218, 218, ITEM_219, 219, ITEM_220, 220, ITEM_221, 221, ITEM_222, 222, ITEM_223, 223,
            ITEM_224, 224, ITEM_225, 225, ITEM_226, 226, ITEM_227, 227, ITEM_228, 228, ITEM_229, 229, ITEM_230, 230,
            ITEM_231, 231, ITEM_232, 232, ITEM_233, 233, ITEM_234, 234, ITEM_235, 235, ITEM_236, 236, ITEM_237, 237,
            ITEM_238, 238, ITEM_239, 239, ITEM_240, 240, ITEM_241, 241, ITEM_242, 242, ITEM_243, 243, ITEM_244, 244,
            ITEM_245, 245, ITEM_246, 246, ITEM_247, 247, ITEM_248, 248, ITEM_249, 249, ITEM_250, 250, ITEM_251, 251,
            ITEM_252, 252, ITEM_253, 253, ITEM_254, 254)