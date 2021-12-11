#include "definitions.h"
#include "scheme.hpp"

#include <benchmark/benchmark.h>

namespace DCPE
{
	// change to run all tests from different seed
	const auto TEST_SEED = 0x13;

	template <typename VALUE_T>
	class SchemeBenchmark : public ::benchmark::Fixture
	{
		public:
		const VALUE_T beta = 1.0 * (1 << 10);

		void SetUp(const ::benchmark::State& state)
		{
			srand(TEST_SEED);
		}

		protected:
		std::unique_ptr<Scheme<VALUE_T>> scheme = std::make_unique<Scheme<VALUE_T>>(beta);
	};

#define B_KeyGen(type)                                                \
	BENCHMARK_TEMPLATE_DEFINE_F(SchemeBenchmark, KeyGen_##type, type) \
	(benchmark::State & state)                                        \
	{                                                                 \
		for (auto _ : state)                                          \
		{                                                             \
			benchmark::DoNotOptimize(scheme->keygen());               \
		}                                                             \
	}

	B_KeyGen(float);
	B_KeyGen(double);

#define B_Encrypt(type)                                                                                          \
	BENCHMARK_TEMPLATE_DEFINE_F(SchemeBenchmark, Encrypt_##type, type)                                           \
	(benchmark::State & state)                                                                                   \
	{                                                                                                            \
		auto key = scheme->keygen();                                                                             \
                                                                                                                 \
		auto dimensions = state.range(0);                                                                        \
                                                                                                                 \
		std::vector<type> message;                                                                               \
		message.resize(dimensions);                                                                              \
		for (auto i = 0; i < dimensions; i++)                                                                    \
		{                                                                                                        \
			message[i] = (type)rand() / RAND_MAX;                                                                \
		}                                                                                                        \
                                                                                                                 \
		std::vector<type> ciphertext;                                                                            \
		ciphertext.resize(dimensions);                                                                           \
                                                                                                                 \
		for (auto _ : state)                                                                                     \
		{                                                                                                        \
			benchmark::DoNotOptimize(scheme->encrypt(key, TO_ARRAY(message), dimensions, TO_ARRAY(ciphertext))); \
		}                                                                                                        \
	}

	B_Encrypt(float);
	B_Encrypt(double);

#define B_Decrypt(type)                                                                         \
	BENCHMARK_TEMPLATE_DEFINE_F(SchemeBenchmark, Decrypt_##type, type)                          \
	(benchmark::State & state)                                                                  \
	{                                                                                           \
		auto key = scheme->keygen();                                                            \
                                                                                                \
		auto dimensions = state.range(0);                                                       \
                                                                                                \
		std::vector<type> message;                                                              \
		message.resize(dimensions);                                                             \
		for (auto i = 0; i < dimensions; i++)                                                   \
		{                                                                                       \
			message[i] = (type)rand() / RAND_MAX;                                               \
		}                                                                                       \
                                                                                                \
		std::vector<type> ciphertext;                                                           \
		ciphertext.resize(dimensions);                                                          \
                                                                                                \
		auto nonce = scheme->encrypt(key, TO_ARRAY(message), dimensions, TO_ARRAY(ciphertext)); \
                                                                                                \
		for (auto _ : state)                                                                    \
		{                                                                                       \
			scheme->decrypt(key, TO_ARRAY(ciphertext), dimensions, nonce, TO_ARRAY(message));   \
		}                                                                                       \
	}

	B_Decrypt(float);
	B_Decrypt(double);

#define R_KeyGen(type)                                   \
	BENCHMARK_REGISTER_F(SchemeBenchmark, KeyGen_##type) \
		->Iterations(1 << 20)                            \
		->Unit(benchmark::kMicrosecond);

	R_KeyGen(float);
	R_KeyGen(double);

#define R_Encrypt(type)                                   \
	BENCHMARK_REGISTER_F(SchemeBenchmark, Encrypt_##type) \
		->Args({1})                                       \
		->Args({2})                                       \
		->Args({3})                                       \
		->Args({5})                                       \
		->Args({10})                                      \
		->Args({25})                                      \
		->Args({50})                                      \
                                                          \
		->Iterations(1 << 15)                             \
		->Unit(benchmark::kMicrosecond);

	R_Encrypt(float);
	R_Encrypt(double);

#define R_Decrypt(type)                                   \
	BENCHMARK_REGISTER_F(SchemeBenchmark, Decrypt_##type) \
		->Args({1})                                       \
		->Args({2})                                       \
		->Args({3})                                       \
		->Args({5})                                       \
		->Args({10})                                      \
		->Args({25})                                      \
		->Args({50})                                      \
                                                          \
		->Iterations(1 << 15)                             \
		->Unit(benchmark::kMicrosecond);

	R_Decrypt(float);
	R_Decrypt(double);

}
BENCHMARK_MAIN();
