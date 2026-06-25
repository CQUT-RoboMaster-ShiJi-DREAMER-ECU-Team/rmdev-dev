#ifdef EMDEVIF_USE_MODULES
import emdevif.timeline;
#else
    #include "emdevif/timeline.hpp"
#endif

#include <cstdint>

namespace emdevif::user_impl::timeline {

uint64_t getMicroseconds() noexcept
{
    static uint64_t counter = 0;
    return ++counter;
}

}  // namespace emdevif::user_impl::timeline
