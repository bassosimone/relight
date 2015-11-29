CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++14 -g
LDFLAGS = -L /usr/local/lib -levent

HEADERS = bytes.hpp dns.hpp dns-resolver.hpp for-each.hpp net.hpp poller.hpp \
          stream.hpp var.hpp
OBJECTS = dns.o net.o poller.o
EXECUTABLES = main_dns main_net main_stream

all: $(EXECUTABLES)
main_dns: main_dns.o $(OBJECTS) $(HEADERS)
	$(CXX) -o main_dns main_dns.o $(OBJECTS) $(LDFLAGS)
main_net: main_net.o $(OBJECTS) $(HEADERS)
	$(CXX) -o main_net main_net.o $(OBJECTS) $(LDFLAGS)
main_stream: main_stream.o $(OBJECTS) $(HEADERS)
	$(CXX) -o main_stream main_stream.o $(OBJECTS) $(LDFLAGS)
clean:
	rm -rf -- $(EXECUTABLES) *.o
