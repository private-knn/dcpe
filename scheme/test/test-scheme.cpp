#include "definitions.h"
#include "scheme.hpp"

#include "gtest/gtest.h"

using namespace std;

namespace DCPE
{
	class SchemeTest : public ::testing::Test
	{
		public:
		inline static const number beta	 = 3;
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
		const auto runs = 10;
		const auto error = 1.0;

		for (auto &&dimensions : {1, 2, 3, 5, 10, 25, 50})
		{
			for (auto run = 0; run < runs; run++)
			{
				auto key = scheme->keygen();

				vector<double> message;
				message.resize(dimensions);
				for (auto i = 0; i < dimensions; i++)
				{
					message[i] = (double)rand() / RAND_MAX;
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
}

int main(int argc, char **argv)
{
	srand(TEST_SEED);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
