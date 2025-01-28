#pragma once
#include <lang/category.hpp>
#include <translate/program.hpp>

namespace komaru::translate {

struct ITranslator {
    virtual ~ITranslator() = default;

    // TODO: returning unique_ptr sucks
    virtual std::unique_ptr<IProgram> Translate(lang::Category& category) = 0;
};

}
