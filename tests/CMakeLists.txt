find_package(GTest REQUIRED)

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
    GTest::GTest
    GTest::Main
)
target_compile_options(UnitTests
    PRIVATE
    -fsanitize=address
)
target_link_options(UnitTests
    PRIVATE
    -fsanitize=address
)
gtest_discover_tests(UnitTests)