CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++14 -g
LDFLAGS = -L /usr/local/lib -levent

HEADERS = bytes.hpp poller.hpp stream.hpp var.hpp
OBJECTS = main.o poller.o stream.o utils-net.o

main: $(OBJECTS) $(HEADERS)
	$(CXX) $(LDFLAGS) -o main $(OBJECTS)
clean:
	rm -rf -- main $(OBJECTS)
