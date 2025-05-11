#include "ghci.hpp"

#include <komaru/parsers/hs_type_parser.hpp>

#include <memory>

namespace komaru::translate::hs {

GHCI::GHCI(PrivateDummy, const std::vector<std::string>& packages)
    : util::CLIProgramManipulator(MakeStartCommand(packages), "ghci> ") {
}

std::optional<GHCIPtr> GHCI::CreateSession(const std::vector<std::string>& packages,
                                           const std::vector<HaskellImport>& imports) {
    auto ghci = std::make_unique<GHCI>(PrivateDummy{}, packages);

    for (const auto& import : imports) {
        std::string resp = ghci->Interact(import.ToString() + "\n");
        if (HasError(resp)) {
            return std::nullopt;
        }
    }

    return ghci;
}

std::optional<lang::Type> GHCI::GetType(const std::string& name) {
    std::string resp = Interact(":t " + name + "\n");
    if (HasError(resp)) {
        return std::nullopt;
    }

    auto pos = resp.find("=>");
    if (pos != std::string::npos) {
        auto maybe_type = parsers::HsTypeParser(resp.substr(pos + 2)).Parse();
        if (!maybe_type) {
            return std::nullopt;
        }
        return maybe_type.value();
    }

    pos = resp.find("::");
    if (pos != std::string::npos) {
        auto maybe_type = parsers::HsTypeParser(resp.substr(pos + 2)).Parse();
        if (!maybe_type) {
            return std::nullopt;
        }
        return maybe_type.value();
    }

    return std::nullopt;
}

std::optional<lang::TypeConstructor> GHCI::GetTypeConstructor(const std::string& name) {
    std::string resp = Interact(":kind " + name + "\n");
    if (HasError(resp)) {
        return std::nullopt;
    }

    auto pos = resp.find("::");

    if (pos == std::string::npos) {
        return std::nullopt;
    }

    return ParseTypeConstructor(name, resp.substr(pos + 2));
}

std::vector<std::string> GHCI::MakeStartCommand(const std::vector<std::string>& packages) {
    std::vector<std::string> command;
    command.push_back("ghci");
    for (const auto& package : packages) {
        command.push_back("-package");
        command.push_back(package);
    }
    return command;
}

bool GHCI::HasError(const std::string& resp) {
    return resp.find("error:") != std::string::npos;
}

std::optional<lang::TypeConstructor> GHCI::ParseTypeConstructor(const std::string& name,
                                                                const std::string& raw) {
    bool expect_arrow = false;
    size_t n_stars = 0;

    for (size_t i = 0; i < raw.size();) {
        if (std::isspace(raw[i])) {
            ++i;
            continue;
        }

        if ((raw[i] == '*') != !expect_arrow) {
            return std::nullopt;
        }

        if (raw[i] == '*') {
            ++i;
            ++n_stars;
        } else if (raw[i] == '-') {
            if (i + 1 >= raw.size() || raw[i + 1] != '>') {
                return std::nullopt;
            }
            i += 2;
        } else {
            return std::nullopt;
        }

        expect_arrow = !expect_arrow;
    }

    if (!expect_arrow || n_stars == 0) {
        return std::nullopt;
    }

    return lang::TypeConstructor(name, n_stars - 1);
}

}  // namespace komaru::translate::hs
