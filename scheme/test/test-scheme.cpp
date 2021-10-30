#include "definitions.h"
#include "scheme.hpp"
#include "utility.hpp"

#include "gtest/gtest.h"

using namespace std;

namespace DCPE
{
	class SchemeTest : public ::testing::Test
	{
		public:
		inline static const number beta	 = 100;
		inline static const number max_s = 10000;

		protected:
		unique_ptr<Scheme> scheme;

		SchemeTest()
		{
			this->scheme = make_unique<Scheme>(beta, max_s);
		}
	};

	TEST_F(SchemeTest, Initialization)
	{
		SUCCEED();
	}

	TEST_F(SchemeTest, EncryptDecrypt)
	{
		const auto runs	 = 1000;
		const auto error = 1.0;

		auto get_random_double = [](auto min, auto max)
		{
			double f = (double)rand() / RAND_MAX;
			return min + f * (max - min);
		};

		for (auto &&dimensions : {1, 2, 3, 5, 10, 25, 50})
		{
			for (auto run = 0; run < runs; run++)
			{
				auto key = scheme->keygen();

				vector<double> message;
				message.resize(dimensions);
				for (auto i = 0; i < dimensions; i++)
				{
					message[i] = get_random_double(-1000.0, +1000.0);
				}

				auto ciphertext = scheme->encrypt(key, message);

				auto decrypted = scheme->decrypt(key, ciphertext);

				for (auto i = 0; i < dimensions; i++)
				{
					ASSERT_NEAR(message[i], decrypted[i], error);
				}
			}
		}
	}

	TEST_F(SchemeTest, PreserveDistanceComparison)
	{
		const auto runs = 1000;

		auto get_random_vector = [](auto dimensions, auto min, auto max)
		{
			vector<double> result;
			result.resize(dimensions);
			for (auto i = 0; i < dimensions; i++)
			{
				result[i] = min + ((double)rand() / RAND_MAX) * (max - min);
			}
			return result;
		};

		auto check_implication = [](double d1, double d2, double df1, double df2)
		{
			if (d1 < d2 - beta)
			{
				return df1 < df2;
			}
			else
			{
				return true;
			}
		};

		for (auto &&dimensions : {1, 2, 3, 5, 10, 25, 50})
		{
			for (auto run = 0; run < runs; run++)
			{
				auto key = scheme->keygen();

				auto x = get_random_vector(dimensions, -1000.0, +1000.0);
				auto y = get_random_vector(dimensions, -1000.0, +1000.0);
				auto z = get_random_vector(dimensions, -1000.0, +1000.0);

				auto f_x = scheme->encrypt(key, x).first;
				auto f_y = scheme->encrypt(key, y).first;
				auto f_z = scheme->encrypt(key, z).first;

				auto xy = distance(x, y);
				auto yz = distance(y, z);
				auto xz = distance(x, z);

				auto f_xy = distance(f_x, f_y);
				auto f_yz = distance(f_y, f_z);
				auto f_xz = distance(f_x, f_z);

				ASSERT_TRUE(check_implication(xy, yz, f_xy, f_yz));
				ASSERT_TRUE(check_implication(yz, xy, f_yz, f_xy));

				ASSERT_TRUE(check_implication(yz, xz, f_yz, f_xz));
				ASSERT_TRUE(check_implication(xz, yz, f_xz, f_yz));

				ASSERT_TRUE(check_implication(xy, xz, f_xy, f_xz));
				ASSERT_TRUE(check_implication(xz, xy, f_xz, f_xy));
			}
		}
	}
}

int main(int argc, char **argv)
{
	srand(TEST_SEED);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
