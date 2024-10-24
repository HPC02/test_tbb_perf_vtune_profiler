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

测试代码运行时间 `232.349 seconds`， 有时候可能会更差。

1. 在循环内部添加`prefetch` `hint`，可能会阻止CPU使用向量指令(SIMD/NEON)。在外部使用`prefetch`则不会。见下面`ARM`参考文档。
2. `prefetch`只是给硬件的`hint`，是否执行还是取决于硬件。

## 2. prefetch 编译选项 ##

```bash
# gcc O3 编译开启该选项
target_compile_options(${target_name1} PRIVATE -Ofast -funroll-loops -fprefetch-loop-arrays -march=native)
```

使用效果还需要测试验证。在本测试代码中，使用编译选项产生一些负优化。

## 参考资料 ##

* [gcc-gnu -- Data Prefetch Support](https://gcc.gnu.org/projects/prefetch.html)
* [ARM -- Prefetching with __builtin_prefetch](https://developer.arm.com/documentation/101458/2304/Optimize/Prefetching-with---builtin-prefetch)
* [OpenBLAS bentchmark](https://github.com/OpenMathLib/OpenBLAS/tree/develop/benchmark)
