# 使用 perf 进行性能分析 #

## 1. 配置 ##

### 1.1 安装 perf ###

```bash
sudo apt-get install linux-tools-common linux-tools-generic linux-tools-`uname -r`
```

### 1.2 设置系统相关设置项以允许 perf 采集 ###

```bash
# 允许非特权用户进行内核分析和访问 CPU 事件
echo 0 | sudo tee /proc/sys/kernel/perf_event_paranoid
sudo sh -c 'echo kernel.perf_event_paranoid=0 >> /etc/sysctl.d/local.conf'

# 启用内核模块符号解析以供非特权用户使用
echo 0 | sudo tee /proc/sys/kernel/kptr_restrict
sudo sh -c 'echo kernel.kptr_restrict=0 >> /etc/sysctl.d/local.conf'

# 生效系统设置
sudo sysctl -p
```

其中：

1. 第一项设置，允许非特权用户进行内核分析和访问 CPU 事件。
2. 第二项设置(`kptr_restrict`)，可以使得`perf`工具可以访问到内核指针，即允许内核符号(`kallsyms`)被映射到用户层。

## 2. 开始 perf 测量及收集数据 ##

在被测量的程序中，添加如下代码获取自身`PID`:

```c++
#include <unistd.h>  // getpid

const pid_t pid = getpid();
std::cout << argv[0] << ". Process ID: " << pid << std::endl;
```

使用ggdb，并使用`fno-omit-frame-pointer`等编译选项，遍已完成之后，启动程序。

随后启动`perf`命令：

```bash
perf record -F 599 -e cycles,cache-misses -ag -p 12738 -- sleep 200
```

其中：

* `-e cycles,cache-misses` 表示采集的类型，使用 `perf list` 可以列出所有可用的事件，如`CPU`相关的事件，`cache`相关的事件，以及是硬件采集 (`PMC`) 还是软件采集；
* `-p 12738` 表示进程 PID；
* `sleep 200` 表示采集持续时间；

采集完成之后，将生成的`perf.data` 重命名为 `data.perf`， `.perf` 文件是 `VTune` 可以识别的文件格式。

为防止采样频率与代码中的某些周期性代码因同频而导致每次采集到相同的地方，故设置采集频率不能为10的倍数，也不能是2的幂次方，可尽量避免采集误导性数据。参考 
[Using perf On Arm platforms](https://static.linaro.org/connect/yvr18/presentations/yvr18-416.pdf)

## 3. VTune 分析数据 ##

## 4. 参考资料 ##

* [现代CPU性能分析与优化 -- Linux Perf](https://weedge.github.io/perf-book-cn/zh/chapters/7-Overview-Of-Performance-Analysis-Tools/7-4_Linux_perf_cn.html)
* [perf Examples](https://www.brendangregg.com/perf.html)

## 5. 更多学习资料 ##

* [Performance Analysis and Tuning on Modern CPU 中文翻译](https://github.com/weedge/perf-book-cn)
* [Using Linux perf at Netflix](https://brendangregg.com/Slides/KernelRecipes_Perf_Events.pdf)
