#include <gtest/gtest.h>

#include <komaru/parsers/cat_type_parser.hpp>

#include <format>

using namespace komaru::parsers;
using namespace komaru::lang;
using namespace komaru::translate;

namespace {

Type MustParse(hs::HaskellSymbolsRegistry& symbols_registry, const std::string& raw) {
    auto parser = CatTypeParser(raw, symbols_registry);
    auto result = parser.Parse();
    if (!result.has_value()) {
        throw std::runtime_error(
            std::format("Failed to parse type: {}\nReason: {}", raw, result.error().Error()));
    }
    return result.value();
}

std::string MustParseStr(hs::HaskellSymbolsRegistry& symbols_registry, const std::string& raw) {
    return MustParse(symbols_registry, raw).ToString();
}

}  // namespace

TEST(CatTypeParser, Simple) {
    hs::HaskellSymbolsRegistry symbols_registry({}, {});

    ASSERT_EQ(MustParseStr(symbols_registry, "a"), "a");
    ASSERT_EQ(MustParseStr(symbols_registry, "Int"), "Int");
    ASSERT_EQ(MustParseStr(symbols_registry, "Int x Int"), "Int x Int");
    ASSERT_EQ(MustParseStr(symbols_registry, "IO S"), "IO S");
    ASSERT_EQ(MustParseStr(symbols_registry, "[Int]"), "[Int]");
    ASSERT_EQ(MustParseStr(symbols_registry, "IO Int x IO Int"), "(IO Int) x (IO Int)");
}

TEST(CatTypeParser, Libs) {
    hs::HaskellSymbolsRegistry symbols_registry({"GLUT", "OpenGL"},
                                                {hs::HaskellImport{
                                                     .module_name = "Graphics.Rendering.OpenGL",
                                                     .ref_name = "",
                                                     .symbols = {},
                                                 },
                                                 hs::HaskellImport{
                                                     .module_name = "Graphics.UI.GLUT",
                                                     .ref_name = "GLUT",
                                                     .symbols = {},
                                                 }});

    ASSERT_EQ(MustParseStr(symbols_registry, "GLfloat"), "GLfloat");
    ASSERT_EQ(MustParseStr(symbols_registry, "StateVar [GLUT.DisplayMode]"),
              "StateVar [GLUT.DisplayMode]");
    ASSERT_EQ(MustParseStr(symbols_registry, "IO GLUT.Window"), "IO GLUT.Window");
    ASSERT_EQ(MustParseStr(symbols_registry, "SettableStateVar (Maybe GLUT.ReshapeCallback)"),
              "SettableStateVar (Maybe GLUT.ReshapeCallback)");
}
