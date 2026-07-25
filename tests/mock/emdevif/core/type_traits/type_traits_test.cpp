#include "host_test_pch.hpp"

#include <array>
#include <tuple>
#include <utility>

#if EMDEVIF_USE_MODULES
import emdevif.core.type_traits;
#else
#include "emdevif/core/type_traits.hpp"
#endif

using namespace emdevif;

namespace {

constexpr auto a = 0;
auto b = 0;
static_assert(emdevif::is_consteval([] { return a; }));
static_assert(!emdevif::is_consteval([] { return b; }));

}  // namespace
