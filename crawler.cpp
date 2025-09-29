#include <iostream>
#include <cpr/cpr.h>
#include <thread>
#include <chrono>

#include "crawler.hpp"
#include "parser.hpp"
#include "url_utils.hpp"

const std::string POISON_PILL = "POISON_PILL_SHUTDOWN_SIGNAL";

Crawler::Crawler()
{
    std::cout << "Crawler object created. Ready to crawl !" << std::endl;
}

Crawler::~Crawler()
{
    std::cout << "Crawler object destroyed. Crawl finished !" << std::endl;
}

void Crawler::start(const std::string &seed_url, int max_depth, int num_threads, int delay_ms)
{
    this->max_depth_limit = max_depth;
    this->politeness_delay = std::chrono::milliseconds(delay_ms);
    std::cout << "Starting crawl with the seed URL: " << seed_url << " and max depth " << max_depth << std::endl;

    urls_to_visit.push({seed_url, 0});

    workers.reserve(num_threads);
    std::cout << "Launching " << num_threads << " worker threads with a " << delay_ms << "ms delay between requests... " << std::endl;

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
            break;
        }

        if (!urls_visited.add(current_url))
        {
            continue;
        }

        active_workers++;

        int current_depth = current_item.second;

        std::cout << "Thread [" << std::this_thread::get_id() << "] processing (depth " << current_depth << "): "
                  << "URL: " << current_url << std::endl;

        std::string domain = get_domain(current_url);
        if (domain.empty())
        {
            std::cerr << "  -> Thread [" << std::this_thread::get_id() << "] could not parse domain from URL. Skipping." << std::endl;
            active_workers--;
            continue;
        }

        std::cout << "  -> Identified Domain: " << domain << std::endl;

        std::this_thread::sleep_for(politeness_delay);

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

        active_workers--;
    }
}