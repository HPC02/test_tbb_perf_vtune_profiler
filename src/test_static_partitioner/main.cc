#include <oneapi/tbb/partitioner.h>
#include <stdlib.h>
#include <unistd.h>  // getpid()
#include <chrono>
#include <cstddef>
#include <iostream>

#include <tbb/task_arena.h>
#include <tbb/tbb.h>

// #include "3rd_utils.h"

namespace {

constexpr size_t kArraySize = 50 * 1000 * 1000;
double v[kArraySize];

}  // namespace

void VectorInit(double* v, size_t n) {
  tbb::parallel_for(size_t(0), n, size_t(1), [=](size_t i) { v[i] = i * 0.02; });
}

double VectorReduction(double* v, size_t n) {
  tbb::task_arena ta(8);
  double sum = ta.execute([&]() {
    return tbb::parallel_deterministic_reduce(
      tbb::blocked_range<double*>(v, v + n), 0.0,
      [](const tbb::blocked_range<double*>& r, double value) -> double { return std::accumulate(r.begin(), r.end(), value); },
      std::plus<double>(), tbb::static_partitioner{});
  });

  return sum;
}

int main(int argc, char* argv[]) {
  const pid_t pid = getpid();
  std::cout << argv[0] << ". Process ID: " << pid << std::endl;

  constexpr size_t kTestLoopNum = 1000 * 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  VectorInit(v, kArraySize);

  double sum1 = VectorReduction(v, kArraySize);
  for (int i = 0; i < kTestLoopNum; i++) {
    const double sum = VectorReduction(v, kArraySize);
    //const auto chk_result = approximatelyEqualAbsRel(sum, sum1, 1e-3, 1e-6);
    //CHECK1(chk_result, "VectorReduction not match");
    sum1 = sum;
  }

  const auto t1 = std::chrono::high_resolution_clock::now();
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
  std::cout << "Duration: " << duration / 1000.0 << " seconds" << std::endl;
  std::cout << "Sum: " << sum1 << ". loop num: " << kTestLoopNum << std::endl;

  return 0;
}