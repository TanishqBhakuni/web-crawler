#include "url_utils.hpp"

std::string get_domain(const std::string &url)
{
    const size_t protocol_end_pos = url.find("://");
    if (protocol_end_pos == std::string::npos)
    {
        return " ";
    }

    const size_t domain_start_pos = protocol_end_pos + 3;

    size_t domain_end_pos = url.find('/', domain_start_pos);

    if (domain_end_pos == std::string::npos)
    {
        return url;
    }

    return url.substr(0, domain_end_pos);
}