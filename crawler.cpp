#include <iostream>
#include "crawler.hpp"
#include <cpr/cpr.h>
#include "parser.hpp"
#include <thread>
#include <chrono>

const std::string POISON_PILL = "POISON_PILL_SHUTDOWN_SIGNAL";

Crawler::Crawler()
{
    std::cout << "Crawler object created. Ready to crawl !" << std::endl;
}

Crawler::~Crawler()
{
    std::cout << "Crawler object destroyed. Crawl finished !" << std::endl;
}

void Crawler::start(const std::string &seed_url, int max_depth, int num_threads)
{
    this->max_depth_limit = max_depth;
    std::cout << "Starting crawl with the seed URL: " << seed_url << " and max depth " << max_depth << std::endl;

    urls_to_visit.push({seed_url, 0});

    workers.reserve(num_threads);
    std::cout << "Launching " << num_threads << " worker threads... " << std::endl;

    for (int i = 0; i < num_threads; ++i)
    {
        workers.emplace_back([this]
                             { this->worker(); });
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (urls_to_visit.empty() && active_workers.load() == 0)
        {
            std::cout << "Manager: Crawl Finished ! Sending shutdown signals." << std::endl;

            for (int i = 0; i < num_threads; ++i)
            {

                urls_to_visit.push({POISON_PILL, 0});
            }

            break;
        }
    }

    for (auto &worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
    std::cout << "All worker threads have completed." << std::endl;
}

void Crawler::worker()
{
    while (true)
    {
        auto current_item = urls_to_visit.wait_and_pop();
        std::string current_url = current_item.first;

        if (current_url == POISON_PILL)
        {
            std::cout << "Thread [" << std::this_thread::get_id() << "] received shutdown signal. Terminating." << std::endl;
            break;
        }

        active_workers++;

        int current_depth = current_item.second;

        if (urls_visited.add(current_url))
        {
            std::cout << "Thread [" << std::this_thread::get_id() << "] processing (depth " << current_depth << "): "
                      << urls_visited.size() << " visited. URL: " << current_url << std::endl;

            cpr::Response response = cpr::Get(cpr::Url{current_url});

            if (response.error)
            {
                std::cerr << "Network error: " << response.error.message << std::endl;
            }
            else if (response.status_code == 200)
            {
                auto new_links = find_links(response.text, current_url);
                if (current_depth < max_depth_limit)
                {
                    for (const auto &link : new_links)
                    {
                        if (!urls_visited.contains(link))
                        {
                            urls_to_visit.push({link, current_depth + 1});
                        }
                    }
                }
            }
            else
            {
                std::cerr << "HTTP error: " << response.status_code << std::endl;
            }
        }

        active_workers--;
    }
}