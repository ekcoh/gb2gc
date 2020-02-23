// Contains two basic benchmark sequencies for memcpy and memmove

#include <benchmark/benchmark.h>

int main(int argc, char** argv)
{
   ::benchmark::Initialize(&argc, argv);
   if (::benchmark::ReportUnrecognizedArguments(argc, argv))
      return 1;
   ::benchmark::RunSpecifiedBenchmarks();
}

static void BM_memcpy(benchmark::State& state)
{
   char* src = new char[state.range(0)];
   char* dst = new char[state.range(0)];
   memset(src, 'x', state.range(0));
   for (auto _ : state)
      memcpy(dst, src, state.range(0));
   state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
   delete[] src;
   delete[] dst;
}
BENCHMARK(BM_memcpy)->Arg(8)->Arg(64)->Arg(512)->Arg(4096);

static void BM_memmove(benchmark::State& state)
{
   char* src = new char[state.range(0)];
   char* dst = new char[state.range(0)];
   memset(src, 'x', state.range(0));
   for (auto _ : state)
      std::memmove(dst, src, state.range(0));
   state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
   delete[] src;
   delete[] dst;
}
BENCHMARK(BM_memmove)->Arg(8)->Arg(64)->Arg(512)->Arg(4096);
