#pragma once
#include <komaru/lang/type.hpp>
#include <komaru/lang/morphism.hpp>
#include <komaru/lang/pattern.hpp>
#include <komaru/lang/guard.hpp>
#include <komaru/util/non_copyable.hpp>

namespace komaru::lang {

class CatProgramBuilder;

// Internal program's representation in the Komaru language.
// It is a graph where each node has one (or none) input pin and
// several (possibly zero) output pins. Each node contains a type associated with it.
// Each of the output pins has either a pattern or guard (boolean expression)
// associated with it, which allows programmer to manipulate the control flow.
// A pair of input pin and output pin from different nodes can be connected by arrow.
// Each arrow contains exactly one morphism associated with it.
// An output pin can have several arrows coming from it which will create copies
// of the same value before passing them to arrow morhpisms. That way a value can
// be reused multiple times. An input pin can have several arrows pointing to it.
// In this case arrows contain special $i morphisms where i is a an unsigned integer
// and they form a tuple type in the target node by placing values on their repsective
// positions determined by $i syntax.
class CatProgram {
    friend class komaru::lang::CatProgramBuilder;

public:
    class Node;
    class OutPin;
    class Arrow;

    // TODO: destroy unneccessary friendships
    class Arrow : public util::NonCopyable {
        friend class CatProgram;
        friend class OutPin;
        friend class komaru::lang::CatProgramBuilder;

    public:
        const Node& TargetNode() const;
        const OutPin& SourcePin() const;
        const MorphismPtr& GetMorphism() const;

    private:
        Arrow(OutPin& source_pin, Node& target_node, MorphismPtr morphism);

    private:
        OutPin& source_pin_;
        Node& target_node_;
        MorphismPtr morphism_;
    };

    class OutPin : public util::NonCopyable {
        friend class CatProgram;
        friend class ::komaru::lang::CatProgramBuilder;

    public:
        using Brancher = std::variant<Pattern, Guard>;

    public:
        const Node& GetNode() const;
        const Brancher& GetBrancher() const;
        const std::deque<Arrow>& Arrows() const;

        void SetPattern(Pattern pattern);
        void SetGuard(Guard guard);

    private:
        explicit OutPin(Node& node);

        Arrow& AddArrow(Node& target_node, MorphismPtr morphism);

    private:
        Node& node_;  // associated node
        Brancher brancher_{Pattern::Any()};
        std::deque<Arrow> arrows_;
    };

    class Node : public util::NonCopyable {
        friend class CatProgram;
        friend class komaru::lang::CatProgramBuilder;

    public:
        Type GetType() const;
        const std::vector<Arrow*>& IncomingArrows() const;
        const std::deque<OutPin>& OutPins() const;
        const std::string& GetName() const;

        OutPin& AddOutPin();
        OutPin& AddOutPin(Pattern pattern);
        OutPin& AddOutPin(Guard guard);
        Node& SetName(std::string name);

    private:
        explicit Node(Type type, std::string name = "");

    private:
        Type type_;
        std::string name_;
        std::vector<Arrow*> incoming_arrows_;
        std::deque<OutPin> out_pins_;
    };

public:
    const std::deque<Node>& GetNodes() const;

private:
    CatProgram() = default;
    // preserves node addresses
    std::deque<Node> nodes_;
};

class CatProgramBuilder {
    using Node = CatProgram::Node;
    using OutPin = CatProgram::OutPin;

public:
    CatProgramBuilder() = default;

    Node& NewNode(Type type, std::string name = "");
    std::pair<Node&, OutPin&> NewNodeWithPin(Type type, std::string name = "");
    CatProgramBuilder& Connect(OutPin& out_pin, Node& node, MorphismPtr morphism);

    CatProgram Extract();

private:
    CatProgram program_;
};

void DebugCatProgram(const CatProgram& program);

}  // namespace komaru::lang
