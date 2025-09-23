#ifndef CRAWLER_HPP
#define CRAWLER_HPP
#include <string>
#include <queue>
#include <unordered_set>


class Crawler{
    public:
    Crawler();
    ~Crawler();

    void start(const std::string& seed_url, int max_depth);

    private:
    std::queue<std::string> urls_to_visit;
    std::unordered_set<std::string> urls_visited;


};

#endif