#include "cpp_translator.hpp"

#include <translate/cpp_program.hpp>

namespace komaru::translate {

std::unique_ptr<IProgram> CppTranslator::Translate(lang::Category&) {
    return std::make_unique<CppProgram>(R"(
        #include <iostream>

        int main() {
            std::cout << "Hello CppTranslator" << std::endl;
        }
    )");
}

}
