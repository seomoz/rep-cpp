CXX          ?= g++
CXXOPTS      ?= -g -Wall -Werror -std=c++11 -Iinclude/ -Ideps/url-cpp/include
DEBUG_OPTS   ?= -fprofile-arcs -ftest-coverage -O0 -fPIC
RELEASE_OPTS ?= -O3
BINARIES      = 

all: test release/librep.o $(BINARIES)

# Release libraries
release:
	mkdir -p release

release/bin: release
	mkdir -p release/bin

release/librep.o: release/directive.o release/agent.o deps/url-cpp/release/liburl.o
	ld -r -o $@ $^

release/%.o: src/%.cpp include/%.h release
	$(CXX) $(CXXOPTS) $(RELEASE_OPTS) -o $@ -c $<

# Debug libraries
debug:
	mkdir -p debug

debug/bin: debug
	mkdir -p debug/bin

debug/librep.o: debug/directive.o debug/agent.o deps/url-cpp/debug/liburl.o
	ld -r -o $@ $^

debug/%.o: src/%.cpp include/%.h debug
	$(CXX) $(CXXOPTS) $(DEBUG_OPTS) -o $@ -c $<

test/%.o: test/%.cpp
	$(CXX) $(CXXOPTS) $(DEBUG_OPTS) -o $@ -c $<

# Tests
test-all: test/test-all.o test/test-agent.o test/test-directive.o debug/librep.o
	$(CXX) $(CXXOPTS) $(DEBUG_OPTS) -o $@ $^ -lgtest -lpthread

# Bench
bench: bench.cpp release/librep.o
	$(CXX) $(CXXOPTS) $(RELEASE_OPTS) -o $@ $< release/librep.o

.PHONY: test
test: test-all
	./test-all
	./scripts/check-coverage.sh $(PWD)

clean:
	rm -rf debug release test-all bench
