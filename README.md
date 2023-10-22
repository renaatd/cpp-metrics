# C++ Metrics
![Build status](https://github.com/renaatd/cpp-metrics/actions/workflows/cmake.yml/badge.svg)

C++ Metrics is a header-only library to measure parameter distributions in an embedded application. The library is written for personal (re)use.

C++ Metrics is inspired by Coda Hale's Metrics library, but has fewer features. 

## Features
- measurement types: gauge, statistics (min/mean/max/variance/stdev), histogram
- histogram can use sliding window reservoir or sampling reservoir
- low overhead: typically < 10 ns / measurement
- updating is thread-safe by default using mutexes, mutexes can be disabled at compile time
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

## See also
- [John D. Cook - Computing skewness and kurtosis in one pass](https://www.johndcook.com/blog/skewness_kurtosis/) - online calculation of mean/variance/skew/kurtosis in C++ - inspired me to also implement += and + on statistics