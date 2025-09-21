#include "parser.hpp"
#include<iostream>
#include<gumbo.h>

static void search_for_links(GumboNode* node, std::vector<std::string>& links){
    if (node -> type != GUMBO_NODE_ELEMENT){
        return;
    }
}

std::vector<std::string> find_links(const std::string& html_body, const std::string& base_url){
    GumboOutput* output = gumbo_parse(html_body.c_str());

    if(output -> status != GUMBO_STATUS_OK){
        std::cerr << "Failed to parse HTML" << std::endl;
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        return {};
    }

    std::cout<<"HTML parsing successful. Root node type: " << output -> root -> type << std::endl;

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return {};
}