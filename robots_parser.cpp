#include "robots_parser.hpp"
#include <sstream>
#include <algorithm>


std::string trim(const std::string& s) { 
    size_t start = s.find_first_not_of("\t\n\r");
    if(start == std::string::npos) return "";

    size_t end = s.find_last_not_of("\t\n\r");
    return s.substr(start, end - start + 1);
}

void RobotsRules::parse_content(const std::string& content){
    std::stringstream ss(content);
    std::string line;

    bool our_user_agent_section = true;

    while (std::getline(ss, line))
    {
        line = trim(line);

        if (line.rfind("User Agent:", 0) == 0)
        {
            std::string agent = trim(line.substr(11));
            our_user_agent_section = (agent == "*");
        }

        if (our_user_agent_section && line.rfind("Disallow: ", 0) == 0)
        {
            std::string path = trim(line.substr(9));
            if (!path.empty())
            {
                disallowed_paths.insert(path);
            }
        }
    }
}

bool RobotsRules::is_allowed(const std::string& path) const {
    for (const auto & disallowed_path : disallowed_paths)
    {
        if (path.rfind(disallowed_path, 0) == 0)
        {
            return false;
        }
    }
    return true;
}