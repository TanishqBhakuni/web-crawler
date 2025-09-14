cmake_minimum_required(VERSION 3.15)

project(CppWebCrawler VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPR_BUILD_TESTS OFF)
set(CPR_USE_SYSTEM_CURL OFF)
set(CPR_ENABLE_SSL ON)

add_subdirectory(external/cpr)

add_executable(crawler main.cpp)

target_link_libraries(crawler PRIVATE cpr::cpr)

