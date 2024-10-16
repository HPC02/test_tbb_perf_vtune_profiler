#include <stdlib.h>
#include <chrono>
#include <iostream>

#include <tbb/tbb.h>

namespace {

const size_t kArraySize = 50 * 1000 * 1000;
double v[kArraySize];

}  // namespace

void VectorInit(double* v, size_t n) {
  tbb::parallel_for(size_t(0), n, size_t(1), [=](size_t i) { v[i] = i * 2; });
}

double VectorReduction(double* v, size_t n) {
  return tbb::parallel_deterministic_reduce(
    tbb::blocked_range<double*>(v, v + n), 0.f,
    [](const tbb::blocked_range<double*>& r, double value) -> double { return std::accumulate(r.begin(), r.end(), value); },
    std::plus<double>());
}

int main(int argc, char* argv[]) {
  const auto t0 = std::chrono::high_resolution_clock::now();
  //task_scheduler_init(task_scheduler_init::automatic);
  VectorInit(v, kArraySize);

  double sum{};
  for (int i = 0; i < 100; i++) {
    sum += VectorReduction(v, kArraySize);
  }

  const auto t1 = std::chrono::high_resolution_clock::now();
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
  std::cout << "Duration: " << duration / 1000.0 << " seconds" << std::endl;
  std::cout << "Sum: " << sum << std::endl;

  return 0;
}