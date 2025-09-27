#include <iostream>
// #include <cpr/cpr.h>
#include <vector>
#include <string>
#include "crawler.hpp"

int main()
{

    std::cout << "--- C++ Web Crawler Starting ---" << std::endl;

    Crawler crawler;

    const std::string seed_url = "https://example.com";
    const int max_depth = 1;
    const int num_threads = 4;
    crawler.start(seed_url, max_depth, num_threads);

    std::cout << "--- C++ Web Crawler Finished ---" << std::endl;
    return 0;
}