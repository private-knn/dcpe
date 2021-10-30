#include "scheme.hpp"

#include "utility.hpp"

#include <cmath>
#include <numeric>

namespace DCPE
{
	using namespace std;

	Scheme::Scheme(number beta, number max_s) :
		beta(beta),
		max_s(max_s) {}

	key Scheme::keygen()
	{
		auto K = hmac_256_keygen();
		auto s = get_ramdom_number(max_s - 1) + 1;

		return {K.first, s};
	}

	pair<vector<double>, bytes> Scheme::encrypt(key key, vector<double> message)
	{
		const auto dimensions = message.size();

		auto nonce = get_random_bytes(l);

		auto lambda_m = compute_lambda_m(key, nonce, dimensions);

		vector<double> c;
		c.resize(dimensions);
		for (auto i = 0; i < dimensions; i++)
		{
			c[i] = message[i] * key.second + lambda_m[i];
		}

		return {c, nonce};
	}

	vector<double> Scheme::decrypt(key key, pair<vector<double>, bytes> ciphertext)
	{
		const auto dimensions = ciphertext.first.size();

		auto lambda_m = compute_lambda_m(key, ciphertext.second, dimensions);

		vector<double> message;
		message.resize(dimensions);
		for (auto i = 0; i < dimensions; i++)
		{
			message[i] = (ciphertext.first[i] - lambda_m[i]) / key.second;
		}

		return message;
	}

	vector<double> Scheme::compute_lambda_m(key key, bytes nonce, size_t dimensions)
	{
		auto tape = hmac_256_sign(key.first, nonce);

		auto u = sample_normal_multivariate_identity(0, dimensions, bytes_to_number(nonce.begin()));

		auto x_prime = sample_uniform(0, key.second * beta / 4, bytes_to_number(nonce.begin() + sizeof(number) / sizeof(bytes)));

		auto x = pow(x_prime, 1.0 / dimensions);

		vector<double> lambda_m;
		lambda_m.resize(dimensions);
		auto norm = sqrt(inner_product(u.begin(), u.end(), u.begin(), 0.0));
		for (auto i = 0; i < dimensions; i++)
		{
			lambda_m[i] = u[i] * x / norm;
		}

		return lambda_m;
	}
}
