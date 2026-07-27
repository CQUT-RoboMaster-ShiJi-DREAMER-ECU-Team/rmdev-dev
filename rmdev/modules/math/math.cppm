/**
 * @file math.cppm
 * @brief 数学相关函数
 */

module;

#ifdef RMDEV_USE_CMSIS_DSP
#include "arm_math.h"
#endif

#include "rmdev/math.hpp"

export module rmdev.math;

export namespace rmdev::num {
using ::rmdev::num::e;
using ::rmdev::num::invPi;
using ::rmdev::num::invPi_2;
using ::rmdev::num::invSqrt2;
using ::rmdev::num::invSqrtPi_2;
using ::rmdev::num::lge;
using ::rmdev::num::ln10;
using ::rmdev::num::ln2;
using ::rmdev::num::log2_e;
using ::rmdev::num::pi;
using ::rmdev::num::pi_inv2;
using ::rmdev::num::pi_inv4;
using ::rmdev::num::sqrt2;
}  // namespace rmdev::num
export namespace rmdev {
using ::rmdev::angleToRad;
using ::rmdev::bitUintMax;
using ::rmdev::cos;
using ::rmdev::integerBits;
using ::rmdev::limitInRange;
using ::rmdev::limitMinMax;
using ::rmdev::radToAngle;
using ::rmdev::sgn;
using ::rmdev::sin;
using ::rmdev::sin_cos;
using ::rmdev::weakEqu;
}  // namespace rmdev
