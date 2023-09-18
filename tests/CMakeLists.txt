# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(UnitTests
    ./TestGauge.cpp
    ./TestHistogram.cpp
    ./TestMinMeanMax.cpp
    ./TestSamplingReservoir.cpp
    ./TestSlidingWindowReservoir.cpp
    ./TestSnapshot.cpp
    ./TestStatistics.cpp
)
target_link_libraries(UnitTests
    Metrics
    gtest_main
)
target_compile_options(UnitTests
    PRIVATE
    -fsanitize=address
)
target_link_options(UnitTests
    PRIVATE
    -fsanitize=address
)

include(GoogleTest)
gtest_discover_tests(UnitTests)
