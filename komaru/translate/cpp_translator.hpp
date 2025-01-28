#pragma once
#include <translate/translator.hpp>

namespace komaru::translate {

class CppTranslator : public ITranslator {
public:
    std::unique_ptr<IProgram> Translate(lang::Category& category) override;
};

}
