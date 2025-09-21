#include <iostream>
// #include <cpr/cpr.h>
#include <vector>
#include <string>
#include "parser.hpp"

void test_parser(){
    const std::string base_url = "https://test.com/path/";
    const std::string test_html = R"html(
    <!DOCTYPE html>
    <html>
    <body>
        <h1>My Test Page</h1>
        <p>A collection of links for our crawler to find.</p>
        <ul>
            <li><a href="https://test.com/page1.html">Absolute Link</a></li>
            <li><a href="/page2.html">Root-Relative Link</a></li>
            <li><a href="subdirectory/page3.html">Path-Relative Link</a></li>
            <li><a href="https://test.com/page4.html#section2">Fragment Link</a></li>
            <li><a href="https://www.external-site.org/index">External Link</a></li>
            <li><a href="mailto:user@test.com">Mail Link</a></li>
            <li><a href="/page2.html">Duplicate Root-Relative</a></li>
            <li><a href="//cdn.test.com/script.js">Protocol-Relative Link</a></li>
            <li><a href="#top">Top of Page</a></li>
        </ul>
    </body>
    </html>)html";

    std::cout << "--- Running Parser Test ---" << std::endl;
    std::cout << "Base URL: " << base_url << std::endl;
    std::cout << "---------------------------" << std::endl;

    std::vector<std::string> found_links = find_links(test_html, base_url);

    std::cout << "Found " << found_links.size() << " valid links:" << std::endl;

    for (const auto& link : found_links) {
        std::cout << "  - " << link << std::endl;
    }
    
    std::cout << "--- Parser Test Complete ---" << std::endl;
}




int main() {

    test_parser();
    // std::string target_url = "https://books.toscrape.com/";
    // std::cout << "Attempting to fetch URL: " << target_url << std::endl;

    // cpr::Response response = cpr::Get(cpr::Url{target_url});

    // if(response.status_code == 200){
        
    //     std::cout << "Request successful! Status code: " << response.status_code << std::endl;
        
    //     std::cout << "--- Begin HTML Content ---" << std::endl;

    //     std::cout << response.text << std::endl;

    //     std::cout << "--- End HTML Content ---" << std::endl;
    // }else{

    //     std::cout << "Failed to fetch the URL"<< response.status_code << std::endl;
    // }

    // if(response.error){
    //     std::cout << "Error: " << response.error.message << std::endl;
    // }

    return 0;
}