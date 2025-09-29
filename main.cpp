#include <iostream>
#include <exception>
#include <string>
#include <thread>

#include "cxxopts/cxxopts.hpp"
#include "crawler.hpp"
#include "url_utils.hpp"
#include "robots_parser.hpp"

int main(int argc, char *argv[])
{
    cxxopts::Options options("Crawler", "A configurable, multithreaded web crawler");

    options.add_options()("h,help", "Print this help message and exit")("u,url", "The mandatory seed URL to start the crawl from", cxxopts::value<std::string>())("d,depth", "Maximum link depth to crawl (e.g., depth 1 crawls the seed and its direct links)", cxxopts::value<int>()->default_value("2"))("t,threads", "Number of worker threads to use (defaults to hardware core count)", cxxopts::value<int>()->default_value(std::to_string(std::thread::hardware_concurrency())))("l,delay", "Sets a politeness delay between requests for each thread (in milliseconds)", cxxopts::value<int>()->default_value("1000"));

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (result.count("url") == 0)
    {
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
    catch (const std::exception &e)
    {
        std::cerr << "An unexpected error occurred during the crawl. " << e.what() << std::endl;
        return 1;
    }

    std::cout << "--- Crawl Complete ---" << std::endl;
    return 0;
}