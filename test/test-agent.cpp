#include <gtest/gtest.h>

#include "agent.h"

TEST(AgentTest, Basic)
{
    Rep::Agent agent = Rep::Agent().allow("/").disallow("/foo");
    EXPECT_EQ(agent.directives().size(), 2);
}

TEST(AgentTest, ChecksAllowed)
{
    Rep::Agent agent = Rep::Agent().allow("/path");
    EXPECT_TRUE(agent.allowed("/path"));
    EXPECT_TRUE(agent.allowed("/elsewhere"));
}

TEST(AgentTest, HonorsLongestFirstPriority)
{
    Rep::Agent agent = Rep::Agent().disallow("/path").allow("/path/exception");
    EXPECT_TRUE(agent.allowed("/path/exception"));
    EXPECT_FALSE(agent.allowed("/path"));
}

TEST(AgentTest, RobotsTextAllowed)
{
    Rep::Agent agent = Rep::Agent().disallow("/robots.txt");
    EXPECT_TRUE(agent.allowed("/robots.txt"));
}

TEST(AgentTest, DisallowNone)
{
    Rep::Agent agent = Rep::Agent().disallow("");
    EXPECT_TRUE(agent.allowed("/anything"));
}

TEST(AgentTest, EscapedRule)
{
    Rep::Agent agent = Rep::Agent().disallow("/a%3cd.html");
    EXPECT_FALSE(agent.allowed("/a<d.html"));
    EXPECT_FALSE(agent.allowed("/a%3cd.html"));
}

TEST(AgentTest, UnescapedRule)
{
    Rep::Agent agent = Rep::Agent().disallow("/a<d.html");
    EXPECT_FALSE(agent.allowed("/a<d.html"));
    EXPECT_FALSE(agent.allowed("/a%3cd.html"));
}

TEST(AgentTest, EscapedRuleWildcard)
{
    Rep::Agent agent = Rep::Agent().disallow("/a%3c*");
    EXPECT_FALSE(agent.allowed("/a<d.html"));
    EXPECT_FALSE(agent.allowed("/a%3cd.html"));
}

TEST(AgentTest, UnescapedRuleWildcard)
{
    Rep::Agent agent = Rep::Agent().disallow("/a<*");
    EXPECT_FALSE(agent.allowed("/a<d.html"));
    EXPECT_FALSE(agent.allowed("/a%3cd.html"));
}

TEST(AgentTest, AcceptsFullUrl)
{
    Rep::Agent agent = Rep::Agent().disallow("/path;params?query");
    EXPECT_FALSE(agent.allowed(
        "http://userinfo@exmaple.com:10/path;params?query#fragment"));
}

TEST(AgentTest, QueryOnly)
{
    Rep::Agent agent = Rep::Agent().disallow("/?");
    EXPECT_TRUE(agent.allowed("/"));
    EXPECT_FALSE(agent.allowed("/?query"));
}

TEST(AgentTest, ParamsOnly)
{
    Rep::Agent agent = Rep::Agent().disallow("/;");
    EXPECT_TRUE(agent.allowed("/"));
    EXPECT_FALSE(agent.allowed("/;params"));
}
