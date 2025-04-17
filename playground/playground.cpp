#include <print>

#include <komaru/translate/cpp/cpp_translator.hpp>
#include <test/translate/programs.hpp>

int main() {
    auto cat_program = komaru::test::MakeFibProgram(5);

    komaru::translate::cpp::CppTranslator translator;

    auto cpp_program = translator.Translate(cat_program);

    if (std::FILE* stream{std::fopen("gen.cpp", "w")})
    {
        std::println(stream, "{}", cpp_program.value()->GetSourceCode());
        std::fclose(stream);
        std::println("Generated gen.cpp");
    }
}
