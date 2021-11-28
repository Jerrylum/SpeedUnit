#include "preset.h"

#ifndef _CMAPI_SPEED_INCLUDE_
#define _CMAPI_SPEED_INCLUDE_

/**
 * 用於控制電機的速度數值的單位
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
const short GEARSET_RED_MAX_RPM = 100,
            GEARSET_GREEN_MAX_RPM = 200,
            GEARSET_BLUE_MAX_RPM = 600;


constexpr double millivoltage(1);             // base unit
constexpr double speedpoint = 12000.0 / 127;  // default unit
constexpr double voltage = 1000;
constexpr double rpm(1);  // base unit
constexpr double rps = 60;

/**
 * VEX V5 馬達能透過使用速度點(Speed Point)、毫伏(mV)以及每分鐘轉速(RPM)設置速度，其中每
 * 種單位的上下限分別是
 * - 速度點： +-127
 * - 毫伏： +-12000
 * - 每分鐘轉速： +-100(RED)  +-200(GREEN)  +-600(BLUE)
 * 
 * 根據慣例，我們一般使用速度點控制馬達。例如調用指令 `groupLeft.move(80);` 即代表設置速度
 * 為 80sp，而函數宣告則是這樣：
 * void motor_group::move(const short speed)
 *                                    -----
 *                                    ^
 *                                    正常來說，這個函數只能支援輸入速度點，要輸入RPM，
 *                                    便需要再次宣告 .move_rpm() 等類似的函數。
 * 
 * 為了支援輸入其他單位的速度而不需要再次宣告相似的函數，我們需要使用編碼速度(encoded speed)
 * 
 * 
 * 編碼速度可以儲存在16 bit 的數字，速度單位的種類以二進制頭2至4個位作判定。
 *
 * 32768 12384 8192 4096 | 2048 1024 512 256 | 128 64 32 16 | 8 4 2 1
 *       <----------------------------------------------------------> millivolts
 *                              <-----------------------------------> rpm
 *                                             <--------------------> speed point
 *
 * 01?? ???? ???? ???? 毫伏小於0
 * <--> 必定是 0101 或 0110 或 0111
 *
 * 10?? ???? ???? ???? 毫伏大於0
 * <--> 必定是 1000 或 1001 或 1010
 * 
 * 1011 0??? ???? ???? 每分鐘轉速
 * <--> 必定是 1011(+) 或 0100(-)
 *
 * 0000 0000 ???? ???? speed point
 * <> 必定是全部 0 或者全部 1
 * 
 * 毫伏最小值 = [*101000100100000] = -12000
 * 毫伏最大值 = [*010111011100000] = +12000
 *
 * 支援二補數轉換：
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
 * 
 * 有了這個編碼，我們便可以使用不同單位設置馬達的速度：
 * groupLeft.move(80);
 * groupLeft.move(80_pt);
 * groupLeft.move(6000_mV);
 * groupLeft.move(234_rpm);
 * 
 * 
 * 這個類別用來解碼以及編碼速度數值，亦可以推算另一種單位的速度數值。
 * 
 */
class advanced_unit {
 private:
  // 速度單位
  advanced_unit_type_e type_;

  // 速度點計算答案
  int8_t point_ = 0;

  // 毫伏計算答案
  short mV_ = 0;

  // 每分鐘轉速計算答案
  short rpm_ = 0;

  // 正被速度控制的馬達的齒輪組
  pros::motor_gearset_e_t gearset_ = pros::E_MOTOR_GEARSET_INVALID;

  // 解碼已編碼的速度
  void decode_(const short speed);

 public:
  /**
   * 構造函數
   *
   * \param encoded
   *        已編碼的速度
   */
  advanced_unit(const short encoded);

  /**
   * 構造函數
   *
   * \param encoded
   *        已編碼的速度
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
   *        速度單位類型
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

  /**
   * 根據原先儲存的單位類型，回傳所對應的數值
   *
   * \return 原始的數字
   */
  short to_raw();

  /**
   * 取得已編碼的速度
   *
   * \return 已編碼的速度
   */
  short to_encoded();

  /**
   * 取得速度驅動類型
   *
   * \return 速度驅動類型
   */
  advanced_unit_type_e get_type();

  /**
   * 設置正被速度控制的馬達的齒輪組, 可以用來計算其他單位的速度數值
   * 比如你輸入了每分鐘轉速，則可以通過齒輪組來計算電壓驅動速度
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

/**
 * 此類別為一個速度控制階段，一般不會手動創建使用，能夠使用在指令上，以表示指令執行期間，馬達
 * 的輸出和目前時間以及目標值之間的關係，例子： 
 * 
 * forward(80, 0_ms[0] >> 500_ms[60] >> -20_val[60] >> -10_val[30])
 *                                                     -----------
 *                                                     ^
 *                                                     最後一個會被傳入指令
 * 
 * 意思指，向前行走80 cm。由開始30速度，至500毫秒加到60速度，直到剩餘距離20cm開始減速，至剩
 * 餘距離10cm減到30速度，直至完結。
 * 
 * 各個階段之間的速度將會透過計算漸變到另一個階段，支援使用不同種類的速度單位，然而有所限制：
 * 
 * forward(80, 0_val[10] >> 50_val[60_pt] >> 100_val[12_v] >> 150_val[100_rpm])
 *                   |------可能------|------可能------|-------不可能-------|
 *                       原始SP到SP       SP到伏/毫伏        RPM和其他類型
 * 
 * 在這個例子，100cm到149cm會維持12伏輸出，直到到150cm立即改變到100_rpm。
 * 
 */
class speed_phase {
 public:
  speed_control_phase_type_e_t type;
  long time = 0;     // ms
  double value = 0;  // tick
  short speed = 0;   // encoded speed

  speed_phase* previous = nullptr; // linked list structure

  speed_phase(long ms, short speed)
      : type(TIME_PHASE), time(ms), speed(speed) {}
  speed_phase(double val, short speed)
      : type(VALUE_PHASE), value(val), speed(speed) {}

  /**
   * 計算馬達的輸出
   *
   * \param time
   *        現在的時間
   * \param value
   *        現在的進度值
   * \param time_end
   *        目標時間，沒有設置則為 0
   * \param value_end
   *        目標進度，沒有設置則為 0
   *
   * \return 已編碼的速度
   */
  short get_output(long time, double value, 
                   long time_end, double value_end);  // return speed

  /**
   * 增加一個控制階段，內部使用
   */
  void add(speed_phase* phase);

  /**
   * Deep Clone 所有相關的控制階段
   */
  speed_phase* clone();

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