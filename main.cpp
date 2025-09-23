#include <iostream>
// #include <cpr/cpr.h>
#include <vector>
#include <string>
#include "crawler.hpp"

int main() {

    std::cout << "--- Main application starting ---" << std::endl;

    Crawler my_crawler;

    my_crawler.start("https://www.google.com", 2);

    std::cout << "--- Main application finishing ---" << std::endl;

    return 0;
}