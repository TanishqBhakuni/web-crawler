#include "parser.hpp"
#include<iostream>
#include<gumbo.h>
#include<vector>
#include<string>

static std::string normalize_url(const std::string& url, const std::string& base_url){
    //1. Basic validation and filtering
    if(href.empty() || href.find("mailto: ") == 0 || href.find("javascript: ") == 0 || href.find("tel:") == 0){
        return "";
    }

    //2. Handle fragment URL's
    if(href[0] == '#'){
        return "";
    }

    //3. Parse the base URL to get scheme and domain 
    //We'll find the position of "://" to seperate the scheme

     size_t scheme_end_pos = base_url.find("://");
     if(scheme_end_pos == std::string::npos){
        return "";
    }

     //Then find the first '/' after the scheme to get the domain. 
     size_t domain_start_pos = base_url.find('/', scheme_end_pos + 3);
     std::string base_scheme = base_url.substr(0, scheme_end_pos);
     std::string base_domain = base_url.substr(0, domain_end_pos);

     std::string resolved_url;
     
     //4. Check for different URL types
    if (href.find("://") != std::string::npos){
        resolved_url = href;
    } 
    else if (href.rfind("//", 0) == 0){
        resolved_url = base_scheme + ":" + href;
    } 
    else if (href[0] == '/'){
        resolved_url = base_domain + href;
    } 
    else{
        size_t last_slash_pos = base_url.rfind('/');
        if (last_slash_pos > scheme_end_pos + 2) {
            resolved_url = base_url.substr(0, last_slash_pos + 1) + href;
        } 
        else {
            resolved_url = base_domain + "/" + href;
        }
    }

    // 5. Remove any fragment identifier
    size_t fragment_pos = resolved_url.find('#');
    if (fragment_pos != std::string::npos) {
        return resolved_url.substr(0, fragment_pos);
    }

    return resolved_url;

}

static void search_for_links(GumboNode* node, std::vector<std::string>& links, const std::string& base_url){
    if (node -> type != GUMBO_NODE_ELEMENT){
        return;
    }

    GumboElement& element = node->v.element;

    if (element.tag == GUMBO_TAG_A){
        
        GumboAttribute* href_attr = gumbo_get_attribute(&element.attributes, "href");

        if(href_attr){
            std::string raw_href = href_attr -> value;

            std::string normalized = normalize_url(raw_href, base_url);

            if(!normalized.empty()){
                links.push_back(normalized);
            }
        }
    }

    for(unsigned int i=0; i<element.children.length; ++i){
        search_for_links(static_cast<GumboNode*>(element.children.data[i]), links, base_url);
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
    search_for_links(output -> root, links, base_url);

    std::cout<<"HTML parsing successful. Root node type: " << output -> root -> type << std::endl;

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return links;
}