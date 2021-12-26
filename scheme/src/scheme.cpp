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
	key<VALUE_T> Scheme<VALUE_T>::keygen()
	{
		return {
			get_ramdom_ull(ULLONG_MAX),
			get_ramdom_ull(ULLONG_MAX),
			sample_uniform<VALUE_T>(0.000001, max_s, get_ramdom_ull(ULLONG_MAX))};
	}

	template <typename VALUE_T>
	std::pair<ull, ull> Scheme<VALUE_T>::encrypt(key<VALUE_T>& key, const VALUE_T* message, int dimensions, VALUE_T* ciphertext)
	{
		std::pair nonce = {get_ramdom_ull(), get_ramdom_ull()};

		auto lambda_m = compute_lambda_m(key, nonce, dimensions);

		for (auto i = 0; i < dimensions; i++)
		{
			ciphertext[i] = message[i] * std::get<2>(key) + lambda_m[i];
		}

		return nonce;
	}

	template <typename VALUE_T>
	void Scheme<VALUE_T>::decrypt(key<VALUE_T>& key, const VALUE_T* ciphertext, int dimensions, std::pair<ull, ull>& nonce, VALUE_T* message)
	{
		auto lambda_m = compute_lambda_m(key, nonce, dimensions);

		for (auto i = 0; i < dimensions; i++)
		{
			message[i] = (ciphertext[i] - lambda_m[i]) / std::get<2>(key);
		}
	}

	template <typename VALUE_T>
	std::vector<VALUE_T> Scheme<VALUE_T>::compute_lambda_m(key<VALUE_T>& key, std::pair<ull, ull>& nonce, int dimensions)
	{
		auto radius = (std::get<2>(key) / 4) * beta;

		auto u = sample_normal_multivariate_identity<VALUE_T>(0.0, dimensions, std::get<0>(key) ^ nonce.first);

		auto x_prime = sample_uniform<VALUE_T>(0.0, 1.0, std::get<1>(key) ^ nonce.second);

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

	template <typename VALUE_T>
	void Scheme<VALUE_T>::set_max_s(VALUE_T max_s)
	{
		if (max_s <= 0.0)
		{
			throw Exception(boost::format("Invalid value of max_s: %f") % max_s);
		}

		this->max_s = max_s;
	}

	template class Scheme<float>;
	template class Scheme<double>;
}
