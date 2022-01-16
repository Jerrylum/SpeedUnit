#include "speed.hpp"

void advanced_unit::decode_(const short encoded) {
  auto f2 = std::bitset<4>(encoded >> 12);

  if (f2 == 0b1011 || f2 == 0b0100) {  // rpm + or -
    short cache = (encoded & 0x07FF) << 5;
    rpm_ = cache >> 5;
    type_ = RPM;
  } else if (f2 >> 2 == 0b01) {  // - mV
    mV_ = encoded | 0x8000;
    type_ = MILLIVOLTS;
  } else if (f2 >> 2 == 0b10) {  // + mV
    mV_ = encoded & 0x7FFF;
    type_ = MILLIVOLTS;
  } else {                       // speed point
    /**
     * @brief overflow range +12383 ~ -12382
     * 
     */
    point_ = encoded;
    if (encoded > 127) point_ = 127;
    if (encoded < -127) point_ = -127;
    mV_ = 12000.0 / 127 * point_;
    type_ = SPEED_POINT;
  }
}

advanced_unit::advanced_unit(const short encoded) {
  decode_(encoded);  // calculate speed
}

advanced_unit::advanced_unit(const short encoded,
                             const pros::motor_gearset_e_t gearset) {
  decode_(encoded);
  set_gearset(gearset);
}

advanced_unit::advanced_unit(const short speed, advanced_unit_type_e type) {
  type_ = type;
  if (type_ == RPM) {
    rpm_ = speed;
    if (speed > 600) rpm_ = 600;
    if (speed < -600) rpm_ = -600;
  } else if (type_ == MILLIVOLTS) {
    mV_ = speed;
    if (speed > 12000) mV_ = 12000;
    if (speed < -12000) mV_ = -12000;
  } else if (type_ == SPEED_POINT) {
    point_ = speed;
    if (speed > 127) point_ = 127;
    if (speed < -127) point_ = -127;
    mV_ = 12000.0 / 127 * point_;
  }
}

short advanced_unit::to_millivolts() {
  return mV_;  // calculated in constructor
}

short advanced_unit::to_speedpoint() {
  if (point_ == 0 && type_ != SPEED_POINT) point_ = 127 / 12000.0 * mV_;
  return point_;
}

short advanced_unit::to_rpm() {
  return rpm_;  // calculated in constructor
}

short advanced_unit::to_raw() {
  if (type_ == RPM) return rpm_;
  else if (type_ == MILLIVOLTS) return mV_;
  else if (type_ == SPEED_POINT) return point_;
  return 0;
}

short advanced_unit::to_encoded() {
  if (type_ == RPM) return encode_rpm(rpm_);
  else if (type_ == MILLIVOLTS) return encode_millivoltage(mV_);
  else if (type_ == SPEED_POINT) return encode_millivoltage(mV_);
  return 0;
}

advanced_unit_type_e advanced_unit::get_type() { return type_; }

void advanced_unit::set_gearset(const pros::motor_gearset_e_t gearset) {
  gearset_ = gearset;
  int max_rpm;
  if (gearset_ == GEARSET::RED)
    max_rpm = GEARSET_RED_MAX_RPM;
  else if (gearset == GEARSET::GREEN)
    max_rpm = GEARSET_GREEN_MAX_RPM;
  else
    max_rpm = GEARSET_BLUE_MAX_RPM;

  if (mV_ == 0) {  // get mV_
    int value = 12000.0 / max_rpm * rpm_;
    if (value > 12000) value = 12000;
    if (value < -12000) value = -12000;
    mV_ = value;
  } else {
    int value = max_rpm / 12000.0 * mV_;
    if (value > max_rpm) value = max_rpm;
    if (value < -max_rpm) value = -max_rpm;
    rpm_ = value;
  }
}

pros::motor_gearset_e_t advanced_unit::get_gearset() { return gearset_; }

void speed_phase::add(speed_phase* phase) { previous = phase; }

speed_phase* speed_phase::clone() {
  speed_phase* rtn = new speed_phase(0);
  rtn->type = this->type;
  rtn->time = this->time;
  rtn->value = this->value;
  rtn->speed = this->speed;
  if (this->previous != nullptr) rtn->previous = this->previous->clone();

  return rtn;
}

short speed_phase::get_output(long time, double value, long time_end, double value_end) {
  #define true_value(f) (f < 0 ? (value_end + f) : f)
  #define true_time(f) (f < 0 ? (time_end + f) : f)

  speed_phase* a = this;
  speed_phase* b = this;
  speed_phase* x = this;
  while (x != nullptr) {
    a = x;
    if (x->type == TIME_PHASE) {
      if (true_time(x->time) <= time) {
        break;
      }
    } else {
      if (true_value(x->value) <= value) {
        break;
      }
    }
    b = x;
    x = x->previous;
  }

  if (a == b) return a->speed;

  double target, fmin, fmax;
  short tmin, tmax, raw_result;

  if (b->type == TIME_PHASE) {
    if (a->type != TIME_PHASE && a->time == 0) {
        a->time = time;  // post-fill
    }

    target = time;
    fmin = true_time(a->time);
    fmax = true_time(b->time);
  } else if (b->type == VALUE_PHASE) {
    if (a->type != VALUE_PHASE && a->value == 0) {
      a->value = value;  // post-fill
    }

    target = value;
    fmin = true_value(a->value);
    fmax = true_value(b->value);
  } else {
    return a->speed;
  }

  advanced_unit a_au(a->speed);
  advanced_unit b_au(b->speed);
  advanced_unit_type_e using_type;

  if ((a_au.get_type() == SPEED_POINT || a_au.get_type() == MILLIVOLTS) && 
      (b_au.get_type() == SPEED_POINT || b_au.get_type() == MILLIVOLTS)) {
    tmin = a_au.to_millivolts();
    tmax = b_au.to_millivolts();
    using_type = MILLIVOLTS;
  } else if (a_au.get_type() == b_au.get_type()) {
    tmin = a_au.to_raw();
    tmax = b_au.to_raw();
    using_type = a_au.get_type();
  } else {
    return a->speed;
  }
  
  raw_result = (target - fmin) / (fmax - fmin) * (tmax - tmin) + tmin;
  return advanced_unit(raw_result, using_type).to_encoded();
}

speed_phase::~speed_phase() { delete previous; }

speed_phase::speed_phase(const speed_phase& other) {
  this->type = other.type;
  this->time = other.time;
  this->value = other.value;
  this->speed = other.speed;
  if (other.previous != nullptr) this->previous = other.previous->clone();
}

speed_phase time_target::operator[](short int speed) {
  return speed_phase(TIME_PHASE, ms, 0, speed);
}

speed_phase value_target::operator[](short int speed) {
  return speed_phase(VALUE_PHASE, 0, val, speed);
}

time_target operator"" _ms(unsigned long long int x) { return time_target(x); }

value_target operator"" _val(long double x) { return value_target(x); }

value_target operator"" _val(unsigned long long int x) {
  return value_target(x);
}

speed_phase operator>>(speed_phase a, speed_phase b) {
  b.add(a.clone());
  return b;
}

speed_phase operator-(speed_phase target) {
  target.time = -target.time;
  target.value = -target.value;
  return target;
}