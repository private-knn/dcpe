#include "definitions.h"
#include "utility.hpp"

#include <benchmark/benchmark.h>

using namespace std;

namespace DCPE
{
	class UtilityBenchmark : public ::benchmark::Fixture
	{
		void SetUp(const ::benchmark::State& state)
		{
			srand(TEST_SEED);
		}
	};

	BENCHMARK_DEFINE_F(UtilityBenchmark, Random)
	(benchmark::State& state)
	{
		for (auto _ : state)
		{
			benchmark::DoNotOptimize(get_random_bytes(64));
		}
	}

	BENCHMARK_DEFINE_F(UtilityBenchmark, Uniform)
	(benchmark::State& state)
	{
		auto i = 0uLL;
		for (auto _ : state)
		{
			sample_uniform(0, 100, i);
		}
	}

	BENCHMARK_DEFINE_F(UtilityBenchmark, Normal)
	(benchmark::State& state)
	{
		auto count = (number)state.range(0);

		auto i = 0uLL;
		for (auto _ : state)
		{
			sample_normal_series(0, 100, i, count);
		}
	}

	BENCHMARK_DEFINE_F(UtilityBenchmark, HMACSign)
	(benchmark::State& state)
	{
		auto message = get_random_bytes(100);
		auto keys	 = hmac_256_keygen();

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(hmac_256_sign(keys.first, message));
		}
	}

	BENCHMARK_DEFINE_F(UtilityBenchmark, HMACVerify)
	(benchmark::State& state)
	{
		auto message   = get_random_bytes(100);
		auto keys	   = hmac_256_keygen();
		auto signature = hmac_256_sign(keys.first, message);

		for (auto _ : state)
		{
			benchmark::DoNotOptimize(hmac_256_verify(keys.second, message, signature));
		}
	}

	BENCHMARK_REGISTER_F(UtilityBenchmark, Random)
		->Iterations(1 << 20)
		->Unit(benchmark::kMicrosecond);

	BENCHMARK_REGISTER_F(UtilityBenchmark, Uniform)
		->Iterations(1 << 20)
		->Unit(benchmark::kMicrosecond);

	BENCHMARK_REGISTER_F(UtilityBenchmark, Normal)
		->Args({1uLL})
		->Args({2uLL})
		->Args({3uLL})
		->Args({10uLL})
		->Args({100uLL})
		->Iterations(1 << 15)
		->Unit(benchmark::kMicrosecond);

	BENCHMARK_REGISTER_F(UtilityBenchmark, HMACSign)
		->Iterations(1 << 15)
		->Unit(benchmark::kMicrosecond);

	BENCHMARK_REGISTER_F(UtilityBenchmark, HMACVerify)
		->Iterations(1 << 15)
		->Unit(benchmark::kMicrosecond);

}
BENCHMARK_MAIN();
