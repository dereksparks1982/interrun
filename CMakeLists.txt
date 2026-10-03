cmake_minimum_required(VERSION 3.16)
project(Interrun VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(interrun_core
    src/core/address_router.cpp
    src/profile/profile_store.cpp
    src/privacy/privacy_policy.cpp
    src/privacy/privacy_receipt.cpp
)

target_include_directories(interrun_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_options(interrun_core PRIVATE -Wall -Wextra -Wpedantic -Werror)

add_executable(interrun src/main.cpp)
target_link_libraries(interrun PRIVATE interrun_core)
target_compile_options(interrun PRIVATE -Wall -Wextra -Wpedantic -Werror)

enable_testing()
add_executable(interrun_core_tests tests/core_tests.cpp)
target_link_libraries(interrun_core_tests PRIVATE interrun_core)
target_compile_options(interrun_core_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
add_test(NAME interrun_core_tests COMMAND interrun_core_tests)
