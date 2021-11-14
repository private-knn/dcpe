#include "utility.hpp"

#include <boost/generator_iterator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <iomanip>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <random>
#include <vector>

#ifndef TESTING
#define CHECK_OPENSSL_ERROR(condition, message)                                        \
	if (condition)                                                                     \
	{                                                                                  \
		throw Exception(boost::format("%s, error 0x%lx") % message % ERR_get_error()); \
	}
#else
#define CHECK_OPENSSL_ERROR(condition, message)
#endif

namespace DCPE
{
	using namespace std;

	/**
	 * @brief can be changed to boost::mt19937 or boost::ecuyer1988.
	 * boost::minstd_rand is NOT crypto-safe.
	 *
	 */
	typedef boost::random::mt19937_64 base_generator_type;

	bytes get_random_bytes(const number size)
	{
		uchar material[size];
#if defined(TESTING) || defined(DEBUG)
		for (number i = 0; i < size; i++)
		{
			material[i] = (uchar)rand();
		}
#else
		RAND_bytes(material, size);
#endif
		return bytes(material, material + size);
	}

	number get_ramdom_number(const number max)
	{
		number material[1];
#if defined(TESTING) || defined(DEBUG)
		auto int_material = (int *)material;
		int_material[0]	  = rand();
		int_material[1]	  = rand();
#else
		RAND_bytes((uchar *)material, sizeof(number));
#endif
		return material[0] % max;
	}

	number bytes_to_number(bytes::iterator first_byte)
	{
		number material[1];
		auto byte_material = (byte *)material;
		for (auto i = 0; i < sizeof(number) / sizeof(byte); i++)
		{
			byte_material[0] = *first_byte;
			first_byte++;
		}

		return material[0];
	}

	VALUE_T sample_uniform(const VALUE_T min, const VALUE_T max, const number seed)
	{
		base_generator_type generator(seed);

		boost::uniform_real<> distribution(min, max);
		boost::variate_generator<base_generator_type &, boost::uniform_real<>> sampler(generator, distribution);

		return sampler();
	}

	vector<VALUE_T> sample_normal_series(const VALUE_T mean, const VALUE_T variance, const number seed, const number count)
	{
		base_generator_type generator(seed);

		boost::normal_distribution<> distribution(mean, variance);
		boost::variate_generator<base_generator_type &, boost::normal_distribution<>> sampler(generator, distribution);

		vector<VALUE_T> samples;
		samples.resize(count);

		for (auto i = 0; i < count; i++)
		{
			samples[i] = sampler();
		}

		return samples;
	}

	vector<VALUE_T> sample_normal_multivariate_identity(const VALUE_T mean, const number dimensions, const number seed)
	{
		auto samples = sample_normal_series(0.0, 1.0, seed, dimensions);

		// optimization
		if (mean != 0)
		{
			return samples;
		}

		for (auto i = 0; i < dimensions; i++)
		{
			samples[i] += mean;
		}

		return samples;
	}

	prf_keys hmac_256_keygen(bytes hash_key)
	{
		auto md	  = EVP_get_digestbyname("SHA256");
		auto size = EVP_MD_size(md);

		if (hash_key.size() == 0)
		{
			hash_key = get_random_bytes(size);
		}
		else if (hash_key.size() != size)
		{
			throw Exception(boost::format("hmac_256_keygen: wrong hash key size %d, must be 0 or %d") % hash_key.size() % size);
		}

		auto signing_key   = EVP_PKEY_new_raw_private_key(EVP_PKEY_HMAC, NULL, TO_ARRAY(hash_key), size);
		auto verifying_key = EVP_PKEY_new_raw_private_key(EVP_PKEY_HMAC, NULL, TO_ARRAY(hash_key), size);

		return {signing_key, verifying_key};
	}

	bytes hmac_256_sign(prf_key key, bytes message)
	{
		EVP_MD_CTX *context = NULL;
		byte *signature		= NULL;
		size_t requested	= 0;
		size_t signature_length;

		signature_length = 0;

#ifndef TESTING
		try
		{
#endif
			context = EVP_MD_CTX_new();
			CHECK_OPENSSL_ERROR(context == NULL, "EVP_MD_CTX_create failed")

			const auto *md = EVP_get_digestbyname("SHA256");
			CHECK_OPENSSL_ERROR(md == NULL, "EVP_get_digestbyname failed")

			auto return_code = EVP_DigestInit_ex(context, md, NULL);
			CHECK_OPENSSL_ERROR(return_code != 1, "EVP_DigestInit_ex failed")

			return_code = EVP_DigestSignInit(context, NULL, md, NULL, key);
			CHECK_OPENSSL_ERROR(return_code != 1, "EVP_DigestSignInit failed")

			return_code = EVP_DigestSignUpdate(context, TO_ARRAY(message), message.size());
			CHECK_OPENSSL_ERROR(return_code != 1, "EVP_DigestSignUpdate failed")

			return_code = EVP_DigestSignFinal(context, NULL, &requested);
			CHECK_OPENSSL_ERROR(return_code != 1, "EVP_DigestSignFinal failed (1)")
			CHECK_OPENSSL_ERROR(!(requested > 0), "EVP_DigestSignFinal failed (2)")

			signature = static_cast<byte *>(OPENSSL_malloc(requested));
			CHECK_OPENSSL_ERROR(signature == NULL, "OPENSSL_malloc failed")

			signature_length = requested;
			return_code		 = EVP_DigestSignFinal(context, signature, &signature_length);
			CHECK_OPENSSL_ERROR(return_code != 1, "EVP_DigestSignFinal failed (3)")
			CHECK_OPENSSL_ERROR(requested != signature_length, "EVP_DigestSignFinal failed (4)")
#ifndef TESTING
		}
		catch (const std::exception &e)
		{
			EVP_MD_CTX_free(context);
			if (signature != NULL)
			{
				OPENSSL_free(signature);
				signature = NULL;
			}

			throw e;
		}
#endif

		if (context)
		{
			EVP_MD_CTX_destroy(context);
			context = NULL;
		}

		bytes result(signature, signature + signature_length);
		return result;
	}

	bool hmac_256_verify(prf_key key, bytes message, bytes signature)
	{
		EVP_MD_CTX *context = NULL;
		auto result			= false;

#ifndef TESTING
		try
		{
#endif
			context = EVP_MD_CTX_new();
			CHECK_OPENSSL_ERROR(context == NULL, "EVP_MD_CTX_create failed")

			const auto *md = EVP_get_digestbyname("SHA256");
			CHECK_OPENSSL_ERROR(md == NULL, "EVP_get_digestbyname failed")

			auto return_code = EVP_DigestInit_ex(context, md, NULL);
			CHECK_OPENSSL_ERROR(return_code != 1, "EVP_DigestInit_ex failed")

			return_code = EVP_DigestSignInit(context, NULL, md, NULL, key);
			CHECK_OPENSSL_ERROR(return_code != 1, "EVP_DigestSignInit failed")

			return_code = EVP_DigestSignUpdate(context, TO_ARRAY(message), message.size());
			CHECK_OPENSSL_ERROR(return_code != 1, "EVP_DigestSignUpdate failed")

			byte buffer[EVP_MAX_MD_SIZE];
			auto buffer_size = sizeof(buffer);

			return_code = EVP_DigestSignFinal(context, buffer, &buffer_size);
			CHECK_OPENSSL_ERROR(return_code != 1, "EVP_DigestVerifyFinal failed")
			CHECK_OPENSSL_ERROR(!(buffer_size > 0), "EVP_DigestSignFinal failed (2)")

			const auto memory_length = (signature.size() < buffer_size ? signature.size() : buffer_size);
			result					 = 0 == CRYPTO_memcmp(TO_ARRAY(signature), buffer, memory_length);
#ifndef TESTING
		}
		catch (const std::exception &e)
		{
			EVP_MD_CTX_free(context);

			throw e;
		}
#endif

		if (context)
		{
			EVP_MD_CTX_destroy(context);
			context = NULL;
		}

		return result;
	}

	VALUE_T distance(vector<VALUE_T> first, vector<VALUE_T> second)
	{
		if (first.size() != second.size())
		{
			throw Exception(boost::format("distance: vectors of different lengths (%d vs %d)") % first.size() % second.size());
		}

		auto result = 0.0;
		for (size_t i = 0; i < first.size(); i++)
		{
			result += (first[i] - second[i]) * (first[i] - second[i]);
		}

		return sqrt(result);
	}

	string key_to_string(EVP_PKEY *key)
	{
		auto len = 10512;

		auto *bio = BIO_new(BIO_s_mem());
		auto pem  = (char *)malloc(len + 1);

		// auto res = PEM_write_bio_PrivateKey(
		// 	bio,  /* BIO to write the private key to */
		// 	key,  /* EVP_PKEY structure */
		// 	NULL, /* default cipher for encrypting the key on disk */
		// 	NULL, /* passphrase required for decrypting the key on disk */
		// 	0,	  /* length of the passphrase string */
		// 	NULL, /* callback for requesting a password */
		// 	NULL  /* data to pass to the callback */
		// );
		auto res = PEM_write_bio_PUBKEY(
			bio,  /* BIO to write the private key to */
			key  /* EVP_PKEY structure */
		);
		if (!res)
		{
			throw Exception("Error");
		}


		// memset(pem, 0, bio->num_write + 1);
		memset(pem, 0, len + 1);

		return string(pem);
	}
}
