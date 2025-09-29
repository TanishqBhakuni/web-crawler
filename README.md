# CppWebCrawler

A simple, configurable, multithreaded web crawler written in modern C++17. It fetches pages over HTTP(S) using cpr (libcurl), parses out links with Gumbo (HTML5 parser), respects robots.txt on a per-domain basis, and traverses to a configurable depth while being polite via a per-thread delay.

## Highlights

- Multithreaded workers with a thread-safe queue and visited set
- Per-domain robots.txt fetch and cache with basic allow/disallow logic
- Link extraction with the Gumbo HTML5 parser
- Politeness delay between requests per worker
- Simple counters and end-of-run stats (pages crawled, links discovered, HTTP errors)
- Minimal dependencies and a single CMake build

## Quick Start

### Prerequisites
- CMake 3.15+
- A C++17 compiler (MSVC, GCC, or Clang)
- Git (optional, to clone the repo)

### Build

1) Configure and build (out-of-source is recommended):

```powershell
# from repository root
mkdir build; cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
```

This will build the `crawler` executable and fetch/build third-party dependencies (cpr, gumbo) from the `external/` directories.

### Run

```powershell
# From the build directory (Windows)
./crawler.exe --url https://example.com --depth 2 --threads 8 --delay 1000
```

Arguments:
- --url, -u: Seed URL to start crawling (required)
- --depth, -d: Maximum crawl depth (default: 2)
- --threads, -t: Number of worker threads (default: hardware concurrency)
- --delay, -l: Politeness delay per request in milliseconds (default: 1000)

Example:
```powershell
./crawler.exe -u https://news.ycombinator.com -d 1 -t 8 -l 500
```

On completion, the program prints a summary and basic crawl stats to the console. It also writes each successfully fetched URL to a `results.txt` file (one per line).

## Sample Output

```
----------------------------------------
		  Crawl Complete
----------------------------------------
Total Crawl Time: 3.42 seconds
Pages Crawled:    57
Links Found:      423
HTTP Errors:      4
----------------------------------------

=== Crawl Stats ===
Pages crawled: 57
Links discovered: 423
HTTP errors: 4
```

## Project Layout

- main.cpp: CLI handling (cxxopts) and program entry point
- crawler.*: The `Crawler` class, worker threads, queue/visited set, robots cache, counters
- parser.*: HTML parsing and link extraction using Gumbo
- robots_parser.*: Minimal robots.txt parsing (User-agent, Disallow) with caching in `robots_cache.*`
- thread_safe_structures.hpp: Simple thread-safe queue and set primitives
- url_utils.*: Tiny helpers for URL domain parsing
- external/: Third-party libraries (cpr, gumbo)
- libs/: Header-only third-party (cxxopts)

## Implementation Notes

- Robots.txt: For each new domain encountered, `robots.txt` is fetched once and cached for subsequent requests. The parser handles `User-agent` (case-insensitive) and `Disallow` directives, ignores comments and blank lines, and applies only the `*` section (or a section that mentions this crawler by name).
- Politeness: Each worker sleeps for the configured delay before issuing the GET request. This keeps request rates reasonable across domains.
- Concurrency: Access to the queue and visited set is protected by internal mutexes. The per-worker accounting (`active_workers`) coordinates shutdown when the queue is empty and workers are idle.
- Counters: The crawler tracks pages fetched with HTTP 200, links discovered, and HTTP errors (non-2xx where a response was received). Transport-level errors are logged but not counted separately.

## Developing

- Standard out-of-source CMake build. You can add additional libraries or tests by editing `CMakeLists.txt` and creating new source files.
- To keep output clean for commits and LinkedIn posts, a `.gitignore` is included to exclude the build directory and generated binaries.

## Future Improvements

- More complete robots.txt handling (Allow, Crawl-delay, sitemaps, wildcards, precedence)
- Domain-level politeness and request scheduling
- Content-type filtering, canonicalization, and deduplication
- Persistent storage of results (e.g., sqlite) and structured reporting (JSON)
- Unit tests for parsing utilities

## License

This project vendors dependencies under their respective licenses. The crawler source itself is provided under the MIT License. See LICENSE if added, or customize to your preference.