#include "unity.h"

void run_crc_tests(void);
void run_binary_tests(void);
void run_serial_number_tests(void);
void run_build_binary_tests(void);

void setUp(void)
{
}
void tearDown(void)
{
}

int main(void)
{
	UNITY_BEGIN();
	run_crc_tests();
	run_binary_tests();
	run_serial_number_tests();
	run_build_binary_tests();
	return UNITY_END();
}
