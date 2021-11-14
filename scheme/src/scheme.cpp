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

	key Scheme::keygen(bytes hash_key, number s)
	{
		auto K = hmac_256_keygen(hash_key);
		if (s == -1)
		{
			s = get_ramdom_number(max_s - 1) + 1;
		}

		return {K.first, s};
	}

	bytes Scheme::encrypt(key& key, const VALUE_T* message, size_t dimensions, VALUE_T* ciphertext)
	{
		auto nonce = get_random_bytes(l);

		auto lambda_m = compute_lambda_m(key, nonce, dimensions);

		for (auto i = 0; i < dimensions; i++)
		{
			ciphertext[i] = message[i] * key.second + lambda_m[i];
		}

		return nonce;
	}

	void Scheme::decrypt(key& key, const VALUE_T* ciphertext, size_t dimensions, bytes& nonce, VALUE_T* message)
	{
		auto lambda_m = compute_lambda_m(key, nonce, dimensions);

		for (auto i = 0; i < dimensions; i++)
		{
			message[i] = (ciphertext[i] - lambda_m[i]) / key.second;
		}
	}

	vector<VALUE_T> Scheme::compute_lambda_m(key& key, bytes& nonce, size_t dimensions)
	{
		auto u = sample_normal_multivariate_identity(0, dimensions, bytes_to_number(nonce.begin()));

		auto x_prime = sample_uniform(0, key.second * beta / 4, bytes_to_number(nonce.begin() + sizeof(number) / sizeof(bytes)));

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

	size_t Scheme::hash_key_size()
	{
		auto md = EVP_get_digestbyname("SHA256");
		return EVP_MD_size(md);
	}
}
