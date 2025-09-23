#include<iostream>
#include"crawler.hpp"

Crawler::Crawler(){
    std::cout<<"Crawler object created. Ready to crawl !" << std::endl;
}

Crawler::~Crawler(){
    std::cout<<"Crawler object destroyed. Crawl finished !"<< std::endl;

}

void Crawler::start(const std::string& seed_url, int max_depth){
    std::cout<<"Starting crawl with the seed URL: " << seed_url << " and max depth " << max_depth << std::endl;
}