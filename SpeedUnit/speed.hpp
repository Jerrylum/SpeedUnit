#include "preset.h"

#ifndef _CMAPI_SPEED_INCLUDE_
#define _CMAPI_SPEED_INCLUDE_

/**
 * 用於控制電機的驅動方法
 */
typedef enum advanced_unit_type_e {
  SPEED_POINT = 0,
  MILLIVOLTS = 1,
  RPM = 2
} advanced_unit_type_e_t;

/**
 * 控制階段的類型
 */
typedef enum speed_control_phase_type_e {
  TIME_PHASE = 0,
  VALUE_PHASE = 1
} speed_control_phase_type_e_t;

/**
 * 齒輪組最大支持轉速
 */
const short GEARSET_RED_MAX_RPM = 100, GEARSET_GREEN_MAX_RPM = 200,
            GEARSET_BLUE_MAX_RPM = 600;

/**
 * Speed variable bits:
 *
 * 32768 12384 8192 4096 | 2048 1024 512 256 | 128 64 32 16 | 8 4 2 1
 *       <----------------------------------------------------------> millivolts
 *                              <-----------------------------------> rpm
 *                                             <--------------------> speed
 * point
 *
 * 1011 0??? ???? ???? rotate per minute
 * <--> must be 1011(+) or 0100(-)
 *
 * 01?? ???? ???? ???? millivolts <0  min = *101000100100000 = -12000
 * <--> only 0101 or 0110  or 0111
 *
 * 10?? ???? ???? ???? millivolts >0  max = *010111011100000 = +12000
 * <--> only 1000 or 1001  or 1010
 *
 * 0000 0000 ???? ???? speed point
 * <> must be all 0 or all 1
 *
 * 1010111011100000+ mv
 * 0101000100011111
 * 0101000100100000-
 *
 * 1011001011111111+ rpm
 * 0100110100000000
 * 0100110100000001-
 *
 * 0000000001111111+ point
 * 1111111110000000
 * 1111111110000001-
 */

constexpr double millivoltage(1);             // base unit
constexpr double speedpoint = 12000.0 / 127;  // default unit
constexpr double voltage = 1000;
constexpr double rpm(1);  // base unit
constexpr double rps = 60;

class advanced_unit {
 private:
  // 速度驅動類型
  advanced_unit_type_e type_;

  // 速度點計算答案
  int8_t point_ = 0;

  // 毫伏計算答案
  short mV_ = 0;

  // 每分鐘轉速計算答案
  short rpm_ = 0;

  // 正被速度控制的馬達的齒輪組
  pros::motor_gearset_e_t gearset_ = pros::E_MOTOR_GEARSET_INVALID;

  void decode_(const short speed);

 public:
  /**
   * 構造函數
   *
   * \param encoded
   *        已編碼的速度變量
   */
  advanced_unit(const short encoded);

  /**
   * 構造函數
   *
   * \param encoded
   *        已編碼的速度變量
   * \param gearset
   *        正被速度控制的馬達的齒輪組
   */
  advanced_unit(const short encoded, const pros::motor_gearset_e_t gearset);
   
  /**
   * 構造函數
   *
   * \param speed
   *        解碼的速度變量
   * \param type
   *        速度驅動類型
   */
  advanced_unit(const short speed, advanced_unit_type_e type);


  /**
   * 取得解碼的毫伏速度
   *
   * \return 毫伏 -12000 ~ +12000
   */
  short to_millivolts();

  /**
   * 取得解碼的速度點
   *
   * \return 速度點 -127 ~ +127
   */
  short to_speedpoint();

  /**
   * 取得解碼的每分鐘轉速
   *
   * \return 每分鐘轉速速度 -600 ~ +600
   */
  short to_rpm();

  short to_raw();

  /**
   * 取得已編碼的速度變量
   *
   * \return 已編碼的速度變量
   */
  short to_encoded();

  /**
   * 取得速度驅動類型
   *
   * \return 速度驅動類型
   */
  advanced_unit_type_e get_type();

  /**
   * 設置正被速度控制的馬達的齒輪組, 可以用來計算速度變量
   * 比如你輸入了每分鐘轉速, 計算機可以通過齒輪組來計算電壓驅動速度.
   *
   * \param gearset
   *        正被速度控制的馬達的齒輪組
   */
  void set_gearset(const pros::motor_gearset_e_t gearset);

  /**
   * 取得齒輪組類型
   *
   * \return 齒輪組
   */
  pros::motor_gearset_e_t get_gearset();

  constexpr static short encode_millivoltage(short raw);

  constexpr static short encode_rpm(short raw);
};

constexpr short operator"" _pt(long double x) {
  if (x > 127) x = 127;
  return advanced_unit::encode_millivoltage(x * speedpoint);
}
constexpr short operator"" _mV(long double x) {
  return advanced_unit::encode_millivoltage(x);
}
constexpr short operator"" _v(long double x) {
  if (x > 12) x = 12;
  return advanced_unit::encode_millivoltage(x * voltage);
}
constexpr short operator"" _rpm(long double x) {
  if (x > 600) x = 600;
  return advanced_unit::encode_rpm(x);
}
constexpr short operator"" _rps(long double x) {
  if (x > 10) x = 10;
  return advanced_unit::encode_rpm(x * rps);
}

constexpr short operator"" _pt(unsigned long long int x) {
  if (x > 127) x = 127;
  return advanced_unit::encode_millivoltage(x * speedpoint);
}
constexpr short operator"" _mV(unsigned long long int x) {
  return advanced_unit::encode_millivoltage(x);
}
constexpr short operator"" _v(unsigned long long int x) {
  if (x > 12) x = 12;
  return advanced_unit::encode_millivoltage(x * voltage);
}
constexpr short operator"" _rpm(unsigned long long int x) {
  if (x > 600) x = 600;
  return advanced_unit::encode_rpm(x);
}
constexpr short operator"" _rps(unsigned long long int x) {
  if (x > 10) x = 10;
  return advanced_unit::encode_rpm(x * rps);
}

class speed_phase {
 public:
  speed_control_phase_type_e_t type;
  long time = 0;     // ms
  double value = 0;  // cm
  short speed = 0;

  speed_phase* previous = nullptr;

  speed_phase(long ms, short speed)
      : type(TIME_PHASE), time(ms), speed(speed) {}
  speed_phase(double val, short speed)
      : type(VALUE_PHASE), value(val), speed(speed) {}
  void add(speed_phase* phase);
  speed_phase* clone();

  short get_output(long time, double value, long time_end,
                   double value_end);  // return speed

  speed_phase(const speed_phase& other);
  ~speed_phase();
};

class time_target {
 public:
  long ms = 0;
  time_target(long x) : ms(x) {}

  speed_phase operator[](short int speed);
};

class value_target {
 public:
  double val = 0;
  value_target(double x) : val(x) {}

  speed_phase operator[](short int speed);
};

time_target operator"" _ms(unsigned long long int x);

value_target operator"" _val(long double x);

value_target operator"" _val(unsigned long long int x);

speed_phase operator>>(speed_phase a, speed_phase b);

speed_phase operator-(speed_phase a);

#endif