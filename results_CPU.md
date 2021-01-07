# CPU Results for Audio Decoding on HPVM

## General Findings
1. Same as the HPVM version of audio encoding, it is launching a single thread on CPU regardless of the function-level parallelism.
2. The order of the execution is as follow: `rotatorSet` -> `zoomerSet` -> `psychoFilter` -> `rotateOrder1` -> `rotateOrder2` -> `rotateOrder3` -> `zoomerProcess` -> `FFT_left` -> `FFT_right` -> `FIR_left` -> `FIR_right` -> `IFFT_left` -> `IFFT_right` -> `overlap_left` -> `overlap_right`.

## Profiling Results from Intel VTune

### General Information
1. The test is based on the modification of updating the `std::map` to `std::unordered_map` in the HPVM runtime.
2. For the binauralization pipeline, sometimes the left function (e.g. `FFT_left`) is faster than the corresponding right function (e.g. `FFT_right`), while sometimes the results are inversed.

### Result Tables and Remarks

| Version | Block Size | Average CPU Time (s) | psychoFilter | FFT_left | FFT_right | IFFT_left | IFFT_right | rotateOrder3 | FIR_left | FIR_right | llvm_hpvm_cpu_dstack_push | llvm_hpvm_cpu_dstack_pop | rotateOrder2 | zoomProcess |
|---------|------------|----------------------|--------------|----------|-----------|-----------|------------|--------------|----------|-----------|---------------------------|--------------------------|--------------|-------------|
| Basic | 500 | 0.647 | 27.89% | 16.1% | 11.27% | 10.17% | 10.3% | 8.27% | 6.43% | 6.47% | - | - | - | - |
| Basic | 1000 | 1.250 | 28.47% | 9.4% | 9.5% | 11.83% | 10.97% | 11.8% | 3.2% | 5.77% | - | - | - | - |
| Parallel | 500 | 2.97 | 5.53% | 2.87% | 2.03% | 2.87% | 2.13% | 5.57% | 24.5% | 22.37% | 13.63% | 8.2% | 4.37% | 3.7% |
| Parallel | 1000 | 5.94 | 5.4% | 2.97% | 2.03% | 2.43% | 2.33% | 5.6% | 23.4% | 23.37% | 13.0% | 8.9% | 4.33% | 2.07% |

Remarks
1. The reason I am using percentage instead of the absolute value is that we care about the functions that are taking the largest portion of the time. Unlike audio encoding, audio decoding has more dominant functions.
2. As show in the table, the average CPU time doubles when the block size changes from 500 to 1000.
3. In the Basic version, the three most time-consuming functions (`psychoFilter`, `FFT`, and `IFFT`) are all calling a function called `kf_work`. According to the profiling results, `kf_work` is the function that is taking the most portion of the time.
4. In the Parallel version, the extra overhead is caused by the HPVM runtime. For example, the HPVM runtime is calling `llvm_hpvm_cpu_dstack_push` and `llvm_hpvm_cpu_dstack_pop` in the parallel version, but these two functions are not called in the non-parallel versions. Plus, for functions that have unrolled loops (e.g. FIR_left and FIR_right), the HPVM runtime is calling `pthread_mutex_lock` and `pthread_mutex_unlock` to secure a valid execution. However, since the CPU version is still executing each unrolled loop in sequential, these two extra function calls are causing extra timing overhead. For the functions that does not have unrolled loops (e.g. `psychoFilter`), the execution time between the non-parallel versions and the parallel versions is almost the same (e.g. for `psychoFilter`, 0.647 * 27.89% is almost identical to 2.97 * 5.53%).
<br>
Takeaways
1. Most 
