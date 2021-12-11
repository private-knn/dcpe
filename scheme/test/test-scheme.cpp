#include "scheme.hpp"
#include "utility.hpp"

#include "gtest/gtest.h"

// change to run all tests from different seed
const auto TEST_SEED = 0x13;

namespace DCPE
{
	template <typename TypeParam>
	class SchemeTest : public testing::Test
	{
		public:
		const TypeParam beta  = 1.0 * (1 << 10);
		const TypeParam max_s = 10000.0;

		protected:
		std::unique_ptr<Scheme<TypeParam>> scheme;

		SchemeTest()
		{
			this->scheme = std::make_unique<Scheme<TypeParam>>(beta);
			this->scheme->set_max_s(max_s);
		}
	};

	using testing::Types;

	typedef Types<float, double> ValidVectorTypes;
	TYPED_TEST_SUITE(SchemeTest, ValidVectorTypes);

	TYPED_TEST(SchemeTest, Initialization)
	{
		SUCCEED();
	}

	TYPED_TEST(SchemeTest, InvalidMaxS)
	{
		EXPECT_THROW({ this->scheme->set_max_s(-1.0); }, Exception);
	}

	TYPED_TEST(SchemeTest, EncryptDecrypt)
	{
		const auto runs	 = 1000;
		const auto error = 1.0;

		auto get_random_value = [](auto min, auto max)
		{
			TypeParam f = (TypeParam)rand() / RAND_MAX;
			return min + f * (max - min);
		};

		for (auto &&dimensions : {1, 2, 3, 5, 10, 25, 50})
		{
			for (auto run = 0; run < runs; run++)
			{
				auto key = this->scheme->keygen();

				std::vector<TypeParam> message;
				message.resize(dimensions);
				for (auto i = 0; i < dimensions; i++)
				{
					message[i] = get_random_value(-1000.0, +1000.0);
				}

				std::vector<TypeParam> ciphertext;
				ciphertext.resize(dimensions);
				auto nonce = this->scheme->encrypt(key, TO_ARRAY(message), dimensions, TO_ARRAY(ciphertext));

				std::vector<TypeParam> decrypted;
				decrypted.resize(dimensions);
				this->scheme->decrypt(key, TO_ARRAY(ciphertext), dimensions, nonce, TO_ARRAY(decrypted));

				for (auto i = 0; i < dimensions; i++)
				{
					ASSERT_NEAR(message[i], decrypted[i], error);
				}
			}
		}
	}

	TYPED_TEST(SchemeTest, PreserveDistanceComparison)
	{
		const auto runs = 1000;

		auto get_random_vector = [](auto dimensions, auto min, auto max)
		{
			std::vector<TypeParam> result;
			result.resize(dimensions);
			for (auto i = 0; i < dimensions; i++)
			{
				result[i] = min + ((TypeParam)rand() / RAND_MAX) * (max - min);
			}
			return result;
		};

		auto check_implication = [beta = this->beta](TypeParam d1, TypeParam d2, TypeParam df1, TypeParam df2)
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
				auto key = this->scheme->keygen();

				auto x = get_random_vector(dimensions, -1000.0, +1000.0);
				auto y = get_random_vector(dimensions, -1000.0, +1000.0);
				auto z = get_random_vector(dimensions, -1000.0, +1000.0);

				std::vector<TypeParam> f_x, f_y, f_z;
				f_x.resize(dimensions);
				f_y.resize(dimensions);
				f_z.resize(dimensions);

				this->scheme->encrypt(key, TO_ARRAY(x), dimensions, TO_ARRAY(f_x));
				this->scheme->encrypt(key, TO_ARRAY(y), dimensions, TO_ARRAY(f_y));
				this->scheme->encrypt(key, TO_ARRAY(z), dimensions, TO_ARRAY(f_z));

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
