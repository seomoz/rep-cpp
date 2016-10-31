Robots Exclusion Protocol Parser for C++
========================================
Supports the [1996 RFC](http://www.robotstxt.org/norobots-rfc.txt), as well as some
modern conventions, including:

- wildcard matching (`*` and `$`)
- sitemap listing
- crawl-delay

__This library deals in UTF-8-encoded strings.__

Matching
--------
A path may match multiple directives. For example, `/some/path/page.html` matches all
of these rules:

```
Allow: /some/
Disallow: /some/path/
Allow: /*/page.html
```

Each directive is given a priority, and the highest-priority matching directive is used.
We choose the length of the expression to be that priority. In the above example, the
priorities are:

```
Allow: /some/            (priority = 6)
Disallow: /some/path/    (priority = 11)
Allow: /*/page.html      (priority = 12)
```

Classes
-------
A `Robots` object is the result of parsing a single `robots.txt` file. It has a mapping of
agent names to `Agent` objects, as well as a vector of the `sitemaps` listed in the file.
An `Agent` object holds the crawl-delay and `Directive`s associated with a particular
user-agent.

Parsing and Querying
--------------------
Here's an example of parsing a robots.txt file:

```c++
#include "robots.h"

std::string content = "...";
Rep::Robots robots = Rep::Robots::parse(content);

// Is this path allowed to the provided agent?
robots.allowed("/some/path", "my-agent");

// Is this URL allowed to the provided agent?
robots.url_allowed("http://example.com/some/path", "my-agent");
```

If a client is interested only in the exclusion rules of a single agent, then:

```c++
Rep::Agent agent = Rep::Robots::parse(content).agent("my-agent");

// Is this path allowed to this agent?
agent.allowed("/some/path");

// Is this URL allowed to this agent?
agent.url_allowed("http://example.com/some/path");
```

Building
========
This library depends on `url-cpp`, which is included as a submodule. We provide two
main targets, `{debug,release}/librep.o`:

```
git submodule update --init --recursive
make release/librep.o
```

Development
===========

Environment
-----------
To launch the `vagrant` image, we only need to
`vagrant up` (though you may have to provide a `--provider` flag):

```bash
vagrant up
```

With a running `vagrant` instance, you can log in and run tests:

```bash
vagrant ssh
cd /vagrant

make test
```

Running Tests
-------------
Tests are run with the top-level `Makefile`:

```bash
make test
```

PRs
===
These are not all hard-and-fast rules, but in general PRs have the following expectations:

- __pass Travis__ -- or more generally, whatever CI is used for the particular project
- __be a complete unit__ -- whether a bug fix or feature, it should appear as a complete
    unit before consideration.
- __maintain code coverage__ -- some projects may include code coverage requirements as
    part of the build as well
- __maintain the established style__ -- this means the existing style of established
    projects, the established conventions of the team for a given language on new
    projects, and the guidelines of the community of the relevant languages and
    frameworks.
- __include failing tests__ -- in the case of bugs, failing tests demonstrating the bug
    should be included as one commit, followed by a commit making the test succeed. This
    allows us to jump to a world with a bug included, and prove that our test in fact
    exercises the bug.
- __be reviewed by one or more developers__ -- not all feedback has to be accepted, but
    it should all be considered.
- __avoid 'addressed PR feedback' commits__ -- in general, PR feedback should be rebased
    back into the appropriate commits that introduced the change. In cases, where this
    is burdensome, PR feedback commits may be used but should still describe the changed
    contained therein.

PR reviews consider the design, organization, and functionality of the submitted code.

Commits
=======
Certain types of changes should be made in their own commits to improve readability. When
too many different types of changes happen simultaneous to a single commit, the purpose of
each change is muddled. By giving each commit a single logical purpose, it is implicitly
clear why changes in that commit took place.

- __updating / upgrading dependencies__ -- this is especially true for invocations like
    `bundle update` or `berks update`.
- __introducing a new dependency__ -- often preceeded by a commit updating existing
    dependencies, this should only include the changes for the new dependency.
- __refactoring__ -- these commits should preserve all the existing functionality and
    merely update how it's done.
- __utility components to be used by a new feature__ -- if introducing an auxiliary class
    in support of a subsequent commit, add this new class (and its tests) in its own
    commit.
- __config changes__ -- when adjusting configuration in isolation
- __formatting / whitespace commits__ -- when adjusting code only for stylistic purposes.

New Features
------------
Small new features (where small refers to the size and complexity of the change, not the
impact) are often introduced in a single commit. Larger features or components might be
built up piecewise, with each commit containing a single part of it (and its corresponding
tests).

Bug Fixes
---------
In general, bug fixes should come in two-commit pairs: a commit adding a failing test
demonstrating the bug, and a commit making that failing test pass.

Tagging and Versioning
======================
Whenever the version included in `setup.py` is changed (and it should be changed when
appropriate using [http://semver.org/](http://semver.org/)), a corresponding tag should
be created with the same version number (formatted `v<version>`).

```bash
git tag -a v0.1.0 -m 'Version 0.1.0

This release contains an initial working version Rep::Robots.'

git push origin
```
