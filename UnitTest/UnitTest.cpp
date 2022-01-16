#include "CppUnitTest.h"
#include "../SpeedUnit/speed.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		
		TEST_METHOD(TestSpeedPoint)
		{
			Assert::AreEqual(0_pt, 0_mV);
			Assert::AreEqual(127_pt, 12000_mV);
			Assert::AreEqual(300_pt, 12000_mV);
			Assert::AreEqual(64_pt, 6047_mV);
			Assert::AreEqual(-64_pt, -6047_mV);
			Assert::AreEqual(-300_pt, -12000_mV);
			Assert::AreEqual(-127_pt, -12000_mV);

			Assert::AreEqual(0.0_pt, 0_mV);
			Assert::AreEqual(127.0_pt, 12000_mV);
			Assert::AreEqual(300.0_pt, 12000_mV);
			Assert::AreEqual(64.0_pt, 6047_mV);
			Assert::AreEqual(-64.0_pt, -6047_mV);
			Assert::AreEqual(-300.0_pt, -12000_mV);
			Assert::AreEqual(-127.0_pt, -12000_mV);

			Assert::AreEqual(0_pt, 0.0_mV);
			Assert::AreEqual(127_pt, 12000.0_mV);
			Assert::AreEqual(300_pt, 12000.0_mV);
			Assert::AreEqual(64_pt, 6047.0_mV);
			Assert::AreEqual(-64_pt, -6047.0_mV);
			Assert::AreEqual(-300_pt, -12000.0_mV);
			Assert::AreEqual(-127_pt, -12000.0_mV);
			
			Assert::AreEqual(127_pt, 24000_mV);
			Assert::AreEqual(-127_pt, -24000_mV);
			Assert::AreEqual(127_pt, 24000.0_mV);
			Assert::AreEqual(-127_pt, -24000.0_mV);
		}

		TEST_METHOD(TestVoltage)
		{
			Assert::AreEqual(0_v, 0_mV);
			Assert::AreEqual(12_v, 12000_mV);
			Assert::AreEqual(20_v, 12000_mV);
			Assert::AreEqual(6.047_v, 6047_mV);
			Assert::AreEqual(-6.047_v, -6047_mV);
			Assert::AreEqual(-20_v, -12000_mV);
			Assert::AreEqual(-12_v, -12000_mV);

			Assert::AreEqual(0.0_v, 0_mV);
			Assert::AreEqual(12.0_v, 12000_mV);
			Assert::AreEqual(20.0_v, 12000_mV);
			Assert::AreEqual(6.047_v, 6047_mV);
			Assert::AreEqual(-6.047_v, -6047_mV);
			Assert::AreEqual(-20.0_v, -12000_mV);
			Assert::AreEqual(-12.0_v, -12000_mV);
		}

		TEST_METHOD(TestRPM)
		{
			Assert::AreEqual(0_rpm, 0_rps);
			Assert::AreEqual(60_rpm, 1_rps);
			Assert::AreEqual(600_rpm, 10_rps);
			Assert::AreEqual(6000_rpm, 10_rps);
			Assert::AreEqual(600_rpm, 100_rps);
			Assert::AreEqual(8000000_rpm, 10_rps);
			Assert::AreEqual(-6000_rpm, -10_rps);
			Assert::AreEqual(-600_rpm, -10_rps);
			Assert::AreEqual(-60_rpm, -1_rps);
		}

		TEST_METHOD(TestAdvancedUnit)
		{
            Assert::AreEqual(12_v, advanced_unit(12_v).to_encoded());
			Assert::AreEqual(12_v, advanced_unit(12000_mV).to_encoded());
			Assert::AreEqual(12_v, advanced_unit(127).to_encoded());  // change to mV type
			Assert::AreEqual(12_v, advanced_unit(3000).to_encoded());  // change to mV type
			Assert::AreEqual(12_v, advanced_unit(127_pt).to_encoded());
			Assert::AreEqual(127_pt, advanced_unit(127_pt).to_encoded());

			Assert::AreEqual((short)12000, advanced_unit(12_v).to_raw());
			Assert::AreEqual((short)12000, advanced_unit(12000_mV).to_raw());
			Assert::AreEqual((short)127, advanced_unit(127).to_raw());
			Assert::AreEqual((short)127, advanced_unit(3000).to_raw());
			Assert::AreEqual((short)12000, advanced_unit(127_pt).to_raw());
			Assert::AreEqual((short)12000, advanced_unit(127_pt).to_raw());
		
			Assert::AreEqual((short)12000, advanced_unit(12_v).to_millivolts());

			Assert::AreEqual((short)127, advanced_unit(12_v).to_speedpoint());
			Assert::AreEqual((short)63, advanced_unit(6_v).to_speedpoint());
			Assert::AreEqual((short)0, advanced_unit(0_v).to_speedpoint());
			Assert::AreEqual((short)-63, advanced_unit(-6_v).to_speedpoint());
            Assert::AreEqual((short)-127, advanced_unit(-12_v).to_speedpoint());

			Assert::AreEqual((short)0, advanced_unit(6_v).to_rpm());
			Assert::AreEqual((short)0, advanced_unit(127).to_rpm());
			Assert::AreEqual((short)100, advanced_unit(127, GEARSET::RED).to_rpm());
			Assert::AreEqual((short)200, advanced_unit(127, GEARSET::GREEN).to_rpm());
			Assert::AreEqual((short)600, advanced_unit(127, GEARSET::BLUE).to_rpm());

			Assert::AreEqual((short)0, advanced_unit(100_rpm).to_millivolts());
			Assert::AreEqual((short)0, advanced_unit(100_rpm).to_speedpoint());
			Assert::AreEqual((short)127, advanced_unit(100_rpm, GEARSET::RED).to_speedpoint());
			Assert::AreEqual((short)63, advanced_unit(100_rpm, GEARSET::GREEN).to_speedpoint());
			Assert::AreEqual((short)21, advanced_unit(100_rpm, GEARSET::BLUE).to_speedpoint());
			Assert::AreEqual((short)127, advanced_unit(600_rpm, GEARSET::RED).to_speedpoint());
			Assert::AreEqual((short)127, advanced_unit(600_rpm, GEARSET::GREEN).to_speedpoint());
			Assert::AreEqual((short)127, advanced_unit(600_rpm, GEARSET::BLUE).to_speedpoint());

			Assert::IsTrue(SPEED_POINT == advanced_unit(127).get_type());
			Assert::IsTrue(MILLIVOLTS == advanced_unit(127_pt).get_type());
			Assert::IsTrue(MILLIVOLTS == advanced_unit(6_v).get_type());
			Assert::IsTrue(MILLIVOLTS == advanced_unit(6000_mV).get_type());
			Assert::IsTrue(RPM == advanced_unit(100_rpm).get_type());
			Assert::IsTrue(RPM == advanced_unit(10_rps).get_type());

			Assert::IsTrue(pros::E_MOTOR_GEARSET_INVALID == advanced_unit(127).get_gearset());
			Assert::IsTrue(pros::E_MOTOR_GEARSET_36 == advanced_unit(127, GEARSET::RED).get_gearset());
			Assert::IsTrue(pros::E_MOTOR_GEARSET_18 == advanced_unit(127, GEARSET::GREEN).get_gearset());
			Assert::IsTrue(pros::E_MOTOR_GEARSET_06 == advanced_unit(127, GEARSET::BLUE).get_gearset());

			Assert::AreEqual((short)-600, advanced_unit(-800, RPM).to_raw());
			Assert::AreEqual((short)30, advanced_unit(30, RPM).to_raw());
			Assert::AreEqual((short)600, advanced_unit(800, RPM).to_raw());

			Assert::AreEqual((short)-12000, advanced_unit(-14000, MILLIVOLTS).to_raw());
			Assert::AreEqual((short)30, advanced_unit(30, MILLIVOLTS).to_raw());
			Assert::AreEqual((short)12000, advanced_unit(14000, MILLIVOLTS).to_raw());

			Assert::AreEqual((short)-127, advanced_unit(-300, SPEED_POINT).to_raw());
			Assert::AreEqual((short)30, advanced_unit(30, SPEED_POINT).to_raw());
			Assert::AreEqual((short)127, advanced_unit(300, SPEED_POINT).to_raw());
		}

		TEST_METHOD(TestTargets) {
			Assert::AreEqual(4L, time_target(4).ms);
			Assert::AreEqual(4L, (4_ms).ms);
			Assert::AreEqual(6.0, value_target(6).val);
			Assert::AreEqual(6.0, (6_val).val);

			auto test1 = 123_ms [456];
			Assert::AreEqual(123L, test1.time);
			Assert::AreEqual(0.0, test1.value);
			Assert::AreEqual((short)456, test1.speed);
			Assert::IsTrue(TIME_PHASE == test1.type);

			auto test2 = -456.789_val [234_rpm];
			Assert::AreEqual(0L, test2.time);
			Assert::AreEqual(-456.789, test2.value);
			Assert::AreEqual(234_rpm, test2.speed);
			Assert::IsTrue(VALUE_PHASE == test2.type);

			auto test3 = test1 >> test2;
			Assert::IsFalse(test2.previous == test3.previous);
			Assert::AreEqual(test2.time, test3.time);
			Assert::AreEqual(test2.value, test3.value);
			Assert::AreEqual(test2.speed, test3.speed);
			Assert::IsTrue(test2.type == test3.type);
            Assert::IsTrue(test1.previous == test3.previous->previous);
			Assert::AreEqual(test1.time, test3.previous->time);
			Assert::AreEqual(test1.value, test3.previous->value);
			Assert::AreEqual(test1.speed, test3.previous->speed);
			Assert::IsTrue(test1.type == test3.previous->type);
		}

		TEST_METHOD(TestSpeedPhase1) {
			auto test1 = 0_ms [30];
			// now time, now value, end time, end value
            Assert::AreEqual((short)30, test1.get_output(0, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, 100, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, -100, 0));
			Assert::AreEqual((short)30, test1.get_output(100, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(-100, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(-100, 0, 100, 0));
			Assert::AreEqual((short)30, test1.get_output(-100, 0, -100, 0));
			Assert::AreEqual((short)30, test1.get_output(100, 0, 100, 0));
			Assert::AreEqual((short)30, test1.get_output(100, 0, -100, 0));
		
			Assert::AreEqual((short)9000, 0_ms [9000].get_output(0, 0, 0, 0));
		}

		TEST_METHOD(TestSpeedPhase2_pt_pt) {
            auto test1 = 100_ms [30] >> 200_ms [60];
			// now time, now value, end time, end value
			Assert::AreEqual((short)30, test1.get_output(-50, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(50, 0, 0, 0));
			Assert::AreEqual(30_pt, test1.get_output(100, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, 100, 0));
			Assert::AreEqual(45_pt, test1.get_output(150, 0, 0, 0));
			Assert::AreEqual((short)60, test1.get_output(200, 0, 0, 0));
			Assert::AreEqual((short)60, test1.get_output(250, 0, 0, 0));
			Assert::AreEqual(45_pt, test1.get_output(150, 0, 200, 0));
			Assert::AreEqual(45_pt, test1.get_output(150, 0, -200, 0));
			Assert::AreEqual((short)60, test1.get_output(200, 0, 200, 0));
			Assert::AreEqual((short)60, test1.get_output(200, 0, -200, 0));
		}

		TEST_METHOD(TestSpeedPhase2_pt_mv) {
			auto test1 = 100_ms [30] >> 200_ms [5669_mV];
			// now time, now value, end time, end value
			Assert::AreEqual((short)30, test1.get_output(-50, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(50, 0, 0, 0));
			Assert::AreEqual(30_pt, test1.get_output(100, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, 100, 0));
			Assert::AreEqual(45_pt, test1.get_output(150, 0, 0, 0));
			Assert::AreEqual(5669_mV, test1.get_output(200, 0, 0, 0));
			Assert::AreEqual(5669_mV, test1.get_output(250, 0, 0, 0));
			Assert::AreEqual(45_pt, test1.get_output(150, 0, 200, 0));
			Assert::AreEqual(45_pt, test1.get_output(150, 0, -200, 0));
			Assert::AreEqual(5669_mV, test1.get_output(200, 0, 200, 0));
            Assert::AreEqual(5669_mV, test1.get_output(200, 0, -200, 0));

			auto test2 = 100_ms [5669_mV] >> 200_ms [30];
			// now time, now value, end time, end value
			Assert::AreEqual(5669_mV, test2.get_output(-50, 0, 0, 0));
			Assert::AreEqual(5669_mV, test2.get_output(0, 0, 0, 0));
			Assert::AreEqual(5669_mV, test2.get_output(50, 0, 0, 0));
			Assert::AreEqual(60_pt, test2.get_output(100, 0, 0, 0));
			Assert::AreEqual(5669_mV, test2.get_output(0, 0, 100, 0));
			Assert::AreEqual(45_pt, test2.get_output(150, 0, 0, 0));
			Assert::AreEqual((short)30, test2.get_output(200, 0, 0, 0));
			Assert::AreEqual((short)30, test2.get_output(250, 0, 0, 0));
			Assert::AreEqual(45_pt, test2.get_output(150, 0, 200, 0));
			Assert::AreEqual(45_pt, test2.get_output(150, 0, -200, 0));
            Assert::AreEqual((short)30, test2.get_output(200, 0, 200, 0));
            Assert::AreEqual((short)30, test2.get_output(200, 0, -200, 0));
		}

		TEST_METHOD(TestSpeedPhase2_pt_rpm) {
			auto test1 = 100_ms [30] >> 200_ms [123_rpm];
			// now time, now value, end time, end value
			Assert::AreEqual((short)30, test1.get_output(-50, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(50, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(100, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, 100, 0));
			Assert::AreEqual((short)30, test1.get_output(150, 0, 0, 0));
			Assert::AreEqual(123_rpm, test1.get_output(200, 0, 0, 0));
			Assert::AreEqual(123_rpm, test1.get_output(250, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(150, 0, 200, 0));
			Assert::AreEqual((short)30, test1.get_output(150, 0, -200, 0));
			Assert::AreEqual(123_rpm, test1.get_output(200, 0, 200, 0));
            Assert::AreEqual(123_rpm, test1.get_output(200, 0, -200, 0));
		}

		TEST_METHOD(TestSpeedPhase2_ms_val) {
			auto test1 = 100_ms [30] >> 101_val [60];
			// now time, now value, end time, end value
			Assert::AreEqual((short)30, test1.get_output(-50, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(50, 0, 0, 0));
			Assert::AreEqual(30_pt, test1.get_output(100, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, 100, 0));
			Assert::AreEqual(30_pt, test1.get_output(150, 0, 0, 0));
			Assert::AreEqual(30_pt, test1.get_output(200, 0, 100, 0));
			Assert::AreEqual(30_pt, test1.get_output(100, 1, 0, 0));
			Assert::AreEqual(1388_mV, test1.get_output(100, -50, 0, 0));
			Assert::AreEqual(45_pt, test1.get_output(100, 51, 0, 0));
			Assert::AreEqual((short)60, test1.get_output(100, 101, 0, 0));
			Assert::AreEqual((short)60, test1.get_output(100, 200, 0, 0));
		}

		TEST_METHOD(TestSpeedPhase2_val_ms) {
			auto test1 = 100_val [30] >> 101_ms [60];
			// now time, now value, end time, end value
			Assert::AreEqual((short)30, test1.get_output(-50, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 0, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(50, 0, 0, 0));
			Assert::AreEqual((short)60, test1.get_output(101, 0, 0, 0));
            Assert::AreEqual((short)30, test1.get_output(0, 0, 100, 0));
			Assert::AreEqual((short)60, test1.get_output(200, 0, 100, 0));
			Assert::AreEqual((short)30, test1.get_output(0, -50, 0, 0));
			Assert::AreEqual((short)30, test1.get_output(0, 50, 0, 0));
			Assert::AreEqual(30_pt, test1.get_output(0, 100, 0, 0));
			Assert::AreEqual(2834_mV, test1.get_output(1, 100, 0, 0));
			Assert::AreEqual(45_pt, test1.get_output(51, 100, 0, 0));
			Assert::AreEqual((short)60, test1.get_output(101, 100, 0, 0));
			Assert::AreEqual((short)60, test1.get_output(200, 100, 0, 0));
		}

		TEST_METHOD(TestSpeedPhase2_negative) {
			auto test1 = -100_ms [30] >> -100_val [60];
			// now time, now value, end time, end value
			Assert::AreEqual((short)30, test1.get_output(0, 0, 200, 200));
			Assert::AreEqual((short)30, test1.get_output(99, 50, 200, 200));
			Assert::AreEqual(30_pt, test1.get_output(100, 50, 200, 200));
			Assert::AreEqual(45_pt, test1.get_output(100, 75, 200, 200));
			Assert::AreEqual((short)60, test1.get_output(150, 100, 200, 200));
			Assert::AreEqual((short)60, test1.get_output(200, 200, 200, 200));
		}

		TEST_METHOD(TestSpeedPhase4_mix) {
			auto test1 = 0_ms [30] >> 101_val [60] >> 101_ms[90] >> 201_val[120];
			// now time, now value, end time, end value
			Assert::AreEqual(30_pt, test1.get_output(0, 0, 200, 201));
			Assert::AreEqual(30_pt, test1.get_output(0, 1, 200, 201));
			Assert::AreEqual(45_pt, test1.get_output(0, 51, 200, 201));
			Assert::AreEqual(60_pt, test1.get_output(1, 101, 200, 201));
			Assert::AreEqual(75_pt, test1.get_output(51, 101, 200, 201));
			Assert::AreEqual(90_pt, test1.get_output(101, 101, 200, 201));
			Assert::AreEqual(9920_mV, test1.get_output(101, 151, 200, 201));
			Assert::AreEqual((short)120, test1.get_output(101, 201, 200, 201));
		}

		TEST_METHOD(TestSpeedPhase_getOutput) {
			auto test1 =  0_ms[30] >> 200_ms[60] >> 100_val[80] >> -50_val[60] >> -1_val[30];

			int count = 200 * 100;
			double x = 300;
			double y = 0;

			// 20000 times ~ 6ms
			for (int i = 0; i < 200 * 100; i++) {
				y += x / count;
				if (test1.get_output(y, y, x, x) == 0)
                    Assert::Fail();
			}
		}

		TEST_METHOD(TestSpeedPhase_clone) {
			auto test1 =  0_ms[30] >> 200_ms[60] >> 100_val[80] >> -50_val[60] >> -1_val[30];

			int count = 200 * 100;

			// 20000 times ~ 22ms
			for (int i = 0; i < 200 * 100; i++) {
                delete test1.clone();
			}
		}
	};
}
