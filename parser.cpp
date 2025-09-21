#include "parser.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <gumbo.h>
#include <vector>
#include <string>

static std::string normalize_url(const std::string& href, const std::string& base_url){
    //1. Basic validation and filtering
    if(href.empty() || href.find("mailto:") == 0 || href.find("javascript:") == 0 || href.find("tel:") == 0){
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
     size_t domain_end_pos = base_url.find('/', scheme_end_pos + 3);
     if(domain_end_pos == std::string::npos){
        domain_end_pos = base_url.length();
    }
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
    // 1. Base Case & Filtering: Check for null node and element nodes only.
    if (node == nullptr || node->type != GUMBO_NODE_ELEMENT){
        return;
    }

    GumboElement& element = node->v.element;

    //2. Identification: Is this an anchor tag ?
    if (element.tag == GUMBO_TAG_A){
        
        //3. Extraction: Get the href attribute.
        GumboAttribute* href_attr = gumbo_get_attribute(&element.attributes, "href");
        //Check if the href attribute exists.        
        if(href_attr){
            std::string raw_href = href_attr -> value;

            //4. Normalization: Convert to absolute URL.
            std::string normalized = normalize_url(raw_href, base_url);

            //Storage: If the URL is valid, add it to our results vector.
            if(!normalized.empty()){
                links.push_back(normalized);
            }
        }
    }

    // 6. Traversal: Continue the search into the element's children,
    // passing the shared 'links' vector and 'base_url' context along.
    for(unsigned int i=0; i<element.children.length; ++i){
        GumboNode* child = static_cast<GumboNode*>(element.children.data[i]);
        if (child != nullptr) {
            search_for_links(child, links, base_url);
        }
    }
}

std::vector<std::string> find_links(const std::string& html_body, const std::string& base_url){
    // 1. ACQUIRE a resource from the C library. Memory is allocated on the heap.
    GumboOutput* output = gumbo_parse(html_body.c_str());

    if(output == nullptr){
        std::cerr << "Error: Failed to parse HTML" << std::endl;
        return {};
    }

    std::vector<std::string> links;
    if(output->root != nullptr) {
        search_for_links(output->root, links, base_url);
        std::cout<<"HTML parsing successful. Root node type: " << output->root->type << std::endl;
    }

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return links;
}