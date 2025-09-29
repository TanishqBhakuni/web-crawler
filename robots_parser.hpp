#ifndef ROBOTS_PARSER_HPP
#define ROBOTS_PARSER_HPP

#include <string>
#include <unordered_set>
#include <vector>

struct RobotsRules {
    public:
    void parse_content(const std::string& content);

    bool is_allowed(const std::string& path) const;

    private:
    std::unordered_set<std::string> disallowed_paths;
};

#endif