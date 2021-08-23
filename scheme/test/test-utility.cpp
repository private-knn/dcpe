#include "definitions.h"
#include "utility.hpp"

#include "gtest/gtest.h"

using namespace std;

namespace DCPE
{
	class UtilityTest : public ::testing::Test
	{
	};

	TEST_F(UtilityTest, NoSeed)
	{
		const auto n = 20uLL;
		for (number i = 0; i < 100; i++)
		{
			auto first	= get_random_bytes(n);
			auto second = get_random_bytes(n);

			ASSERT_NE(first, second);
		}
	}

	TEST_F(UtilityTest, SameSeed)
	{
		const auto n = 20uLL;
		int seed	 = 0x15;

		srand(seed);

		auto first = get_random_bytes(n);

		srand(seed);

		auto second = get_random_bytes(n);

		ASSERT_EQ(first, second);
	}

	TEST_F(UtilityTest, SameSeedNumber)
	{
		const auto max = 100000uLL;
		int seed	   = 0x15;

		srand(seed);

		auto first = get_ramdom_number(max);

		srand(seed);

		auto second = get_ramdom_number(max);

		ASSERT_EQ(first, second);
	}
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
