#include "robots_cache.hpp"

void RobotsCache::add(const std::string &domain, const RobotsRules &rules)
{
    std::lock_guard<std::mutex> guard(mutex_);

    cache_[domain] = rules;
}

bool RobotsCache::try_get(const std::string &domain, RobotsRules &rules_out) const
{
    std::lock_guard<std::mutex> guard(mutex_);

    auto it = cache_.find(domain);

    if (it != cache_.end())
    {
        rules_out = it->second;
        return true;
    }

    return false;
}