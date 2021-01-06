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

| Version | Block Size | Average CPU Time (s) | psychoFilter | FFT | IFFT | zoomerProcess | rotateOrder3 |
|---------|------------|----------------------|--------------|-----|------|---------------|--------------|
| Basic | 500 | 0.585 | 21.3% | 23.9% | 28.7% | 8.3% | 15.0% |
| Basic | 1000 | 1.174 | 26.4% | 23.0% | 28.7% | 5.1% | 7.6% |

Remarks
1. The reason I am using percentage instead of the absolute value is that we care about the functions that are taking the largest portion of the time. Unlike audio encoding, audio decoding has more dominant functions.
2. As show in the table, the average CPU time doubles when the block size changes from 500 to 1000.
3. In the Basic version, the three most time-consuming functions (`psychoFilter`, `FFT`, and `IFFT`) are all calling a function called `kf_work`. According to the profiling results, `kf_work` is the function that is taking the most portion of the time.
