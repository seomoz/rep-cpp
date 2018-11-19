#include <gtest/gtest.h>

#include "agent.h"

TEST(AgentTest, Basic)
{
    Rep::Agent agent = Rep::Agent("a.com").allow("/").disallow("/foo");
    auto expectedSize = static_cast<std::vector<Rep::Directive>::size_type>(2);
    EXPECT_EQ(agent.directives().size(), expectedSize);
}

TEST(AgentTest, ChecksAllowed)
{
    Rep::Agent agent = Rep::Agent("a.com").allow("/path");
    EXPECT_TRUE(agent.allowed("/path"));
    EXPECT_TRUE(agent.allowed("/elsewhere"));
}

TEST(AgentTest, IgnoresExternalDisallow)
{
    Rep::Agent agent = Rep::Agent("a.com")
        .allow("/path")
        .disallow("http://b.com/external");
    EXPECT_TRUE(agent.allowed("/path"));
    EXPECT_TRUE(agent.allowed("/external"));
}

TEST(AgentTest, IgnoresExternalAllow)
{
    Rep::Agent agent = Rep::Agent("a.com")
        .disallow("/path")
        .allow("http://b.com/path/exception");
    EXPECT_FALSE(agent.allowed("/path"));
    EXPECT_FALSE(agent.allowed("/path/exception"));
}

TEST(AgentTest, NeverExternalAllowed)
{
    Rep::Agent agent = Rep::Agent("a.com");
    EXPECT_FALSE(agent.allowed("http://b.com/"));
}

TEST(AgentTest, HonorsLongestFirstPriority)
{
    Rep::Agent agent = Rep::Agent("a.com")
        .disallow("/path")
        .allow("/path/exception");
    EXPECT_TRUE(agent.allowed("/path/exception"));
    EXPECT_FALSE(agent.allowed("/path"));
}

TEST(AgentTest, RobotsTextAllowed)
{
    Rep::Agent agent = Rep::Agent("a.com").disallow("/robots.txt");
    EXPECT_TRUE(agent.allowed("/robots.txt"));
}

TEST(AgentTest, DisallowNone)
{
    Rep::Agent agent = Rep::Agent("a.com").disallow("");
    EXPECT_TRUE(agent.allowed("/anything"));
}

TEST(AgentTest, MiddleWildcard)
{
    Rep::Agent agent = Rep::Agent("a.com").disallow("/test*foo");
    EXPECT_FALSE(agent.allowed("/testfoo"));
    EXPECT_FALSE(agent.allowed("/testafoo"));
    EXPECT_FALSE(agent.allowed("/testaasdffoo"));
    EXPECT_FALSE(agent.allowed("/test/foo"));
    EXPECT_TRUE(agent.allowed("/testfo"));
    EXPECT_TRUE(agent.allowed("/estfoo"));
}

TEST(AgentTest, EscapedRule)
{
    Rep::Agent agent = Rep::Agent("a.com").disallow("/a%3cd.html");
    EXPECT_FALSE(agent.allowed("/a<d.html"));
    EXPECT_FALSE(agent.allowed("/a%3cd.html"));
}

TEST(AgentTest, UnescapedRule)
{
    Rep::Agent agent = Rep::Agent("a.com").disallow("/a<d.html");
    EXPECT_FALSE(agent.allowed("/a<d.html"));
    EXPECT_FALSE(agent.allowed("/a%3cd.html"));
}

TEST(AgentTest, EscapedRuleWildcard)
{
    Rep::Agent agent = Rep::Agent("a.com").disallow("/a%3c*");
    EXPECT_FALSE(agent.allowed("/a<d.html"));
    EXPECT_FALSE(agent.allowed("/a%3cd.html"));
}

TEST(AgentTest, UnescapedRuleWildcard)
{
    Rep::Agent agent = Rep::Agent("a.com").disallow("/a<*");
    EXPECT_FALSE(agent.allowed("/a<d.html"));
    EXPECT_FALSE(agent.allowed("/a%3cd.html"));
}

TEST(AgentTest, AcceptsFullUrl)
{
    Rep::Agent agent = Rep::Agent("a.com").disallow("/path;params?query");
    EXPECT_FALSE(agent.allowed(
        "http://userinfo@exmaple.com:10/path;params?query#fragment"));
}

TEST(AgentTest, QueryOnly)
{
    Rep::Agent agent = Rep::Agent("a.com").disallow("/?");
    EXPECT_TRUE(agent.allowed("/"));
    EXPECT_FALSE(agent.allowed("/?query"));
}

TEST(AgentTest, ParamsOnly)
{
    Rep::Agent agent = Rep::Agent("a.com").disallow("/;");
    EXPECT_TRUE(agent.allowed("/"));
    EXPECT_FALSE(agent.allowed("/;params"));
}

TEST(AgentTest, Str)
{
    Rep::Agent agent("a.com");
    agent.disallow("/foo");
    agent.allow("/bar");
    EXPECT_EQ("[Directive(Disallow: /foo), Directive(Allow: /bar)]", agent.str());
}

TEST(AgentTest, StrWithDelay)
{
    Rep::Agent agent("a.com");
    agent.delay(1.0);
    agent.disallow("/foo");
    agent.allow("/bar");
    EXPECT_EQ("Crawl-Delay: 1 [Directive(Disallow: /foo), Directive(Allow: /bar)]", agent.str());
}
