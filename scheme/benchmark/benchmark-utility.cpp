#include "definitions.h"
#include "utility.hpp"

#include <benchmark/benchmark.h>

using namespace std;

namespace DCPE
{
	class UtilityBenchmark : public ::benchmark::Fixture
	{
	};

	BENCHMARK_DEFINE_F(UtilityBenchmark, Random)
	(benchmark::State& state)
	{
		for (auto _ : state)
		{
			benchmark::DoNotOptimize(get_random_bytes(64));
		}
	}

	BENCHMARK_REGISTER_F(UtilityBenchmark, Random)
		->Iterations(1 << 20)
		->Unit(benchmark::kMicrosecond);
}

BENCHMARK_MAIN();
