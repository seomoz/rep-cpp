#include <iostream>
#include <chrono>
#include <ctime>

#include "directive.h"
#include "robots.h"

/**
 * Run func() `count` times in each of `runs` experiments, where `name` provides a
 * meaningful description of the task being benchmarked. Prints out the time for each
 * run, the average time, and rate.
 */
template<typename Functor>
void bench(const std::string& name, size_t count, size_t runs, Functor func)
{
    std::cout << "Benchmarking " << name << " with " << count << " per run:" << std::endl;
    double total(0);
    for (size_t run = 0; run < runs; ++run)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t it = 0; it < count; ++it)
        {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        total += duration;
        std::cout << "    Run " << run << ": " << duration << " ms" << std::endl;
    }
    std::cout << "  Average: " << (total / runs) << " ms" << std::endl;
    std::cout << "     Rate: " << ((count * runs) / total) << " k-iter / s" << std::endl;
}

int main(int argc, char* argv[]) {

    size_t count = 1000000;
    size_t runs = 5;

    bench("directive basic parse", count, runs, []() {
        Rep::Directive("/basic/path", true);
    });

    bench("directive wildcard parse", count, runs, []() {
        Rep::Directive("/path/*/with/**/wildcards/*", true);
    });

    Rep::Directive directive("/basic/path", true);
    bench("directive basic check", count, runs, [directive]() {
        directive.match("/basic/path/other");
    });

    directive = Rep::Directive("/path/*/with/**/wildcards/*", true);
    bench("directive wildcard check", count, runs, [directive]() {
        directive.match("/path/is/with/a/few/wildcards/");
    });

    std::string content =
        "# /robots.txt for http://www.fict.org/\n"
        "# comments to webmaster@fict.org\n"
        "\n"
        "User-agent: unhipbot\n"
        "Disallow: /\n"
        "\n"
        "User-agent: webcrawler\n"
        "User-agent: excite\n"
        "Disallow:\n"
        "\n"
        "User-agent: *\n"
        "Disallow: /org/plans.html\n"
        "Allow: /org/\n"
        "Allow: /serv\n"
        "Allow: /~mak\n"
        "Disallow: /\n";
    bench("parse RFC", count / 10, runs, [content]() {
        Rep::Robots robot(content);
    });
}
