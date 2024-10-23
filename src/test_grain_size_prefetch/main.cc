#include <stdlib.h>
#include <unistd.h>  // getpid
#include <chrono>
#include <cstddef>
#include <iostream>

#include <tbb/task_arena.h>
#include <tbb/tbb.h>

namespace {

constexpr size_t kGrainSize = 1000 * 10;
constexpr size_t kArraySize = 50 * 1000 * 1000;
double v[kArraySize];

}  // namespace

namespace {

// https://www.intel.com/content/www/us/en/docs/onetbb/developer-guide-api-reference/2021-13/reduction.html
template <typename T>
T RepeatableReduce(const T* first, const T* last, T identity) {
  return oneapi::tbb::parallel_deterministic_reduce(
    // Index range for reduction
    oneapi::tbb::blocked_range<const T*>(first, last, 1000),
    // Identity element
    identity,
    // Reduce a subrange and partial sum
    [&](oneapi::tbb::blocked_range<const T*> r, T partial_sum) -> float {
      return std::accumulate(r.begin(), r.end(), partial_sum);
    },
    // Reduce two partial sums
    std::plus<T>());
}

}  // namespace

void VectorInit(double* v, size_t n) {
  tbb::parallel_for(size_t(0), n, size_t(1), [=](size_t i) { v[i] = i * 1.0e-8; });
}

double VectorReduction(double* v, size_t n) {
  return tbb::parallel_deterministic_reduce(
    tbb::blocked_range<double*>(v, v + n, kGrainSize), 0.f,
    [](const tbb::blocked_range<double*>& r, double value) -> double {
      size_t count{};
      for (auto it = r.begin(); it != r.end(); ++it) {
        if ((count % 16) == 0) {
          __builtin_prefetch(it + 8, 0, 3);  // Prefetch the next 16 elements
        }
        value += *it;
        count++;
      }
      return value;
    },
    std::plus<double>());
}

int main(int argc, char* argv[]) {
  const pid_t pid = getpid();
  std::cout << argv[0] << ". Process ID: " << pid << std::endl;

  constexpr size_t kTestLoopNum = 1000 * 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  //task_scheduler_init(task_scheduler_init::automatic);
  VectorInit(v, kArraySize);

  double sum{};
  for (int i = 0; i < kTestLoopNum; i++) {
    sum = VectorReduction(v, kArraySize);
  }

  const auto t1 = std::chrono::high_resolution_clock::now();
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
  std::cout << "Duration: " << duration / 1000.0 << " seconds" << std::endl;
  std::cout << "Sum: " << sum << ". loop num: " << kTestLoopNum << std::endl;

  return 0;
}