#ifndef CRAWLER_HPP
#define CRAWLER_HPP
#include "thread_safe_structures.hpp"
#include <string>
#include <utility>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

class Crawler
{
public:
    Crawler();
    ~Crawler();

    void start(const std::string &seed_url, int max_depth, int num_threads, int delay_ms);

private:
    void worker();

    ThreadSafeQueue<std::pair<std::string, int>> urls_to_visit;
    ThreadSafeSet<std::string> urls_visited;
    std::vector<std::thread> workers;
    std::atomic<int> active_workers{0};

    int max_depth_limit;

    std::chrono::milliseconds politeness_delay{0};
    
};

#endif