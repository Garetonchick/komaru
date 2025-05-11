#include <gtest/gtest.h>

#include <komaru/translate/haskell/ghci.hpp>
#include <komaru/lang/morphism.hpp>

using namespace komaru::translate;
using namespace komaru::lang;

TEST(GHCI, SimpleGetType) {
    auto maybe_ghci = hs::GHCI::CreateSession({}, {});

    ASSERT_TRUE(maybe_ghci.has_value());

    auto ghci = std::move(maybe_ghci.value());

    auto id_type = ghci->GetType("id");
    ASSERT_TRUE(id_type.has_value());
    ASSERT_EQ(id_type.value().ToString(), "a -> a");
    ASSERT_EQ(id_type, Morphism::Identity()->GetType());

    auto no_type = ghci->GetType("kek");
    ASSERT_FALSE(no_type.has_value());

    auto expected_zip_type =
        Type::FunctionChain({Type::List(Type::Var("a")), Type::List(Type::Var("b")),
                             Type::List(Type::Tuple({Type::Var("a"), Type::Var("b")}))});

    auto zip_type = ghci->GetType("zip");
    ASSERT_TRUE(zip_type.has_value());
    ASSERT_EQ(zip_type.value().ToString(), "[a] -> [b] -> [a x b]");
    ASSERT_EQ(zip_type, expected_zip_type);
}

TEST(GHCI, Packages) {
    auto maybe_ghci = hs::GHCI::CreateSession({"GLUT"}, {hs::HaskellImport{
                                                            .module_name = "Graphics.UI.GLUT",
                                                            .ref_name = "GLUT",
                                                            .symbols = {},
                                                        }});

    ASSERT_TRUE(maybe_ghci.has_value());

    auto ghci = std::move(maybe_ghci.value());

    Type expected_display_type =
        Type::Parameterized("GLUT.SettableStateVar", {Type::Simple("GLUT.DisplayCallback")});
    auto display_type = ghci->GetType("GLUT.displayCallback");
    ASSERT_TRUE(display_type.has_value());
    ASSERT_EQ(display_type.value().ToString(), "GLUT.SettableStateVar GLUT.DisplayCallback");
    ASSERT_EQ(display_type.value(), expected_display_type);
}

TEST(GHCI, GetTypeConstructor) {
    auto maybe_ghci = hs::GHCI::CreateSession({}, {});

    ASSERT_TRUE(maybe_ghci.has_value());

    auto ghci = std::move(maybe_ghci.value());

    auto maybe_io = ghci->GetTypeConstructor("IO");
    ASSERT_TRUE(maybe_io.has_value());
    ASSERT_EQ(maybe_io.value().ToString(), "IO");
    ASSERT_EQ(maybe_io.value().GetNumParams(), 1);

    auto no_constructor = ghci->GetTypeConstructor("kek");
    ASSERT_FALSE(no_constructor.has_value());

    auto maybe_int = ghci->GetTypeConstructor("Int");
    ASSERT_TRUE(maybe_int.has_value());
    ASSERT_EQ(maybe_int.value().ToString(), "Int");
    ASSERT_EQ(maybe_int.value().GetNumParams(), 0);
}
