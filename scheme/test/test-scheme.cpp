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
		inline static const number beta = 1 << 20;

		protected:
		unique_ptr<Scheme> scheme;

		SchemeTest()
		{
			this->scheme = make_unique<Scheme>(beta);
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

		auto get_random_value = [](auto min, auto max)
		{
			VALUE_T f = (VALUE_T)rand() / RAND_MAX;
			return min + f * (max - min);
		};

		for (auto &&dimensions : {1, 2, 3, 5, 10, 25, 50})
		{
			for (auto run = 0; run < runs; run++)
			{
				auto key = scheme->keygen();

				vector<VALUE_T> message;
				message.resize(dimensions);
				for (auto i = 0; i < dimensions; i++)
				{
					message[i] = get_random_value(-1000.0, +1000.0);
				}

				vector<VALUE_T> ciphertext;
				ciphertext.resize(dimensions);
				auto nonce = scheme->encrypt(key, TO_ARRAY(message), dimensions, TO_ARRAY(ciphertext));

				vector<VALUE_T> decrypted;
				decrypted.resize(dimensions);
				scheme->decrypt(key, TO_ARRAY(ciphertext), dimensions, nonce, TO_ARRAY(decrypted));

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
			vector<VALUE_T> result;
			result.resize(dimensions);
			for (auto i = 0; i < dimensions; i++)
			{
				result[i] = min + ((VALUE_T)rand() / RAND_MAX) * (max - min);
			}
			return result;
		};

		auto check_implication = [](VALUE_T d1, VALUE_T d2, VALUE_T df1, VALUE_T df2)
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

				vector<VALUE_T> f_x, f_y, f_z;
				f_x.resize(dimensions);
				f_y.resize(dimensions);
				f_z.resize(dimensions);

				scheme->encrypt(key, TO_ARRAY(x), dimensions, TO_ARRAY(f_x));
				scheme->encrypt(key, TO_ARRAY(y), dimensions, TO_ARRAY(f_y));
				scheme->encrypt(key, TO_ARRAY(z), dimensions, TO_ARRAY(f_z));

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
