#pragma once
#include <util/non_copyable.hpp>
#include <util/non_movable.hpp>

namespace komaru::util {

struct NonCopyableNonMovable : public NonCopyable,
                               public NonMovable {};

}  // namespace komaru::util
