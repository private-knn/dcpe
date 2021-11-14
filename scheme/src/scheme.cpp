#include "scheme.hpp"

#include "utility.hpp"

#include <cmath>
#include <numeric>

namespace DCPE
{
	using namespace std;

	Scheme::Scheme(VALUE_T beta) :
		beta(beta) {}

	key Scheme::keygen()
	{
		return {
			get_ramdom_number(ULLONG_MAX),
			get_ramdom_number(ULLONG_MAX),
			get_ramdom_number(max_s - 1) + 1};
	}

	pair<number, number> Scheme::encrypt(key& key, const VALUE_T* message, size_t dimensions, VALUE_T* ciphertext)
	{
		pair nonce = {get_ramdom_number(), get_ramdom_number()};

		auto lambda_m = compute_lambda_m(key, nonce, dimensions);

		for (auto i = 0; i < dimensions; i++)
		{
			ciphertext[i] = message[i] * get<2>(key) + lambda_m[i];
		}

		return nonce;
	}

	void Scheme::decrypt(key& key, const VALUE_T* ciphertext, size_t dimensions, pair<number, number>& nonce, VALUE_T* message)
	{
		auto lambda_m = compute_lambda_m(key, nonce, dimensions);

		for (auto i = 0; i < dimensions; i++)
		{
			message[i] = (ciphertext[i] - lambda_m[i]) / get<2>(key);
		}
	}

	vector<VALUE_T> Scheme::compute_lambda_m(key& key, pair<number, number>& nonce, size_t dimensions)
	{
		auto u = sample_normal_multivariate_identity(0, dimensions, get<0>(key) ^ nonce.first);

		auto x_prime = sample_uniform(0, (get<2>(key) / 4) * beta, get<1>(key) ^ nonce.second);

		auto x = pow(x_prime, 1.0 / dimensions);

		vector<VALUE_T> lambda_m;
		lambda_m.resize(dimensions);
		auto norm = sqrt(inner_product(u.begin(), u.end(), u.begin(), 0.0));
		for (auto i = 0; i < dimensions; i++)
		{
			lambda_m[i] = u[i] * x / norm;
		}

		return lambda_m;
	}
}
