/**
 * @file logger.hpp
 * @brief 日志
 */

#pragma once
#ifndef EMDEVIF_LOGGER_HPP
#define EMDEVIF_LOGGER_HPP

#include "emdevif/logger/config.hpp"
#include "emdevif/logger/config_values.hpp"

#if (EMDEVIF_LOGGER_MODE == 0)
#include "emdevif/logger/sync.hpp"
#elif (EMDEVIF_LOGGER_MODE == 1)
#include "emdevif/logger/async.hpp"
#elif (EMDEVIF_LOGGER_MODE == 2)
#include "emdevif/logger/external_impl.hpp"
#endif

#endif  // !EMDEVIF_LOGGER_HPP
