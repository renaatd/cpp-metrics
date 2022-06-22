# C++ Metrics

Small C++ library, header-only, to measure parameter distributions in an embedded application.

C++ Metrics is inspired by Coda Hale's Metrics library, but has fewer features. 

## Features
- measurement types: gauge, statistics (min/mean/max/variance/stdev), histogram
- histogram can use sliding window reservoir or sampling reservoir
- low overhead: typically < 10 ns / measurement
- optional registry for reporting from another location
- no build system needed, just copy the header files in a project
- no background threads
- no external dependencies

## Limitations
- uses naive locking (mutex) when sampling - can impact performance on some processors or when concurrency is very high
- no rate-type measurements