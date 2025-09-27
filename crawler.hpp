#ifndef CRAWLER_HPP
#define CRAWLER_HPP
#include "thread_safe_structures.hpp"
#include <string>
#include <utility>

class Crawler{
    public:
    Crawler();
    ~Crawler();

    void start(const std::string& seed_url, int max_depth);

    private:
    ThreadSafeQueue<std::pair<std::string,int>> urls_to_visit;
    ThreadSafeSet<std::string> urls_visited;
};

#endif