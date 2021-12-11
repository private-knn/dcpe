#include "scheme.hpp"

#include "utility.hpp"

#include <cmath>
#include <numeric>

namespace DCPE
{
	template <typename VALUE_T>
	Scheme<VALUE_T>::Scheme(VALUE_T beta) :
		beta(beta) {}

	template <typename VALUE_T>
	key Scheme<VALUE_T>::keygen()
	{
		return {
			get_ramdom_ull(ULLONG_MAX),
			get_ramdom_ull(ULLONG_MAX),
			get_ramdom_ull(max_s - 1) + 1}; // TODO(dmytro): must be uniform VALUE_T
	}

	template <typename VALUE_T>
	std::pair<ull, ull> Scheme<VALUE_T>::encrypt(key& key, const VALUE_T* message, int dimensions, VALUE_T* ciphertext)
	{
		std::pair nonce = {get_ramdom_ull(), get_ramdom_ull()};

		auto lambda_m = compute_lambda_m(key, nonce, dimensions);

		for (auto i = 0; i < dimensions; i++)
		{
			ciphertext[i] = message[i] * get<2>(key) + lambda_m[i];
		}

		return nonce;
	}

	template <typename VALUE_T>
	void Scheme<VALUE_T>::decrypt(key& key, const VALUE_T* ciphertext, int dimensions, std::pair<ull, ull>& nonce, VALUE_T* message)
	{
		auto lambda_m = compute_lambda_m(key, nonce, dimensions);

		for (auto i = 0; i < dimensions; i++)
		{
			message[i] = (ciphertext[i] - lambda_m[i]) / get<2>(key);
		}
	}

	template <typename VALUE_T>
	std::vector<VALUE_T> Scheme<VALUE_T>::compute_lambda_m(key& key, std::pair<ull, ull>& nonce, int dimensions)
	{
		auto radius = (get<2>(key) / 4) * beta;

		auto u = sample_normal_multivariate_identity<VALUE_T>(0.0, dimensions, get<0>(key) ^ nonce.first);

		auto x_prime = sample_uniform<VALUE_T>(0.0, 1.0, get<1>(key) ^ nonce.second);

		auto x = radius * pow(x_prime, 1.0 / dimensions);

		std::vector<VALUE_T> lambda_m;
		lambda_m.resize(dimensions);
		auto norm = sqrt(inner_product(u.begin(), u.end(), u.begin(), 0.0));
		for (auto i = 0; i < dimensions; i++)
		{
			lambda_m[i] = u[i] * x / norm;
		}

		return lambda_m;
	}

	template class Scheme<float>;
	template class Scheme<double>;
}
