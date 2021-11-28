#pragma once

#include <bitset>
#include <iostream>
#include <stack>
#pragma warning(disable : 4996)

#define MOTOR_GEARSET_36 pros::E_MOTOR_GEARSET_36
#define MOTOR_GEARSET_18 pros::E_MOTOR_GEARSET_18
#define MOTOR_GEARSET_06 pros::E_MOTOR_GEARSET_06
#define MOTOR_GEARSET_6 pros::E_MOTOR_GEARSET_06

namespace pros {
/**
 * Indicates the current internal gear ratio of a motor.
 */
typedef enum motor_gearset_e {
  E_MOTOR_GEARSET_36 = 0,  // 36:1, 100 RPM, Red gear set
  E_MOTOR_GEARSET_18 = 1,  // 18:1, 200 RPM, Green gear set
  E_MOTOR_GEARSET_06 = 2,  // 6:1, 600 RPM, Blue gear set
  E_MOTOR_GEARSET_INVALID = INT32_MAX
} motor_gearset_e_t;
};  // namespace pros

/**
 * 此namespace定義了不同齒輪組名稱的簡寫
 * 可使用於馬達設定
 */
namespace GEARSET {
const pros::motor_gearset_e_t
    RED = MOTOR_GEARSET_36,    // 36:1, 100 RPM, Red gear set
    GREEN = MOTOR_GEARSET_18,  // 18:1, 200 RPM, Green gear set
    BLUE = MOTOR_GEARSET_6;    // 6:1,  600 RPM, Blue gear set
};

#define abs_const(x) ((x > 0) ? x : -x)

#define sgn_nonzero(x) (x > 0 ? 1 : -1)

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}
