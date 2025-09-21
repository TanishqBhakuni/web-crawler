#include "parser.hpp"
#include<iostream>
#include<gumbo.h>

static void search_for_links(GumboNode* node, std::vector<std::string>& links){
    if (node -> type != GUMBO_NODE_ELEMENT){
        return;
    }

    GumboElement& element = node->v.element;

    if (element.tag == GUMBO_TAG_A){
        
        GumboAttribute* href_attr = gumbo_get_attribute(&element.attributes, "href");

        if(href_attr){
            std::string href_value = href_attr -> value;
            std::cout<< "Found a link to: " << href_value << std::endl;
        }
    }

    for(unsigned int i=0; i<element.children.length; ++i){
        search_for_links(static_cast<GumboNode*>(element.children.data[i]), links);
    }
}

std::vector<std::string> find_links(const std::string& html_body, const std::string& base_url){
    GumboOutput* output = gumbo_parse(html_body.c_str());

    if(output -> status != GUMBO_STATUS_OK){
        std::cerr << "Error: Failed to parse HTML" << std::endl;
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        return {};
    }

    std::vector<std::string> links;
    search_for_links(output -> root, links);

    std::cout<<"HTML parsing successful. Root node type: " << output -> root -> type << std::endl;

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return links;
}