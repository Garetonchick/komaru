#pragma once
#include <komaru/util/non_copyable.hpp>
#include <komaru/util/non_movable.hpp>

namespace komaru::util {

struct NonCopyableNonMovable : public NonCopyable,
                               public NonMovable {};

}  // namespace komaru::util
