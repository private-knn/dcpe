#pragma once

#include "definitions.h"

namespace DCPE
{
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
	bytes get_random_bytes(const int size);

	/**
	 * @brief returns a pseudorandom number
	 *
	 * @param max the non-inclusive max of the range (min is inclusive 0).
	 * @return number the resulting number
	 */
	ull get_ramdom_ull(const ull max = ULLONG_MAX);

	/**
	 * @brief Samples a real uniform value [min, max] using random coins from the seed
	 *
	 * @param min the left endpoint of the distribution (inclusive)
	 * @param max the right endpoint of the distribution (inclusive)
	 * @param seed the seed value to supply to pseudo-random generator for coins
	 * @return VALUE_T the sample value
	 */
	template <typename VALUE_T>
	VALUE_T sample_uniform(const VALUE_T min, const VALUE_T max, const ull seed);

	/**
	 * @brief Samples a series of normal values using random coins from the seed
	 *
	 * @param mean the mu parameter of the distribution
	 * @param variance the sigma squared parameter of the distribution
	 * @param seed the seed value to supply to pseudo-random generator for coins
	 * @param count the number of samples to return
	 * @return vector<VALUE_T> the sample values
	 */
	template <typename VALUE_T>
	std::vector<VALUE_T> sample_normal_series(const VALUE_T mean, const VALUE_T variance, const ull seed, const int count);

	/**
	 * @brief Samples a value from a Multivariate Normal distribution with variance identity matrix.
	 *
	 * \note
	 * Because the variance is identity, the algorithm is essentially the same as sample_normal_series.
	 *
	 * @param mean the mu parameter of the distribution
	 * @param dimensions the number of dimensions in the distribution
	 * @param seed the seed value to supply to pseudo-random generator for coins
	 * @return vector<VALUE_T> the sample value
	 */
	template <typename VALUE_T>
	std::vector<VALUE_T> sample_normal_multivariate_identity(const VALUE_T mean, const int dimensions, const ull seed);

	/**
	 * @brief computes Euclidean distance between two vectors
	 *
	 * @param first first vector argument
	 * @param second second vector argument
	 * @return VALUE_T the Euclidean distance
	 */
	template <typename VALUE_T>
	VALUE_T distance(std::vector<VALUE_T> first, std::vector<VALUE_T> second);
}
