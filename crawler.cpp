#include<iostream>
#include"crawler.hpp"
#include <cpr/cpr.h>
#include "parser.hpp"

Crawler::Crawler(){
    std::cout<<"Crawler object created. Ready to crawl !" << std::endl;
}

Crawler::~Crawler(){
    std::cout<<"Crawler object destroyed. Crawl finished !"<< std::endl;

}

void Crawler::start(const std::string& seed_url, int max_depth){
    std::cout<<"Starting crawl with the seed URL: " << seed_url << " and max depth " << max_depth << std::endl;

    urls_to_visit.push({seed_url, 0});

    while (!urls_to_visit.empty()){
        
        auto current_item = urls_to_visit.front();
        urls_to_visit.pop();
        
        // Extract the URL and its depth from the pair.
        std::string current_url = current_item.first;
        int current_depth = current_item.second;
        
        if(urls_visited.count(current_url)){
            std::cout<<"Already visited" << current_url << "Skipping !" << std::endl;
            continue;
        }
        urls_visited.insert(current_url);
        std::cout << "Processing (depth " << current_depth << "): " << std::endl;

        cpr::Response response = cpr::Get(cpr::Url{current_url});

        if(response.error){
            std::cerr << "-> Network Error: " << response.error.message << std::endl;
            continue;
        }
        if(response.status_code == 200){
            std::cout << " -> Success (" << response.status_code << ")" << std::endl;


            std::vector<std::string> new_links = find_links(response.text, current_url);
            std::cout << "  -> Found " << new_links.size() << " links on the page." << std::endl;

            if(current_depth < max_depth){
                for(const auto& link: new_links){
                    if(urls_visited.count(link) == 0){
                        urls_to_visit.push({link, current_depth + 1});
                    }
                }
            }
            else{
                std::cerr << " -> Faild with HTTP status code" << response.status_code << std::endl;
            }
        }   
    }
}