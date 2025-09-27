#ifndef CRAWLER_HPP
#define CRAWLER_HPP
#include "thread_safe_structures.hpp"
#include <string>
#include <utility>
#include <vector>
#include <thread>
#include <atomic>

class Crawler
{
public:
    Crawler();
    ~Crawler();

    void start(const std::string &seed_url, int max_depth, int num_threads);

private:
    void worker();

    ThreadSafeQueue<std::pair<std::string, int>> urls_to_visit;
    ThreadSafeSet<std::string> urls_visited;
    std::vector<std::thread> workers;

    int max_depth_limit;

    std::atomic<int> active_workers{0};
};

#endif