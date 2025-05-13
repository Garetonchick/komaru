#include <gtest/gtest.h>

#include <komaru/parsers/pattern_parser.hpp>

#include <format>

using namespace komaru::parsers;
using namespace komaru::lang;
using namespace komaru::translate;

namespace {

Pattern MustParse(hs::HaskellSymbolsRegistry& symbols_registry, const std::string& raw) {
    auto parser = PatternParser(raw, symbols_registry);
    auto result = parser.Parse();
    if (!result.has_value()) {
        throw std::runtime_error(
            std::format("Failed to parse morphism: {}\nReason: {}", raw, result.error().Error()));
    }
    return result.value();
}

std::string MustParseStr(hs::HaskellSymbolsRegistry& symbols_registry, const std::string& raw) {
    return MustParse(symbols_registry, raw).ToString();
}

}  // namespace

TEST(PatternParser, Simple) {
    hs::HaskellSymbolsRegistry symbols_registry({}, {});

    ASSERT_EQ(MustParseStr(symbols_registry, "kek"), "kek");
    ASSERT_EQ(MustParseStr(symbols_registry, "*"), "*");
    ASSERT_EQ(MustParseStr(symbols_registry, "True"), "True");
    ASSERT_EQ(MustParseStr(symbols_registry, "Just a"), "Just a");
    ASSERT_EQ(MustParseStr(symbols_registry, "Just 'm'"), "Just 'm'");
    ASSERT_EQ(MustParseStr(symbols_registry, "(a, b)"), "(a, b)");
    ASSERT_EQ(MustParseStr(symbols_registry, "(a, b, c)"), "(a, b, c)");
    ASSERT_EQ(MustParseStr(symbols_registry, "(42, a, 'm')"), "(42, a, 'm')");
    ASSERT_EQ(MustParseStr(symbols_registry, "(42, *, 'm')"), "(42, *, 'm')");
}

TEST(PatternParser, Lib) {
    hs::HaskellSymbolsRegistry symbols_registry({"GLUT"}, {hs::HaskellImport{
                                                              .module_name = "Graphics.UI.GLUT",
                                                              .ref_name = "GLUT",
                                                              .symbols = {},
                                                          }});

    ASSERT_EQ(MustParseStr(symbols_registry, "(GLUT.Char 'q', GLUT.Down)"),
              "(GLUT.Char 'q', GLUT.Down)");
}
