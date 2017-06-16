#include <gtest/gtest.h>

#include "directive.h"

TEST(DirectiveTest, BasicExact)
{
    std::string directive("/tmp");
    std::string example("/tmp");
    EXPECT_TRUE(Rep::Directive(directive, true).match(example));
}

TEST(DirectiveTest, BasicFileExtension)
{
    std::string directive("/tmp");
    std::string example("/tmp.html");
    EXPECT_TRUE(Rep::Directive(directive, true).match(example));
}

TEST(DirectiveTest, BasicDirectory)
{
    std::string directive("/tmp");
    std::string example("/tmp/a.html");
    EXPECT_TRUE(Rep::Directive(directive, true).match(example));
}

TEST(DirectiveTest, BasicDirectoryAndFile)
{
    std::string directive("/tmp/");
    std::string example("/tmp");
    EXPECT_FALSE(Rep::Directive(directive, true).match(example));
}

TEST(DirectiveTest, BasicDirectoryAndDirectory)
{
    std::string directive("/tmp/");
    std::string example("/tmp/");
    EXPECT_TRUE(Rep::Directive(directive, true).match(example));
}

TEST(DirectiveTest, BasicDirectoryAndDirectoryAndFile)
{
    std::string directive("/tmp/");
    std::string example("/tmp/a.html");
    EXPECT_TRUE(Rep::Directive(directive, true).match(example));
}

TEST(DirectiveTest, WildcardTest)
{
    std::vector<std::string> examples = {
        "/hello/how/are/you",
        "/hello/how/are/you/today",
        "/hello/how/are/yo/are/you",
    };
    std::vector<std::string> antiexamples = {
        "/hello/",
        "/hi/how/are/you"
    };
    std::string directive("/hello/*/are/you");
    Rep::Directive parsed(directive, true);
    for (auto example : examples)
    {
        EXPECT_TRUE(parsed.match(example)) <<
            example << " didn't match " << directive;
    }

    for (auto example : antiexamples)
    {
        EXPECT_FALSE(parsed.match(example)) <<
            example << " matched " << directive;
    }
}

TEST(DirectiveTest, MultipleWildcardTest)
{
    std::vector<std::string> examples = {
        "/this-test-is-a-simple-test",
        "/this-test-is-another-test-is-a-tricky-test"
    };
    std::vector<std::string> antiexamples = {
        "/this-test-is-a-mislead"
    };
    std::string directive("/this-*-is-a-*-test");
    Rep::Directive parsed(directive, true);
    for (auto example : examples)
    {
        EXPECT_TRUE(parsed.match(example)) <<
            example << " didn't match " << directive;
    }

    for (auto example : antiexamples)
    {
        EXPECT_FALSE(parsed.match(example)) <<
            example << " matched " << directive;
    }
}

TEST(DirectiveTest, Str)
{
    EXPECT_EQ("Allow: /foo", Rep::Directive("/foo", true).str());
    EXPECT_EQ("Disallow: /bar", Rep::Directive("/bar", false).str());
}

TEST(GoogleTest, EmptyAndWildcard)
{
    std::vector<std::string> examples = {
        "/",
        "/fish",
        "/fish.html",
        "/fish/salmon.html",
        "/fishheads",
        "/fishheads/yummy.html",
        "/fish.php?id=anything"
    };
    std::vector<std::string> directives = {
        "/",
        "/*"
    };
    for (auto directive : directives)
    {
        Rep::Directive parsed(directive, true);
        for (auto example : examples)
        {
            EXPECT_TRUE(parsed.match(example)) <<
                example << " didn't match " << directive;
        }
    }
}

TEST(GoogleTest, Prefix)
{
    std::vector<std::string> examples = {
        "/fish",
        "/fish.html",
        "/fish/salmon.html",
        "/fishheads",
        "/fishheads/yummy.html",
        "/fish.php?id=anything"
    };
    std::vector<std::string> antiexamples = {
        "/Fish.asp",
        "/catfish",
        "/?id=fish"
    };
    std::string directive("/fish");
    Rep::Directive parsed(directive, true);
    for (auto example : examples)
    {
        EXPECT_TRUE(parsed.match(example)) <<
            example << " didn't match " << directive;
    }

    for (auto example : antiexamples)
    {
        EXPECT_FALSE(parsed.match(example)) <<
            example << " matched " << directive;
    }
}

TEST(GoogleTest, TrailingWildcard)
{
    std::vector<std::string> examples = {
        "/fish",
        "/fish.html",
        "/fish/salmon.html",
        "/fishheads",
        "/fishheads/yummy.html",
        "/fish.php?id=anything"
    };
    std::vector<std::string> antiexamples = {
        "/Fish.asp",
        "/catfish",
        "/?id=fish"
    };
    std::string directive("/fish*");
    Rep::Directive parsed(directive, true);
    for (auto example : examples)
    {
        EXPECT_TRUE(parsed.match(example)) <<
            example << " didn't match " << directive;
    }

    for (auto example : antiexamples)
    {
        EXPECT_FALSE(parsed.match(example)) <<
            example << " matched " << directive;
    }
}

TEST(GoogleTest, Directory)
{
    std::vector<std::string> examples = {
        "/fish/",
        "/fish/?id=anything",
        "/fish/salmon.htm"
    };
    std::vector<std::string> antiexamples = {
        "/fish",
        "/fish.html",
        "/Fish/Salmon.asp"
    };
    std::string directive("/fish/");
    Rep::Directive parsed(directive, true);
    for (auto example : examples)
    {
        EXPECT_TRUE(parsed.match(example)) <<
            example << " didn't match " << directive;
    }

    for (auto example : antiexamples)
    {
        EXPECT_FALSE(parsed.match(example)) <<
            example << " matched " << directive;
    }
}

TEST(GoogleTest, WildcardExtension)
{
    std::vector<std::string> examples = {
        "/filename.php",
        "/folder/filename.php",
        "/folder/filename.php?parameters",
        "/folder/any.php.file.html",
        "/filename.php/"
    };
    std::vector<std::string> antiexamples = {
        "/",
        "/windows.PHP"
    };
    std::string directive("/*.php");
    Rep::Directive parsed(directive, true);
    for (auto example : examples)
    {
        EXPECT_TRUE(parsed.match(example)) <<
            example << " didn't match " << directive;
    }

    for (auto example : antiexamples)
    {
        EXPECT_FALSE(parsed.match(example)) <<
            example << " matched " << directive;
    }
}

TEST(GoogleTest, WildcardExtensionEnd)
{
    std::vector<std::string> examples = {
        "/filename.php",
        "/folder/filename.php"
    };
    std::vector<std::string> antiexamples = {
        "/filename.php?parameters",
        "/filename.php/",
        "/filename.php5",
        "/windows.PHP"
    };
    std::string directive("/*.php$");
    Rep::Directive parsed(directive, true);
    for (auto example : examples)
    {
        EXPECT_TRUE(parsed.match(example)) <<
            example << " didn't match " << directive;
    }

    for (auto example : antiexamples)
    {
        EXPECT_FALSE(parsed.match(example)) <<
            example << " matched " << directive;
    }
}

TEST(GoogleTest, FishStarExtension)
{
    std::vector<std::string> examples = {
        "/fish.php",
        "/fishheads/catfish.php?parameters"
    };
    std::vector<std::string> antiexamples = {
        "/Fish.PHP"
    };
    std::string directive("/fish*.php");
    Rep::Directive parsed(directive, true);
    for (auto example : examples)
    {
        EXPECT_TRUE(parsed.match(example)) <<
            example << " didn't match " << directive;
    }

    for (auto example : antiexamples)
    {
        EXPECT_FALSE(parsed.match(example)) <<
            example << " matched " << directive;
    }
}
