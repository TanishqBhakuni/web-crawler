#ifndef CRAWLER_HPP
#define CRAWLER_HPP
#include "thread_safe_structures.hpp"
#include "robots_cache.hpp"

#include <string>
#include <utility>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>

class Crawler
{
public:
    Crawler();
    ~Crawler();

    void start(const std::string &seed_url, int max_depth, int num_threads, int delay_ms);
    // Print a short crawl summary (for end-of-run reporting)
    void print_stats() const;

private:
    void worker();

    ThreadSafeQueue<std::pair<std::string, int>> urls_to_visit;
    ThreadSafeSet<std::string> urls_visited;
    std::vector<std::thread> workers;
    std::atomic<int> active_workers{0};

    int max_depth_limit;

    std::chrono::milliseconds politeness_delay{0};
    RobotsCache robots_rules_cache_;

    std::atomic<int> pages_crawled_{0};
    std::atomic<int> links_found_{0};
    std::atomic<int> http_errors_{0};

    // Output and timing for showcase/reporting
    std::ofstream output_file_;
    std::mutex file_writer_mutex_;
    std::chrono::high_resolution_clock::time_point start_time_{};
    
};

#endif