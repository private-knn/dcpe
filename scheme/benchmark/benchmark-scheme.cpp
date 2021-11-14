#include "definitions.h"
#include "scheme.hpp"

#include <benchmark/benchmark.h>

using namespace std;

namespace DCPE
{
	class SchemeBenchmark : public ::benchmark::Fixture
	{
		public:
		inline static const number beta	 = 1 << 20;

		void SetUp(const ::benchmark::State& state)
		{
			srand(TEST_SEED);
		}

		protected:
		unique_ptr<Scheme> scheme = make_unique<Scheme>(beta);
	};

	BENCHMARK_DEFINE_F(SchemeBenchmark, KeyGen)
	(benchmark::State& state)
	{
		for (auto _ : state)
		{
			benchmark::DoNotOptimize(scheme->keygen());
		}
	}

	BENCHMARK_DEFINE_F(SchemeBenchmark, Encrypt)
	(benchmark::State& state)
	{
		auto key = scheme->keygen();

		auto dimensions = state.range(0);

		vector<VALUE_T> message;
		message.resize(dimensions);
		for (auto i = 0; i < dimensions; i++)
		{
			message[i] = (VALUE_T)rand() / RAND_MAX;
		}

		vector<VALUE_T> ciphertext;
		ciphertext.resize(dimensions);

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(scheme->encrypt(key, TO_ARRAY(message), dimensions, TO_ARRAY(ciphertext)));
		}
	}

	BENCHMARK_DEFINE_F(SchemeBenchmark, Decrypt)
	(benchmark::State& state)
	{
		auto key = scheme->keygen();

		auto dimensions = state.range(0);

		vector<VALUE_T> message;
		message.resize(dimensions);
		for (auto i = 0; i < dimensions; i++)
		{
			message[i] = (VALUE_T)rand() / RAND_MAX;
		}

		vector<VALUE_T> ciphertext;
		ciphertext.resize(dimensions);

		auto nonce = scheme->encrypt(key, TO_ARRAY(message), dimensions, TO_ARRAY(ciphertext));

		for (auto _ : state)
		{
			scheme->decrypt(key, TO_ARRAY(ciphertext), dimensions, nonce, TO_ARRAY(message));
		}
	}

	BENCHMARK_REGISTER_F(SchemeBenchmark, KeyGen)
		->Iterations(1 << 20)
		->Unit(benchmark::kMicrosecond);

	BENCHMARK_REGISTER_F(SchemeBenchmark, Encrypt)
		->Args({1})
		->Args({2})
		->Args({3})
		->Args({5})
		->Args({10})
		->Args({25})
		->Args({50})

		->Iterations(1 << 15)
		->Unit(benchmark::kMicrosecond);

	BENCHMARK_REGISTER_F(SchemeBenchmark, Decrypt)
		->Args({1})
		->Args({2})
		->Args({3})
		->Args({5})
		->Args({10})
		->Args({25})
		->Args({50})

		->Iterations(1 << 15)
		->Unit(benchmark::kMicrosecond);
}
BENCHMARK_MAIN();
