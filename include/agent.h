#ifndef AGENT_CPP_H
#define AGENT_CPP_H

#include <vector>

#include "directive.h"


namespace Rep
{

    class Agent
    {
    public:
        /* The type for the delay. */
        typedef float delay_t;

        /**
         * Construct an agent.
         */
        Agent(): directives_(), delay_(-1.0), sorted_(true) {}

        /**
         * Add an allowed directive.
         */
        Agent& allow(const std::string& query);

        /**
         * Add a disallowed directive.
         */
        Agent& disallow(const std::string& query);

        /**
         * Set the delay for this agent.
         */
        Agent& delay(delay_t value) {
            delay_ = value;
            return *this;
        }

        /**
         * Return the delay for this agent.
         */
        delay_t delay() const { return delay_; }

        /**
         * A vector of the directives, in priority-sorted order.
         */
        const std::vector<Directive>& directives() const;

        /**
         * Return true if the URL (either a full URL or a path) is allowed.
         */
        bool allowed(const std::string& path) const;

        std::string str() const;

        /**
         * Canonically escape the provided query for matching purposes.
         */
        static std::string escape(const std::string& query);

    private:
        mutable std::vector<Directive> directives_;
        delay_t delay_;
        mutable bool sorted_;
    };
}

#endif
