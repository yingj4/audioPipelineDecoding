# CPU Results for Audio Decoding on HPVM

## General Findings
1. Same as the HPVM version of audio encoding, it is launching a single thread on CPU regardless of the function-level parallelism.
2. The order of the execution is as follow: `rotatorSet` -> `zoomerSet` -> `psychoFilter` -> `rotateOrder1` -> `rotateOrder2` -> `rotateOrder3` -> `zoomerProcess` -> `FFT_left` -> `FFT_right` -> `FIR_left` -> `FIR_right` -> `IFFT_left` -> `IFFT_right` -> `overlap_left` -> `overlap_right`.

## Profiling Results from Intel VTune

### General Information
1. The test is based on the modification of updating the `std::map` to `std::unordered_map` in the HPVM runtime.
2. For the binauralization pipeline, sometimes the left function (e.g. `FFT_left`) is faster than the corresponding right function (e.g. `FFT_right`), while sometimes the results are inversed.
3. The extra CPU time in the parallel version is caused by the HPVM runtime. Functions including `pthread_mutext_lock`, `pthread_mutex_unlock`, and `std::unordered_map::find` are causing this extra overhead.

### Result Tables and Remarks

| Version | Block Size | Average CPU Time (s) | psychoFilter | FFT_left | FFT_right | IFFT_left | IFFT_right | rotateOrder3 | FIR_left | FIR_right | llvm_hpvm_cpu_dstack_push | llvm_hpvm_cpu_dstack_pop | rotateOrder2 | zoomProcess |
|---------|------------|----------------------|--------------|----------|-----------|-----------|------------|--------------|----------|-----------|---------------------------|--------------------------|--------------|-------------|
| Basic | 500 | 0.647 | 27.89% | 16.1% | 11.27% | 10.17% | 10.3% | 8.27% | 6.43% | 6.47% | - | - | - | - |
| Basic | 1000 | 1.250 | 28.47% | 9.4% | 9.5% | 11.83% | 10.97% | 11.8% | 3.2% | 5.77% | - | - | - | - |

Remarks
1. The reason I am using percentage instead of the absolute value is that we care about the functions that are taking the largest portion of the time. Unlike audio encoding, audio decoding has more dominant functions.
2. As show in the table, the average CPU time doubles when the block size changes from 500 to 1000.
3. In the Basic version, the three most time-consuming functions (`psychoFilter`, `FFT`, and `IFFT`) are all calling a function called `kf_work`. According to the profiling results, `kf_work` is the function that is taking the most portion of the time.
