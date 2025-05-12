#include <gtest/gtest.h>

#include <komaru/parsers/morphism_parser.hpp>

#include <format>

using namespace komaru::parsers;
using namespace komaru::lang;
using namespace komaru::translate;

namespace {

MorphismPtr MustParse(hs::HaskellSymbolsRegistry& symbols_registry, const std::string& raw) {
    auto parser = MorphismParser(raw, symbols_registry);
    auto result = parser.Parse();
    if (!result.has_value()) {
        throw std::runtime_error(
            std::format("Failed to parse morphism: {}\nReason: {}", raw, result.error().Error()));
    }
    return result.value();
}

std::string MustParseStr(hs::HaskellSymbolsRegistry& symbols_registry, const std::string& raw) {
    return MustParse(symbols_registry, raw)->ToString();
}

}  // namespace

TEST(MorphismParser, Simple) {
    hs::HaskellSymbolsRegistry symbols_registry({}, {hs::HaskellImport{
                                                        .module_name = "Control.Monad",
                                                        .ref_name = "",
                                                        .symbols = {},
                                                    }});

    ASSERT_EQ(MustParseStr(symbols_registry, "$"), "$");
    ASSERT_EQ(MustParseStr(symbols_registry, "$0"), "$0");
    ASSERT_EQ(MustParseStr(symbols_registry, "$1"), "$1");
    ASSERT_EQ(MustParseStr(symbols_registry, "readLn"), "readLn");
    ASSERT_EQ(MustParseStr(symbols_registry, "liftM2 (+)"), "liftM2 (+)");
    ASSERT_EQ(MustParseStr(symbols_registry, ">>= print"), ">>= print");
    ASSERT_EQ(MustParseStr(symbols_registry, "42"), "42");
}
