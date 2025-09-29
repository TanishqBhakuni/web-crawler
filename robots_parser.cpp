#include "robots_parser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

static std::string trim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

static std::string to_lower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
                   { return static_cast<char>(std::tolower(c)); });
    return s;
}

void RobotsRules::parse_content(const std::string &content)
{
    std::stringstream ss(content);
    std::string line;

    bool in_our_section = false;

    while (std::getline(ss, line))
    {
        // Strip comments
        auto hash = line.find('#');
        if (hash != std::string::npos)
            line = line.substr(0, hash);

        line = trim(line);
        if (line.empty())
            continue;

        auto colon = line.find(':');
        if (colon == std::string::npos)
            continue;

        std::string key = to_lower(trim(line.substr(0, colon)));
        std::string value = trim(line.substr(colon + 1));

        if (key == "user-agent")
        {
            in_our_section = (value == "*" || to_lower(value).find("cppwebcrawler") != std::string::npos);
        }
        else if (key == "disallow" && in_our_section)
        {
            if (!value.empty())
            {
                disallowed_paths.insert(value);
            }
        }
    }
}

bool RobotsRules::is_allowed(const std::string &path) const
{
    for (const auto &disallowed_path : disallowed_paths)
    {
        if (disallowed_path == "/")
        {
            return false; // block all
        }
        if (path.rfind(disallowed_path, 0) == 0)
        {
            return false;
        }
    }
    return true;
}