#include <gtest/gtest.h>

#include "robots.h"

TEST(RobotsTest, DisallowFirst)
{
    // Disallow must be preceeded by a User-Agent line
    ASSERT_THROW(Rep::Robots("Disallow: /path"), std::invalid_argument);
}

TEST(RobotsTest, AllowFirst)
{
    // Allow must be preceeded by a User-Agent line
    ASSERT_THROW(Rep::Robots("Allow: /path"), std::invalid_argument);
}

TEST(RobotsTest, CrawlDelayFirst)
{
    // Crawl-delay must be preceeded by a User-Agent line
    ASSERT_THROW(Rep::Robots("Crawl-delay: 1"), std::invalid_argument);
}

TEST(RobotsTest, WellFormedCrawlDelay)
{
    std::string content =
        "User-agent: *\n"
        "Crawl-delay: 5.2\n";
    Rep::Robots robot(content);
    EXPECT_NEAR(robot.agent("any").delay(), 5.2, 0.000001);
}

TEST(RobotsTest, MalformedCrawlDelay)
{
    std::string content =
        "User-agent: *\n"
        "Crawl-delay: word\n";
    Rep::Robots robot(content);
    EXPECT_EQ(robot.agent("any").delay(), -1.0);
}

TEST(RobotsTest, HonorsDefaultAgent)
{
    std::string content =
        "User-agent: *\n"
        "Disallow: /tmp\n"
        "\n"
        "User-agent: other-agent\n"
        "Allow: /tmp\n";
    Rep::Robots robot(content);
    EXPECT_FALSE(robot.allowed("/tmp", "agent"));
    EXPECT_TRUE(robot.allowed("/path", "agent"));
}

TEST(RobotsTest, HonorsSpecificAgent)
{
    std::string content =
        "User-agent: *\n"
        "Disallow: /tmp\n"
        "\n"
        "User-agent: agent\n"
        "Allow: /tmp\n";
    Rep::Robots robot(content);
    EXPECT_TRUE(robot.allowed("/tmp", "agent"));
    EXPECT_TRUE(robot.allowed("/path", "agent"));
}

TEST(RobotsTest, Grouping)
{
    std::string content =
        "User-agent: one\n"
        "User-agent: two\n"
        "Disallow: /tmp\n";
    Rep::Robots robot(content);
    EXPECT_FALSE(robot.allowed("/tmp", "one"));
    EXPECT_FALSE(robot.allowed("/tmp", "two"));
}

TEST(RobotsTest, GroupingUnknownKeys)
{
    // When we encounter unknown keys, we should disregard any grouping that may have
    // happened between user agent rules.
    //
    // This is an example from the wild. Despite `Noindex` not being a valid directive,
    // we'll not consider the "*" and "ia_archiver" rules together.
    std::string content =
        "User-agent: *\n"
        "Disallow: /content/2/\n"
        "User-agent: *\n"
        "Noindex: /gb.html\n"
        "Noindex: /content/2/\n"
        "User-agent: ia_archiver\n"
        "Disallow: /\n";
    Rep::Robots robot(content);
    EXPECT_TRUE(robot.allowed("/foo", "agent"));
    EXPECT_FALSE(robot.allowed("/bar", "ia_archiver"));
}

TEST(RobotsTest, SeparatesAgents)
{
    std::string content =
        "User-agent: one\n"
        "Crawl-delay: 1\n"
        "\n"
        "User-agent: two\n"
        "Crawl-delay: 2\n";
    Rep::Robots robot(content);
    EXPECT_NE(robot.agent("one").delay(), robot.agent("two").delay());
}

TEST(RobotsTest, ExposesSitemaps)
{
    std::string content =
        "Sitemap: http://a.com/sitemap.xml\n"
        "Sitemap: http://b.com/sitemap.xml\n";
    Rep::Robots robot(content);
    std::vector<std::string> expected = {
        "http://a.com/sitemap.xml", "http://b.com/sitemap.xml"
    };
    EXPECT_EQ(robot.sitemaps(), expected);
}

TEST(RobotsTest, CaseInsensitivity)
{
    std::string content =
        "User-agent: agent\n"
        "Disallow: /path\n";
    Rep::Robots robot(content);
    EXPECT_FALSE(robot.allowed("/path", "agent"));
    EXPECT_FALSE(robot.allowed("/path", "aGeNt"));
}

TEST(RobotsTest, Empty)
{
    std::string content;
    Rep::Robots robot(content);
    EXPECT_TRUE(robot.sitemaps().empty());
    EXPECT_TRUE(robot.allowed("/", "agent"));
}

TEST(RobotsTest, Comments)
{
    std::string content = 
        "User-Agent: *  # comment saying it's the default agent\n"
        "Disallow: /\n";
    Rep::Robots robot(content);
    EXPECT_FALSE(robot.allowed("/path", "agent"));
}

TEST(RobotsTest, AcceptsFullUrl)
{
    std::string content = 
        "User-Agent: agent\n"
        "Disallow: /path;params?query\n";
    Rep::Robots robot(content);
    EXPECT_FALSE(robot.allowed(
        "http://userinfo@exmaple.com:10/path;params?query#fragment", "agent"));
}

TEST(RobotsTest, SkipMalformedLine)
{
    std::string content =
        "User-Agent: agent\n"
        "Disallow /no/colon/in/this/line\n";
    Rep::Robots robot(content);
    EXPECT_TRUE(robot.allowed("/no/colon/in/this/line", "agent"));
}

TEST(RobotsTest, RfcExample)
{
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
    Rep::Robots robot(content);

    // The unhip bot
    EXPECT_FALSE(robot.allowed("/", "unhipbot"));
    EXPECT_FALSE(robot.allowed("/index.html", "unhipbot"));
    EXPECT_TRUE(robot.allowed("/robots.txt", "unhipbot"));
    EXPECT_FALSE(robot.allowed("/server.html", "unhipbot"));
    EXPECT_FALSE(robot.allowed("/services/fast.html", "unhipbot"));
    EXPECT_FALSE(robot.allowed("/services/slow.html", "unhipbot"));
    EXPECT_FALSE(robot.allowed("/orgo.gif", "unhipbot"));
    EXPECT_FALSE(robot.allowed("/org/about.html", "unhipbot"));
    EXPECT_FALSE(robot.allowed("/org/plans.html", "unhipbot"));
    EXPECT_FALSE(robot.allowed("/%7Ejim/jim.html", "unhipbot"));
    EXPECT_FALSE(robot.allowed("/%7Emak/mak.html", "unhipbot"));

    // The webcrawler agent
    EXPECT_TRUE(robot.allowed("/", "webcrawler"));
    EXPECT_TRUE(robot.allowed("/index.html", "webcrawler"));
    EXPECT_TRUE(robot.allowed("/robots.txt", "webcrawler"));
    EXPECT_TRUE(robot.allowed("/server.html", "webcrawler"));
    EXPECT_TRUE(robot.allowed("/services/fast.html", "webcrawler"));
    EXPECT_TRUE(robot.allowed("/services/slow.html", "webcrawler"));
    EXPECT_TRUE(robot.allowed("/orgo.gif", "webcrawler"));
    EXPECT_TRUE(robot.allowed("/org/about.html", "webcrawler"));
    EXPECT_TRUE(robot.allowed("/org/plans.html", "webcrawler"));
    EXPECT_TRUE(robot.allowed("/%7Ejim/jim.html", "webcrawler"));
    EXPECT_TRUE(robot.allowed("/%7Emak/mak.html", "webcrawler"));

    // The excite agent
    EXPECT_TRUE(robot.allowed("/", "excite"));
    EXPECT_TRUE(robot.allowed("/index.html", "excite"));
    EXPECT_TRUE(robot.allowed("/robots.txt", "excite"));
    EXPECT_TRUE(robot.allowed("/server.html", "excite"));
    EXPECT_TRUE(robot.allowed("/services/fast.html", "excite"));
    EXPECT_TRUE(robot.allowed("/services/slow.html", "excite"));
    EXPECT_TRUE(robot.allowed("/orgo.gif", "excite"));
    EXPECT_TRUE(robot.allowed("/org/about.html", "excite"));
    EXPECT_TRUE(robot.allowed("/org/plans.html", "excite"));
    EXPECT_TRUE(robot.allowed("/%7Ejim/jim.html", "excite"));
    EXPECT_TRUE(robot.allowed("/%7Emak/mak.html", "excite"));

    // All others
    EXPECT_FALSE(robot.allowed("/", "anything"));
    EXPECT_FALSE(robot.allowed("/index.html", "anything"));
    EXPECT_TRUE(robot.allowed("/robots.txt", "anything"));
    EXPECT_TRUE(robot.allowed("/server.html", "anything"));
    EXPECT_TRUE(robot.allowed("/services/fast.html", "anything"));
    EXPECT_TRUE(robot.allowed("/services/slow.html", "anything"));
    EXPECT_FALSE(robot.allowed("/orgo.gif", "anything"));
    EXPECT_TRUE(robot.allowed("/org/about.html", "anything"));
    EXPECT_FALSE(robot.allowed("/org/plans.html", "anything"));
    EXPECT_FALSE(robot.allowed("/%7Ejim/jim.html", "anything"));
    EXPECT_TRUE(robot.allowed("/%7Emak/mak.html", "anything"));
}
