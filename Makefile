CXX          ?= g++
CXXOPTS      ?= -Wall -Werror -std=c++11 -Iinclude/ -Ideps/url-cpp/include
DEBUG_OPTS   ?= -g -fprofile-arcs -ftest-coverage -O0 -fPIC
RELEASE_OPTS ?= -O3
BINARIES      = 

all: test release/librep.o $(BINARIES)

# Release libraries
release:
	mkdir -p release

release/bin: release
	mkdir -p release/bin

deps/url-cpp/release/liburl.o: deps/url-cpp/* deps/url-cpp/include/* deps/url-cpp/src/*
	make -C deps/url-cpp release/liburl.o

release/librep.o: release/directive.o release/agent.o release/robots.o deps/url-cpp/release/liburl.o
	ld -r -o $@ $^

release/%.o: src/%.cpp include/%.h release
	$(CXX) $(CXXOPTS) $(RELEASE_OPTS) -o $@ -c $<

# Debug libraries
debug:
	mkdir -p debug

debug/bin: debug
	mkdir -p debug/bin

deps/url-cpp/debug/liburl.o: deps/url-cpp/* deps/url-cpp/include/* deps/url-cpp/src/*
	make -C deps/url-cpp debug/liburl.o

debug/librep.o: debug/directive.o debug/agent.o debug/robots.o deps/url-cpp/debug/liburl.o
	ld -r -o $@ $^

debug/%.o: src/%.cpp include/%.h debug
	$(CXX) $(CXXOPTS) $(DEBUG_OPTS) -o $@ -c $<

test/%.o: test/%.cpp
	$(CXX) $(CXXOPTS) $(DEBUG_OPTS) -o $@ -c $<

# Tests
test-all: test/test-all.o test/test-agent.o test/test-directive.o test/test-robots.o debug/librep.o
	$(CXX) $(CXXOPTS) $(DEBUG_OPTS) -o $@ $^ -lgtest -lpthread

# Bench
bench: bench.cpp release/librep.o
	$(CXX) $(CXXOPTS) $(RELEASE_OPTS) -o $@ $< release/librep.o

.PHONY: test
test: test-all
	./test-all
	./scripts/check-coverage.sh $(PWD)

clean:
	rm -rf debug release test-all bench test/*.o deps/url-cpp/{debug,release}
