# README #

## 1. 测试原始来源 ##

 `VTune Profiler` 进行性能分析：[使用VTune Profiler测试TBB overhead](https://www.intel.com/content/www/us/en/docs/vtune-profiler/cookbook/2024-1/intel-tbb-scheduling-overhead.html)。

### 1.1 编译选项 ###

```cmake
# 1. enable frame pointer optimization
# 2. disable optimize sibling calls (disable call instructions optimized to jump)
# 3. enable debug symbols
target_compile_options(${target_name1} PRIVATE -fno-omit-frame-pointer -fno-optimize-sibling-calls -ggdb)
```

* [(GCC) gcc 编译选项 -fno-omit-frame-pointer, -fno-tree-vectorize, -fno-optimize-sibling-calls, 及内存泄漏、非法访问检测 ASAN](https://www.cnblogs.com/vaughnhuang/p/16435356.html)

## 2. 资料 ##

* `Intel TBB` API 使用教程：[Intel® oneAPI Threading Building Blocks](https://www.intel.com/content/www/us/en/docs/onetbb/developer-guide-api-reference/2021-13/reduction.html)
* [Too long TBB's shedule time when using parallel_deterministic_reduce](https://stackoverflow.com/questions/79090338/too-long-tbbs-shedule-time-when-using-parallel-deterministic-reduce)

## 3. 测试过程及优化 ##

### 3.1 reduce ###

1. 针对一些比如遍历求和操作，他们之间没有顺序要求，可以改用并行的 `reduce`。前提是数据的构造代价小，如稀疏矩阵拷贝代价就比较大。
2. 计算的先后顺序有关的，比如针对浮点的乘加操作，先后顺序变化影响计算精度，此时使用`parallel_deterministic_reduce`。官方解释是：合并顺序是预先定义好的，确保每次调用`deterministic_reduce`的结果相同。

### 3.2 优化：调整 `grain size` ###

`Intel TBB` 动态划分任务，以及把任务提交给线程执行，都需要消耗时间。优化包括：

* 调整 `grain size` 减少调度的开销。
* 使用静态划分`static_partitioner`减少调度开销。

#### 3.2.1 调整 `grain size` ####

通过设置`grain size`，可以大致设定 `TBB` 每个任务要处理的数据量，即划分粒度：

```c++
tbb::blocked_range<double*>(v, v + n, 1000)
```

设置`grain size`之前，显示的热点 `call stack` 如下图 (100次循环):

![grain_size_default_1](./doc/images/perf_no_grain_size_set.png)

设置`grain size`等于10000, `Intel TBB` 内部调度时间明显减少 (10000次循环):

![grain_size_10000](./doc/images/perf_grain_size_10000.png)

#### 3.2.2 使用静态划分`static_partitioner` ####

通过设置使用`static_partitioner`，即预先划分好任务，减少调度开销，具有与`blocked_range`类似的效果。但其控制的方式不同：`static_partitioner`适用于任务均衡的计算场景。

```c++
tbb::task_arena ta(8);
  double sum = ta.execute([&]() {
    return tbb::parallel_deterministic_reduce(
      tbb::blocked_range<double*>(v, v + n), 0.0,
      [](const tbb::blocked_range<double*>& r, double value) -> double { return std::accumulate(r.begin(), r.end(), value); },
      std::plus<double>(), tbb::static_partitioner{});
  });

  return sum;
```

## 4. 使用VTune Profiler & linux perf 测试 ##

见 [性能测试以及各种优化对比测试](/doc/profiline_tests.md) .
