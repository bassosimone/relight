CXX = g++
CXXFLAGS = $(COVERAGE) -Wall -Wextra -std=c++11 -g
LDFLAGS = $(COVERAGE) -L /usr/local/lib -levent

HEADERS = bytes.hpp dns.hpp dns-resolver.hpp for-each.hpp net.hpp poller.hpp \
          stream.hpp var.hpp
OBJECTS = dns.o net.o poller.o
EXECUTABLES = main_dns main_stream

.PHONY: all check clean

all: $(EXECUTABLES)
main_dns: main_dns.o $(OBJECTS) $(HEADERS)
	$(CXX) -o main_dns main_dns.o $(OBJECTS) $(LDFLAGS)
main_stream: main_stream.o $(OBJECTS) $(HEADERS)
	$(CXX) -o main_stream main_stream.o $(OBJECTS) $(LDFLAGS)
clean:
	rm -rf -- $(EXECUTABLES) *.o
check: $(EXECUTABLES)
	./main_dns
	./main_stream
