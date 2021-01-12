# CPU Results for Audio Decoding on HPVM

## Non-streaming Versions

### General Findings
1. Same as the HPVM version of audio encoding, it is launching a single thread on CPU regardless of the function-level parallelism.
2. The order of the execution is as follow: `rotatorSet` -> `zoomerSet` -> `psychoFilter` -> `rotateOrder1` -> `rotateOrder2` -> `rotateOrder3` -> `zoomerProcess` -> `FFT_left` -> `FFT_right` -> `FIR_left` -> `FIR_right` -> `IFFT_left` -> `IFFT_right` -> `overlap_left` -> `overlap_right`.

### Profiling Results from Intel VTune

#### General Information
1. The test is based on the modification of updating the `std::map` to `std::unordered_map` in the HPVM runtime.
2. For the binauralization pipeline, sometimes the left function (e.g. `FFT_left`) is faster than the corresponding right function (e.g. `FFT_right`), while sometimes the results are inversed.
3. The functions with parallel implementation include `FIR_left`, `FIR_right`, `rotateOrder1`, `rotateOrder2`, and `rotateOrder3`.

#### Result Table and Remarks

| Version | Block Size | Average CPU Time (s) | psychoFilter | FFT_left | FFT_right | IFFT_left | IFFT_right | rotateOrder3 | FIR_left | FIR_right | llvm_hpvm_cpu_dstack_push | llvm_hpvm_cpu_dstack_pop | rotateOrder2 | zoomProcess |
|---------|------------|------------------|--------------|----------|-----------|-----------|------------|--------------|----------|-----------|---------------------------|--------------------------|--------------|-------------|
| Basic | 500 | 0.647 | 27.89% | 16.1% | 11.27% | 10.17% | 10.3% | 8.27% | 6.43% | 6.47% | - | - | - | - |
| Basic | 1000 | 1.250 | 28.47% | 9.4% | 9.5% | 11.83% | 10.97% | 11.8% | 3.2% | 5.77% | - | - | - | - |
| Parallel | 500 | 2.97 | 5.53% | 2.87% | 2.03% | 2.87% | 2.13% | 5.57% | 24.5% | 22.37% | 13.63% | 8.2% | 4.37% | 3.7% |
| Parallel | 1000 | 5.94 | 5.4% | 2.97% | 2.03% | 2.43% | 2.33% | 5.6% | 23.4% | 23.37% | 13.0% | 8.9% | 4.33% | 2.07% |

#### Remarks
1. The reason I am using percentage instead of the absolute value is that we care about the functions that are taking the largest portion of the time. Unlike audio encoding, audio decoding has more dominant functions.
2. As show in the table, the average CPU time doubles when the block size changes from 500 to 1000.
3. In the Basic version, the three most time-consuming functions (`psychoFilter`, `FFT`, and `IFFT`) are all calling a function called `kf_work`. According to the profiling results, `kf_work` is the function that is taking the most portion of the time.
4. In the Parallel version, the extra overhead is caused by the HPVM runtime. For example, the HPVM runtime is calling `llvm_hpvm_cpu_dstack_push` and `llvm_hpvm_cpu_dstack_pop` in the parallel version, but these two functions are not called in the non-parallel versions. Plus, for functions that have unrolled loops (e.g. `FIR_left` and `FIR_right`), the HPVM runtime is calling `pthread_mutex_lock` and `pthread_mutex_unlock` to secure a valid execution. However, since the CPU version is still executing each unrolled loop in sequential, these two extra function calls are causing extra timing overhead. For the functions that does not have unrolled loops (e.g. `psychoFilter`), the execution time between the non-parallel versions and the parallel versions is almost the same (e.g. for `psychoFilter`, 0.647 * 27.89% is almost identical to 2.97 * 5.53%).
5. The HPVM runtime overhead mentioned in the previous remark point is caused by an HPVM function called `invoke_child`. This function is called every time when the dynamic replication is executed ("dynamic replication execution" = "the execution of each unrolled loop"). Inside each `invoke_child`, it is running `llvm_hpvm_request_mem`, which will further call `pthread_mutex_lock`, `pthread_mutex_unlock`, and the functions related to `std::unordered_map`.

#### Takeaways
1. The three rotate-order functions (`rotateOrder1`, `rotateOrder2`, and `rotateOrder3`) can definitely be parallelly executed. At a first glimpse, the benefit we can gain is limited, as shown from the result table. This is the FIR functions (`FIR_left` and `FIR_right`). However, after achieving the task-level (or say function-level) parallelism, we may have some good improvement, since the results in the table are from sequential execution for all functions.
2. We may need to change some settings in the code to achieve parallelism. For example, in the rotate-order functions, we need to change intermediate variables into intermediate arrays to achieve parallel executions.
3. Functions like `psychoFilter`, `FFT_left`, `FFT_right`, `IFFT_left`, and `IFFT_right` has inter-loop dependencies (e.g. addition and multiplication, like `a += b[i];`). They cannot be directly changed into parallel execution, but the performance can be improved by using the reduction algorithm at the cost of a small area and energy overhead.

## Streaming Versions

### General Findings
1. The streaming version may use multiple cores when having task-level parallelisms. This is implemented by the HPVM runtime for the streaming version. In the non-parallel version, it was using at most 2 cores. In the parallel version, it sometimes ran 3 cores simultaneously.
2. Since audio decoding has task-level parallelism, the HPVM runtime is also calling `pthread_mutex_lock` and `pthread_mutex_unlock`. Plus, this task-level parallelism will call `llvm_hpvm_bufferPush` and `llvm_hpvm_bufferPop`, and these two functions will further call `pthread_cond_signal` and `pthread_cond_wait` respectively. These two `pthread_cond` functions will incur timing overhead.
3. The runtime overhead mentioned in the previous two fingding points makes the streaming version slower than the non-streaming version on average. Though they are using more cores, the spin time is significant (the spin time is shown in the table below).
4. Even in the parallel version, `llvm_hpvm_cpu_dstack_push` and `llvm_hpvm_cpu_dstack_pop` are taking a trivial or even no time. This might be due to the implementation of the HPVM runtime.

### Profiling Results from Intel VTune

#### General Information
The points in "General Information", "Remarks", and "Takeaways" are nearly identical to the non-streaming versions. The following sections only contain the newly added remarks that are more relevant to the streaming versions.

#### Result Table

| Version | Block Size | Average CPU Time (s) | Spin Time (s) | psychoFilter | FFT_left | FFT_right | IFFT_left | IFFT_right | FIR_left | FIR_right | rotateOrder3 | rotateOrder2 | rotateOrder1 | zoomProcess |
|---------|------------|------------------|-----------|--------------|----------|-----------|-----------|------------|----------|-----------|--------------|--------------|--------------|-------------|
| Streaming | 500 | 0.693 | 0.08 | 21.6% | 11.13% | 10.8% | 9.97% | 9.63% | 5.6% | 5.2% | 9.67% | 1.17% | 0.77% | 10% |
| Streaming | 1000 | 1.634 | 0.17 | 20.4% | 10.17% | 10.17% | 10% | 10.33% | 5.33% | 5% | 9.43% | 2.5% | 1.63% | 8.7% |
| Streaming-and-Parallel | 500 | 8.54 | 5.44 | 0.97% | 0.47% | 0.47% | 0.4% | 0.47% | 39.23% | 39.17% | 6.17% | 5.93% | 5.93% | 0.5% |
| Streaming-and-Parallel | 1000 | 17.317 | 10.85 | 0.97% | 0.43% | 0.5% | 0.5% | 0.43% | 39.2% | 39.23% | 6.13% | 5.97% | 5.8% | 0.37% |
