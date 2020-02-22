// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <benchmark/benchmark.h>

#include <set> // std::set

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
BENCHMARK(BM_memcpy)->Arg(8)->Arg(64)->Arg(512)->Arg(1 << 10)->Arg(8 << 10);

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
BENCHMARK(BM_memmove)->Arg(8)->Arg(64)->Arg(512)->Arg(1 << 10)->Arg(8 << 10);
