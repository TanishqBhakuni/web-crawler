#include <iostream>
#include <exception>
#include <string>
#include <thread>

#include "cxxopts/cxxopts.hpp"
#include "crawler.hpp"



int main(int argc, char* argv[])
{
    cxxopts::Options options("Crawler", "A simple, multithreaded web crawler");

    options.add_options()
       
        ("h,help", "Print usage information")
        ("u,url", "The seed URL to start the crawl from", cxxopts::value<std::string>())
        ("d,depth", "The maximum depth for the crawl", cxxopts::value<int>()->default_value("2"))
        ("t,threads", "Number of worker threads", cxxopts::value<int>()->default_value(std::to_string(std::thread::hardware_concurrency())))
        ("l,delay", "Politeness delay between requests (in ms)", cxxopts::value<int>()->default_value("1000"));

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (result.count("url") == 0) {
        std::cerr << " Error: The -url option is mandatory " << std::endl;
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string seed_url = result["url"].as<std::string>();
    int max_depth = result["depth"].as<int>();
    int num_threads = result["threads"].as<int>();
    int delay_ms = result["delay"].as<int>();

    Crawler crawler;

    std::cout << "--- Crawler Starting ---" << std::endl;
    std::cout << "URL: " << seed_url << std::endl;
    std::cout << "Depth: " << max_depth << std::endl;
    std::cout << "Threads: " << num_threads << std::endl;
    std::cout << "Delay: " << delay_ms << "ms" << std::endl;
    std::cout << "---------------------------" << std::endl;

    try
    {
        crawler.start(seed_url, max_depth, num_threads, delay_ms);
    }
    catch(const std::exception& e)
    {
        std::cerr << "An unexpected error occurred during the crawl. " << e.what() << std::endl;
        return 1;
    }

    std::cout << "--- Crawl Complete ---" << std::endl;
    return 0;
}