#include "preset.h"

#ifndef _CMAPI_SPEED_INCLUDE_
#define _CMAPI_SPEED_INCLUDE_

/**
 * �Ω󱱨�q�����t�׼ƭȪ����
 */
typedef enum advanced_unit_type_e {
  SPEED_POINT = 0,
  MILLIVOLTS = 1,
  RPM = 2
} advanced_unit_type_e_t;

/**
 * ����q������
 */
typedef enum speed_control_phase_type_e {
  TIME_PHASE = 0,
  VALUE_PHASE = 1
} speed_control_phase_type_e_t;

/**
 * �����ճ̤j�����t
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
 * VEX V5 ���F��z�L�ϥγt���I(Speed Point)�B�@��(mV)�H�ΨC������t(RPM)�]�m�t�סA�䤤�C
 * �س�쪺�W�U�����O�O
 * - �t���I�G +-127
 * - �@��G +-12000
 * - �C������t�G +-100(RED)  +-200(GREEN)  +-600(BLUE)
 * 
 * �ھںD�ҡA�ڭ̤@��ϥγt���I����F�C�Ҧp�եΫ��O `groupLeft.move(80);` �Y�N��]�m�t��
 * �� 80sp�A�Ө�ƫŧi�h�O�o�ˡG
 * void motor_group::move(const short speed)
 *                                    -----
 *                                    ^
 *                                    ���`�ӻ��A�o�Ө�ƥu��䴩��J�t���I�A�n��JRPM�A
 *                                    �K�ݭn�A���ŧi .move_rpm() ����������ơC
 * 
 * ���F�䴩��J��L��쪺�t�צӤ��ݭn�A���ŧi�ۦ�����ơA�ڭ̻ݭn�ϥνs�X�t��(encoded speed)
 * 
 * 
 * �s�X�t�ץi�H�x�s�b16 bit ���Ʀr�A�t�׳�쪺�����H�G�i���Y2��4�Ӧ�@�P�w�C
 *
 * 32768 12384 8192 4096 | 2048 1024 512 256 | 128 64 32 16 | 8 4 2 1
 *       <----------------------------------------------------------> millivolts
 *                              <-----------------------------------> rpm
 *                                             <--------------------> speed point
 *
 * 01?? ???? ???? ???? �@��p��0
 * <--> ���w�O 0101 �� 0110 �� 0111
 *
 * 10?? ???? ???? ???? �@��j��0
 * <--> ���w�O 1000 �� 1001 �� 1010
 * 
 * 1011 0??? ???? ???? �C������t
 * <--> ���w�O 1011(+) �� 0100(-)
 *
 * 0000 0000 ???? ???? speed point
 * <> ���w�O���� 0 �Ϊ̥��� 1
 * 
 * �@��̤p�� = [*101000100100000] = -12000
 * �@��̤j�� = [*010111011100000] = +12000
 *
 * �䴩�G�ɼ��ഫ�G
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
 * ���F�o�ӽs�X�A�ڭ̫K�i�H�ϥΤ��P���]�m���F���t�סG
 * groupLeft.move(80);
 * groupLeft.move(80_pt);
 * groupLeft.move(6000_mV);
 * groupLeft.move(234_rpm);
 * 
 * 
 * �o�����O�ΨӸѽX�H�νs�X�t�׼ƭȡA��i�H����t�@�س�쪺�t�׼ƭȡC
 * 
 */
class advanced_unit {
 private:
  // �t�׳��
  advanced_unit_type_e type_;

  // �t���I�p�⵪��
  int8_t point_ = 0;

  // �@��p�⵪��
  short mV_ = 0;

  // �C������t�p�⵪��
  short rpm_ = 0;

  // ���Q�t�ױ�����F��������
  pros::motor_gearset_e_t gearset_ = pros::E_MOTOR_GEARSET_INVALID;

  // �ѽX�w�s�X���t��
  void decode_(const short speed);

 public:
  /**
   * �c�y���
   *
   * \param encoded
   *        �w�s�X���t��
   */
  advanced_unit(const short encoded);

  /**
   * �c�y���
   *
   * \param encoded
   *        �w�s�X���t��
   * \param gearset
   *        ���Q�t�ױ�����F��������
   */
  advanced_unit(const short encoded, const pros::motor_gearset_e_t gearset);
   
  /**
   * �c�y���
   *
   * \param speed
   *        �ѽX���t���ܶq
   * \param type
   *        �t�׳������
   */
  advanced_unit(const short speed, advanced_unit_type_e type);


  /**
   * ���o�ѽX���@��t��
   *
   * \return �@�� -12000 ~ +12000
   */
  short to_millivolts();

  /**
   * ���o�ѽX���t���I
   *
   * \return �t���I -127 ~ +127
   */
  short to_speedpoint();

  /**
   * ���o�ѽX���C������t
   *
   * \return �C������t�t�� -600 ~ +600
   */
  short to_rpm();

  /**
   * �ھڭ���x�s����������A�^�ǩҹ������ƭ�
   *
   * \return ��l���Ʀr
   */
  short to_raw();

  /**
   * ���o�w�s�X���t��
   *
   * \return �w�s�X���t��
   */
  short to_encoded();

  /**
   * ���o�t���X������
   *
   * \return �t���X������
   */
  advanced_unit_type_e get_type();

  /**
   * �]�m���Q�t�ױ�����F��������, �i�H�Ψӭp���L��쪺�t�׼ƭ�
   * ��p�A��J�F�C������t�A�h�i�H�q�L�����ըӭp��q���X�ʳt��
   *
   * \param gearset
   *        ���Q�t�ױ�����F��������
   */
  void set_gearset(const pros::motor_gearset_e_t gearset);

  /**
   * ���o����������
   *
   * \return ������
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
 * �����O���@�ӳt�ױ���q�A�@�뤣�|��ʳЫبϥΡA����ϥΦb���O�W�A�H��ܫ��O��������A���F
 * ����X�M�ثe�ɶ��H�ΥؼЭȤ��������Y�A�Ҥl�G 
 * 
 * forward(80, 0_ms[0] >> 500_ms[60] >> -20_val[60] >> -10_val[30])
 *                                                     -----------
 *                                                     ^
 *                                                     �̫�@�ӷ|�Q�ǤJ���O
 * 
 * �N����A�V�e�樫80 cm�C�Ѷ}�l30�t�סA��500�@��[��60�t�סA����Ѿl�Z��20cm�}�l��t�A�ܳ�
 * �l�Z��10cm���30�t�סA���ܧ����C
 * 
 * �U�Ӷ��q�������t�ױN�|�z�L�p�⺥�ܨ�t�@�Ӷ��q�A�䴩�ϥΤ��P�������t�׳��A�M�Ӧ��ҭ���G
 * 
 * forward(80, 0_val[10] >> 50_val[60_pt] >> 100_val[12_v] >> 150_val[100_rpm])
 *                   |------�i��------|------�i��------|-------���i��-------|
 *                       ��lSP��SP       SP���/�@��        RPM�M��L����
 * 
 * �b�o�ӨҤl�A100cm��149cm�|����12���X�A�����150cm�ߧY���ܨ�100_rpm�C
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
   * �p�Ⱘ�F����X
   *
   * \param time
   *        �{�b���ɶ�
   * \param value
   *        �{�b���i�׭�
   * \param time_end
   *        �ؼЮɶ��A�S���]�m�h�� 0
   * \param value_end
   *        �ؼжi�סA�S���]�m�h�� 0
   *
   * \return �w�s�X���t��
   */
  short get_output(long time, double value, 
                   long time_end, double value_end);  // return speed

  /**
   * �W�[�@�ӱ���q�A�����ϥ�
   */
  void add(speed_phase* phase);

  /**
   * Deep Clone �Ҧ�����������q
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