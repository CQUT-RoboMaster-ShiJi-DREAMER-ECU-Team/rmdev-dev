#include <string_view>

#ifdef EMDEVIF_USE_MODULES
import emdevif.peripheral.peripheral_handle_map;
import emdevif.peripheral.model.gpio;
import emdevif.peripheral.model.serial;
#else
#include "emdevif/peripheral/peripheral_handle_map.hpp"
#include "emdevif/peripheral/model/gpio.hpp"
#include "emdevif/peripheral/model/serial.hpp"
#endif

namespace {

emdevif::GpioModel::Instance gpio_inst{};
emdevif::SerialModel::Instance serial_inst{};

}  // namespace

namespace emdevif::user_impl::peripheral_handle_map {

void* findHandle(std::string_view name) noexcept
{
    if (name == "test_gpio") {
        return &gpio_inst;
    }
    if (name == "test_serial") {
        return &serial_inst;
    }
    return nullptr;
}

}  // namespace emdevif::user_impl::peripheral_handle_map
