#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

std::vector<std::string> find_links(const std::string& html_body, const std::string& base_url);

#endif