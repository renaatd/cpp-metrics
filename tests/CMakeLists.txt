# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

# On recent versions of CMake: use DOWNLOAD_EXTRACT_TIMESTAMP YES
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/d1467f5813f4d363cfd11aba99c4e9fe47a85e99.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(UnitTests
    ./TestGauge.cpp
    ./TestHistogram.cpp
    ./TestKurtosis.cpp
    ./TestMinMax.cpp
    ./TestMinMeanMax.cpp
    ./TestSamplingReservoir.cpp
    ./TestSlidingWindowReservoir.cpp
    ./TestSnapshot.cpp
    ./TestVariance.cpp
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

add_executable(TestDeadlock
    ./TestDeadlock.cpp
)
target_link_libraries(TestDeadlock
    Metrics
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(UnitTests)
gtest_discover_tests(TestDeadlock)
