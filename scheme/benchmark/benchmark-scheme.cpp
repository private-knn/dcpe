#include "definitions.h"
#include "scheme.hpp"

#include <benchmark/benchmark.h>

using namespace std;

namespace DCPE
{
	class SchemeBenchmark : public ::benchmark::Fixture
	{
		public:
		inline static const number beta	 = 3;
		inline static const number max_s = 10000;

		void SetUp(const ::benchmark::State& state)
		{
			srand(TEST_SEED);
		}

		protected:
		unique_ptr<Scheme> scheme = make_unique<Scheme>(beta, max_s);
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

		vector<double> message;
		message.resize(dimensions);
		for (auto i = 0; i < dimensions; i++)
		{
			message[i] = (double)rand() / RAND_MAX;
		}

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(scheme->encrypt(key, message));
		}
	}

	BENCHMARK_DEFINE_F(SchemeBenchmark, Decrypt)
	(benchmark::State& state)
	{
		auto key = scheme->keygen();

		auto dimensions = state.range(0);

		vector<double> message;
		message.resize(dimensions);
		for (auto i = 0; i < dimensions; i++)
		{
			message[i] = (double)rand() / RAND_MAX;
		}
		auto ciphertext = scheme->encrypt(key, message);

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(scheme->decrypt(key, ciphertext));
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
