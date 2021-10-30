#pragma once

#include "definitions.h"

namespace DCPE
{
	using namespace std;

	/**
	 * @brief generate an array of bytes pseudorandomly
	 *
	 * \note
	 * It uses OpenSSL PRG unless TESTING macro is defined.
	 * If it is, C++ standard rand() is used (easy for testing and debugging).
	 *
	 * @param size the number of bytes to generate
	 * @return bytes the resulting bytes
	 */
	bytes get_random_bytes(const number size);

	/**
	 * @brief returns a pseudorandom number
	 *
	 * @param max the non-inclusive max of the range (min is inclusive 0).
	 * @return number the resulting number
	 */
	number get_ramdom_number(const number max);

	/**
	 * @brief a helper that composes a value of type "number" from a series of bytes
	 *
	 * @param first_byte an iterator at the element of byte vector from which to start collecting bytes
	 * @return number a resulting concatentation of bytes represented as a number
	 */
	number bytes_to_number(bytes::iterator first_byte);

	/**
	 * @brief Samples a real uniform value [min, max] using random coins from the seed
	 *
	 * @param min the left endpoint of the distribution (inclusive)
	 * @param max the right endpoint of the distribution (inclusive)
	 * @param seed the seed value to supply to pseudo-random generator for coins
	 * @return double the sample value
	 */
	double sample_uniform(const double min, const double max, const number seed);

	/**
	 * @brief Samples a series of normal values using random coins from the seed
	 *
	 * @param mean the mu parameter of the distribution
	 * @param variance the sigma squared parameter of the distribution
	 * @param seed the seed value to supply to pseudo-random generator for coins
	 * @param count the number of samples to return
	 * @return vector<double> the sample values
	 */
	vector<double> sample_normal_series(const double mean, const double variance, const number seed, const number count);

	/**
	 * @brief Samples a value from a Multivariate Normal distribution with variance identity matrix.
	 *
	 * \note
	 * Because the variance is identity, the algorithm is essentially the same as sample_normal_series.
	 *
	 * @param mean the mu parameter of the distribution
	 * @param dimensions the number of dimensions in the distribution
	 * @param seed the seed value to supply to pseudo-random generator for coins
	 * @return vector<double> the sample value
	 */
	vector<double> sample_normal_multivariate_identity(const double mean, const number dimensions, const number seed);

	/**
	 * @brief Generates a pair of keys to be used in HMAC sign and verify routines
	 *
	 * @param hash_key an optional base hash key to be used in key derivation.
	 * Use for debugging and testing.
	 * @return keys sign and verify keys
	 */
	prf_keys hmac_256_keygen(bytes hash_key = bytes());

	/**
	 * @brief produces an HMAC signature of the given message under given sign key
	 *
	 * @param key sign key produced in hmac_256_keygen
	 * @param message an input to sign
	 * @return bytes the signature of the input
	 */
	bytes hmac_256_sign(prf_key key, bytes message);

	/**
	 * @brief checks if a signature verifies for given message under given verification key
	 *
	 * @param key verification key produced in hmac_256_keygen
	 * @param message an input that was sign
	 * @param signature a signature produced in hmac_256_sign
	 * @return true if signature verifies
	 * @return false otherwise
	 */
	bool hmac_256_verify(prf_key key, bytes message, bytes signature);

	/**
	 * @brief computes Euclidean distance between two vectors
	 *
	 * @param first first vector argument
	 * @param second second vector argument
	 * @return double the Euclidean distance
	 */
	double distance(vector<double> first, vector<double> second);
}
