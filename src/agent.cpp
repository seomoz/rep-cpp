#include <algorithm>

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

    std::string Agent::escape(const std::string& query)
    {
        // Special case: treat multiple leading slashes as one.
        std::string query_;
        if (query.size() > 1 && query[0] == '/')
        {
            query_.reserve(query.size());
            query_.append(1, '/');
            bool leading_slashes_consumed = false;
            for (auto character : query)
            {
                if (leading_slashes_consumed)
                {
                    query_.append(1, character);
                }
                else if (character != '/')
                {
                    leading_slashes_consumed = true;
                    query_.append(1, character);
                }
            }
        }
        else
        {
            query_.assign(query);
        }

        return Url::Url(query_).defrag().escape().fullpath();
    }
}
