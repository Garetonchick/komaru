#include <print>

#include <komaru/translate/cpp/cpp_translator.hpp>
#include <test/translate/programs.hpp>
#include <catlib/cpp/catlib.hpp>

void SaveCode(const komaru::lang::CatProgram& cat_program) {
    komaru::translate::cpp::CppTranslator translator("../../catlib/cpp");

    auto cpp_program = translator.Translate(cat_program);

    if (std::FILE* stream{std::fopen("gen.cpp", "w")})
    {
        std::println(stream, "{}", cpp_program.value()->GetSourceCode());
        std::fclose(stream);
        std::println("Generated gen.cpp");
    }
}

void PlayWithIO() {
    IO<int32_t> read1 = Read<int32_t>();
    IO<int32_t> read2 = read1;
    IO<int32_t> io_sum = LiftM2(Plus, read1, read2);
    IO<std::monostate> io_print = Bind(io_sum, Print);

    io_print.Run();
}

int main() {
    // Choose your fighter (or code whatever you wish yourself)
    // SaveCode(komaru::test::MakeFibProgram(5));
    SaveCode(komaru::test::MakeIO101Program());
    // PlayWithIO();
}
