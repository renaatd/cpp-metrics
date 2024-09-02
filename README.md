# C++ Metrics
![Build status](https://github.com/renaatd/cpp-metrics/actions/workflows/cmake.yml/badge.svg)

C++ Metrics is a header-only library to measure parameter distributions in an embedded application. The library is inspired by Coda Hale's Metrics library, but has fewer features. 

## Metrics
| Class            | Description                                                             |
|------------------|-------------------------------------------------------------------------|
| Gauge            | Store a single measurement                                              |
| MinMax           | Store minimum/maximum measurement                                       |
| MinMeanMax       | Same as above + mean value                                              |
| Variance         | Same as above + (sample) variance, (sample) standard deviation, and RMS |
| Kurtosis         | Same as above + skew and kurtosis                                       |
| LinearRegression | Least squares linear regression - best fit line through measurements    |
| Histogram        | Store n samples in a reservoir, get bins, min/Q25/Q50/Q75/max           |

## Reservoirs for histogram
| Class                  | Description                                                  |
|------------------------|--------------------------------------------------------------|
| SlidingWindowReservoir | Store last n measurements                                    |
| SamplingReservoir      | Store n randomly selected measurements from all measurements | 

## Features
- low overhead: typically < 10 ns / measurement
- updating is made thread-safe by using mutexes, mutexes can be disabled at compile time
- optional registry for reporting all metrics at once
- no build system needed, just copy the header files in a project
- no background threads
- no external dependencies

## Limitations
- uses naive locking (mutex) when sampling - can impact performance on some processors or when parallellism is very high
- no rate-type measurements

## Algorithms
- Reservoir sampling: [optimal algorithm L](https://en.wikipedia.org/wiki/Reservoir_sampling#Optimal:_Algorithm_L)
- Variance: [Welford's online algorithm](https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm)
- Linear regression using LSQ: [Simple linear regression](https://en.wikipedia.org/wiki/Simple_linear_regression)

## See also
- [John D. Cook - Computing skewness and kurtosis in one pass](https://www.johndcook.com/blog/skewness_kurtosis/) - online calculation of mean/variance/skew/kurtosis in C++ - inspired me to also implement += and + on statistics
