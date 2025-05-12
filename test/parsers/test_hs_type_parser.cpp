#include <gtest/gtest.h>

#include <komaru/parsers/hs_type_parser.hpp>

#include <format>

using namespace komaru::parsers;
using namespace komaru::lang;

namespace {

Type MustParse(const std::string& raw) {
    auto parser = HsTypeParser(raw);
    auto result = parser.Parse();
    if (!result.has_value()) {
        throw std::runtime_error(
            std::format("Failed to parse type: {}\nReason: {}", raw, result.error().Error()));
    }
    return result.value();
}

std::string MustParseStr(const std::string& raw) {
    return MustParse(raw).ToString();
}

}  // namespace

TEST(HsTypeParser, Simple) {
    ASSERT_EQ(MustParseStr("a"), "a");
    ASSERT_EQ(MustParseStr("Int"), "Int");
    ASSERT_EQ(MustParseStr("(Int, Int)"), "Int x Int");
    ASSERT_EQ(MustParseStr("IO a"), "IO a");
    ASSERT_EQ(MustParseStr("IO ()"), "IO S");
    ASSERT_EQ(MustParseStr("[Int]"), "[Int]");
    ASSERT_EQ(MustParseStr("(IO Int, IO Int)"), "(IO Int) x (IO Int)");
}

TEST(HsTypeParser, Libs) {
    ASSERT_EQ(MustParseStr("GLfloat"), "GLfloat");
    ASSERT_EQ(MustParseStr("StateVar [GLUT.DisplayMode]"), "StateVar [GLUT.DisplayMode]");
    ASSERT_EQ(MustParseStr("IO GLUT.Window"), "IO GLUT.Window");
    ASSERT_EQ(MustParseStr("SettableStateVar (Maybe GLUT.ReshapeCallback)"),
              "SettableStateVar (Maybe GLUT.ReshapeCallback)");
}
