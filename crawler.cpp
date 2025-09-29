#include <iostream>
#include <cpr/cpr.h>
#include <thread>
#include <chrono>

#include "crawler.hpp"
#include "parser.hpp"
#include "url_utils.hpp"
#include "robots_parser.hpp"

const std::string POISON_PILL = "POISON_PILL_SHUTDOWN_SIGNAL";

Crawler::Crawler()
    :output_file_("results.txt")
{
    if (!output_file_.is_open()) {
        throw std::runtime_error("Fatal: Could not open results.txt for writing.");
    }

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
    this->start_time_ = std::chrono::high_resolution_clock::now();
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

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time_);

    int pages_crawled_count = pages_crawled_.load();
    int links_found_count = links_found_.load();
    int http_errors_count = http_errors_.load();

    std::cout << "\n----------------------------------------" << std::endl;
    std::cout << "          Crawl Complete" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Total Crawl Time: " << std::fixed << std::setprecision(2) << duration.count() << " seconds" << std::endl;
    std::cout << "Pages Crawled:    " << pages_crawled_count << std::endl;
    std::cout << "Links Found:      " << links_found_count << std::endl;
    std::cout << "HTTP Errors:      " << http_errors_count << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Results saved to: results.txt" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
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

        RobotsRules rules;
        if (!robots_rules_cache_.try_get(domain, rules))
        {
            std::cout << "  -> No rules for '" << domain << "' in cache. Fetching robots.txt." << std::endl;

            std::string robots_txt_url = domain + "/robots.txt";

            cpr::Response robots_resp = cpr::Get(cpr::Url{robots_txt_url});

            RobotsRules new_rules;
            if (robots_resp.status_code == 200)
            {
                new_rules.parse_content(robots_resp.text);
                std::cout << "  -> Successfully parsed robots.txt for '" << domain << "'." << std::endl;
            }
            else
            {
                std::cout << "  -> No robots.txt found for '" << domain << "' (status " << robots_resp.status_code << "). Assuming all paths are allowed." << std::endl;
            }

            robots_rules_cache_.add(domain, new_rules);
            rules = new_rules;
        }
        else
        {
            std::cout << "  -> Found rules for '" << domain << "' in cache." << std::endl;
        }

        std::string path = current_url.substr(domain.length());
        if (path.empty())
        {
            path = "/";
        }

        if (!rules.is_allowed(path))
        {
            std::cout << "  -> Path '" << path << "' is DISALLOWED by robots.txt. Skipping." << std::endl;
            active_workers--;
            continue;
        }

        std::cout << "  -> Path '" << path << "' is ALLOWED by robots.txt. Proceeding with fetch." << std::endl;

        std::this_thread::sleep_for(politeness_delay);
        cpr::Response response = cpr::Get(cpr::Url{current_url});

        if (response.status_code == 200)
        {
            pages_crawled_++;

            {
                std::lock_guard<std::mutex> guard(file_writer_mutex_);
                output_file_ << current_url << std::endl;
            }

            std::vector<std::string> new_links = find_links(response.text, current_url);

            links_found_ += static_cast<int>(new_links.size());

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
        else if (!response.error)
        {
            std::cerr << "  -> Failed with HTTP status code: " << response.status_code
                      << " for URL: " << current_url << std::endl;
            http_errors_++; 
        }

        if (response.error)
        {
            std::cerr << "  -> Network Error: " << response.error.message
                      << " for URL: " << current_url << std::endl;
        }
        
        active_workers--;
    }
}