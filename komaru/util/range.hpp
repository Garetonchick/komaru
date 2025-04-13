#pragma once
#include <vector>
#include <ranges>

namespace komaru::util {

template <typename ContainerT>
using Range = std::decay_t<decltype(std::ranges::subrange(std::declval<ContainerT>().begin(),
                                                          std::declval<ContainerT>().end()))>;

template <typename T>
using VectorRange = Range<std::vector<T>>;

}  // namespace komaru::util
