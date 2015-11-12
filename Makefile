CXX = clang++
CPPFLAGS = -I /usr/local/include
CXXFLAGS = -Wall -Wextra -std=c++14 -g
LDFLAGS = -L /usr/local/lib -levent

HEADERS = bytes.hpp poller.hpp relight-impl.hpp stream.hpp var.hpp

main: main.o $(HEADERS)
	$(CXX) $(LDFLAGS) -o main main.o
clean:
	rm -rf -- *.o main
