#include <algorithm>
#include <sstream>
#include <regex>

#include "url.h"

#include "agent.h"
#include "directive.h"

namespace Rep
{
    Agent& Agent::allow(const std::string& query)
    {
        directives_.push_back(Directive(escape(query), true));
        sorted_ = false;
        return *this;
    }

    Agent& Agent::disallow(const std::string& query)
    {
        if (query.empty())
        {
            // Special case: "Disallow:" means "Allow: /"
            directives_.push_back(Directive(query, true));
        }
        else
        {
            directives_.push_back(Directive(escape(query), false));
        }
        sorted_ = false;
        return *this;
    }

    const std::vector<Directive>& Agent::directives() const
    {
        if (!sorted_)
        {
            std::sort(directives_.begin(), directives_.end(), [](const Directive& a, const Directive& b) {
                return b.priority() < a.priority();
            });
            sorted_ = true;
        }
        return directives_;
    }

    bool Agent::allowed(const std::string& query) const
    {
        std::string path(escape(query));

        if (path.compare("/robots.txt") == 0)
        {
            return true;
        }

        for (auto directive : directives())
        {
            if (directive.match(path))
            {
                return directive.allowed();
            }
        }
        return true;
    }

    std::string Agent::str() const
    {
        std::stringstream out;
        out << '[';
        auto begin = directives().begin();
        auto end = directives().end();
        if (begin != end)
        {
            out << "Directive(" << begin->str() << ')';
            ++begin;
        }
        for (; begin != end; ++begin)
        {
            out << ", Directive(" << begin->str() << ')';
        }
        out << ']';
        return out.str();
    }

    std::string Agent::escape(const std::string& query)
    {
      	std::regex escaped_slash ("%2[Ff]");
      	std::regex escaped_newline ("\n");
      	std::string result;

      	result = std::regex_replace(query, escaped_slash, "\n");

      	std::string url = Url::Url(result).defrag().escape().fullpath();

      	return std::regex_replace(url, escaped_newline, "%2F");
    }
}
