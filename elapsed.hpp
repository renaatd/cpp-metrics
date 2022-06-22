#ifndef ELAPSED_HPP
#define ELAPSED_HPP

#include <chrono>

class Elapsed {
  public:
    long long ElapsedUs() {
        auto stop = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(stop - _start).count();
    }
  private:
    const std::chrono::time_point<std::chrono::high_resolution_clock> _start { std::chrono::high_resolution_clock::now() };
};

#endif
