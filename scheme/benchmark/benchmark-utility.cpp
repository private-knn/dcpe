#include "definitions.h"
#include "utility.hpp"

#include <benchmark/benchmark.h>

namespace DCPE
{
	// change to run all tests from different seed
	const auto TEST_SEED = 0x13;

	template <typename VALUE_T>
	class UtilityBenchmark : public ::benchmark::Fixture
	{
		void SetUp(const ::benchmark::State& state)
		{
			srand(TEST_SEED);
		}
	};

	BENCHMARK_TEMPLATE_DEFINE_F(UtilityBenchmark, Random, float)
	(benchmark::State& state)
	{
		for (auto _ : state)
		{
			benchmark::DoNotOptimize(get_ramdom_ull());
		}
	}

#define B_Uniform(type)                                                 \
	BENCHMARK_TEMPLATE_DEFINE_F(UtilityBenchmark, Uniform_##type, type) \
	(benchmark::State & state)                                          \
	{                                                                   \
		auto i = 0uLL;                                                  \
		for (auto _ : state)                                            \
		{                                                               \
			sample_uniform<type>(0.0, 100.0, i);                        \
		}                                                               \
	}

	B_Uniform(float);
	B_Uniform(double);

#define B_Normal(type)                                                 \
	BENCHMARK_TEMPLATE_DEFINE_F(UtilityBenchmark, Normal_##type, type) \
	(benchmark::State & state)                                         \
	{                                                                  \
		auto count = (ull)state.range(0);                              \
                                                                       \
		auto i = 0uLL;                                                 \
		for (auto _ : state)                                           \
		{                                                              \
			sample_normal_series<type>(0.0, 100.0, i, count);          \
		}                                                              \
	}

	B_Normal(float);
	B_Normal(double);

	BENCHMARK_REGISTER_F(UtilityBenchmark, Random)
		->Iterations(1 << 20)
		->Unit(benchmark::kMicrosecond);

#define R_Uniform(type)                                    \
	BENCHMARK_REGISTER_F(UtilityBenchmark, Uniform_##type) \
		->Iterations(1 << 20)                              \
		->Unit(benchmark::kMicrosecond);

	R_Uniform(float);
	R_Uniform(double);

#define R_Normal(type)                                    \
	BENCHMARK_REGISTER_F(UtilityBenchmark, Normal_##type) \
		->Args({1uLL})                                    \
		->Args({2uLL})                                    \
		->Args({3uLL})                                    \
		->Args({10uLL})                                   \
		->Args({100uLL})                                  \
		->Iterations(1 << 15)                             \
		->Unit(benchmark::kMicrosecond);

	R_Normal(float);
	R_Normal(double);

}
BENCHMARK_MAIN();
