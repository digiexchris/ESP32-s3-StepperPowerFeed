// win64_tests.cpp : Defines the entry point for the application.
//

#include "win64_tests.h"
#include "../src/components/MovePlanner/MovePlanner.h"
#include "lib/Unity/src/unity.h"

using namespace std;

void test_CalculateSamples(void)
{
	TEST_ASSERT_EQUAL_UINT32(2000, MovePlanner::CalculateSamples(0,200,10,10000));
}


int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_CalculateSamples);
	return UNITY_END();
}