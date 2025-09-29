# Specifies the minimum version of CMake required to build this project.
cmake_minimum_required(VERSION 3.15)

# Declares the project's name.
project(CppWebCrawler VERSION 1.0)

# Set the C++ standard to C++17 and make it a requirement.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure and add the 'cpr' networking library
set(CPR_BUILD_TESTS OFF)
add_subdirectory(external/cpr)


# Add Gumbo parser from local directory
add_subdirectory(external/gumbo-parser)

# This command defines our main executable target, 'crawler', which is built
# from the 'main.cpp' source file.
add_executable(crawler main.cpp parser.cpp crawler.cpp url_utils.cpp robots_parser.cpp robots_cache.cpp)
target_include_directories(crawler PRIVATE libs)

# This is the crucial linking step. We are now adding 'gumbo' to the list of
# libraries that our 'crawler' target depends on.

# Note that we list all dependencies here. Our 'crawler' needs both 'cpr::cpr' for
# networking and 'gumbo' for HTML parsing.
# The 'PRIVATE' keyword indicates that these libraries are internal implementation
# details of our crawler.
target_link_libraries(crawler PRIVATE cpr::cpr gumbo_static)