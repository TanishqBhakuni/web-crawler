#ifndef ROBOTS_CACHE_HPP
#define ROBOTS_CACHE_HPP

#include <string>
#include <map>
#include <mutex>
#include "robots_parser.hpp"

class RobotsCache
{
public:
    void add(const std::string &domain, const RobotsRules &rules);
    bool try_get(const std::string &domain, RobotsRules &rules_out) const;

private:
    std::map<std::string, RobotsRules> cache_;
    mutable std::mutex mutex_;
};

#endif