#include <iostream>
#include <regex>
#include <string>
#include <sstream>

namespace darllen
{

template <typename It, typename Alloc, typename Arg>
bool read_impl(const std::match_results<It, Alloc>& matches, size_t current,
        size_t size, Arg& arg)
{
    if (current < size)
    {
        // TODO: Reduce copies
        // TODO: Handle different character types
        std::istringstream input(matches[current].str());
        input >> arg;
        // TODO: return false on failead read
        return true;
    }
    return false;
}

template <typename It, typename Alloc, typename Arg, typename... Args>
bool read_impl(const std::match_results<It, Alloc>& matches, size_t current,
        size_t size, Arg& arg, Args&... args)
{
    if (current < size)
    {
        // TODO: Reduce copies
        // TODO: Handle different character types
        std::istringstream input(matches[current].str());
        input >> arg;
        // TODO: return false on failead read
        return read_impl(matches, current + 1, size, args...);
    }
    return false;
}

template <typename It, typename Alloc, typename... Args>
bool read_impl(const std::match_results<It, Alloc>& matches, size_t current,
        size_t size, Args&... args)
{
}

// TODO: Template the match_results allocator
// TODO: Match flags
template <typename It, typename CharT, typename Traits,
          typename... Args>
bool read(const std::basic_regex<CharT, Traits>& format, It begin, It end,
          Args&... args)
{
    std::match_results<It> matches;
    if (std::regex_match(begin, end, matches, format))
    {
        return read_impl(matches, 1, matches.size(), args...);
    }
    return false;
}
}

int main(int argc, const char* argv[])
{
    std::string input(argc < 2? "The answer is 42!" : argv[1]);
    int a = 0;
    char e = '.';
    std::regex rx("The answer is ([[:digit:]]+)(.)");
    if (darllen::read(rx, begin(input), end(input), a, e))
    {
        std::cout << "Got: " << a << e << std::endl;
    }
    else
    {
        std::cout << "No match!" << std::endl;
    }
    return 0;
}
