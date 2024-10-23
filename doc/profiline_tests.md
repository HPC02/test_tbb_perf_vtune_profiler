# 性能测试以及各种优化对比测试 #

## 1. soft prefetch ##

`prefetch`之前代码 (`src/test_grain_size`)：

```c++
double VectorReduction(double* v, size_t n) {
  return tbb::parallel_deterministic_reduce(
    tbb::blocked_range<double*>(v, v + n, kGrainSize), 0.f,
    [](const tbb::blocked_range<double*>& r, double value) -> double { return std::accumulate(r.begin(), r.end(), value); },
    std::plus<double>());
}
```

测试代码运行时间 `233.106 seconds` .

`prefetch`之后代码 (`src/test_grain_size_prefetch`)：

```c++
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
```

测试代码运行时间 `232.998 seconds` .


## 参考资料 ##

* [gcc-gnu -- Data Prefetch Support](https://gcc.gnu.org/projects/prefetch.html)
* [ARM -- Prefetching with __builtin_prefetch](https://developer.arm.com/documentation/101458/2304/Optimize/Prefetching-with---builtin-prefetch)
* [OpenBLAS bentchmark](https://github.com/OpenMathLib/OpenBLAS/tree/develop/benchmark)
