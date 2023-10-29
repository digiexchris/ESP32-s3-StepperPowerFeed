﻿// win64_tests.cpp : Defines the entry point for the application.
//

#include "win64_tests.h"
#include "MovePlanner.h"
#include "unity.h"

using namespace std;

void setUp(void) {}
void tearDown(void) {}

void test_CalculateSamples(void)
{
	TEST_ASSERT_EQUAL_UINT32(2000, MovePlanner::CalculateSamples(0,200,10));
	TEST_ASSERT_EQUAL_UINT32(2000, MovePlanner::CalculateSamples(200, 0, 10));
	TEST_ASSERT_EQUAL_UINT32(1995, MovePlanner::CalculateSamples(10, 200, 10));
}


void test_GenerateMove(void) {
	uint32_t* accSamples = 0;
	uint32_t* decSamples = 0;
	uint32_t* uniformSamples = 0;

	//accelerate, move, decelerate
	MovePlanner::GenerateMove(*accSamples, *decSamples, *uniformSamples, 200, 0, 10, 100, 10000);
	TEST_ASSERT_EQUAL_UINT32(2000, accSamples);
	TEST_ASSERT_EQUAL_UINT32(200, decSamples);
	TEST_ASSERT_EQUAL_UINT32(10000 - 2000 - 200, uniformSamples);

	MovePlanner::GenerateMove(*accSamples, *decSamples, *uniformSamples, 200, 0, 10, 100, 10000);
	TEST_ASSERT_EQUAL_UINT32(2000, accSamples);
	TEST_ASSERT_EQUAL_UINT32(200, decSamples);
	TEST_ASSERT_EQUAL_UINT32(10000 - 2000 - 200, uniformSamples);
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_CalculateSamples);
	RUN_TEST(test_GenerateMove);
	return UNITY_END();
}