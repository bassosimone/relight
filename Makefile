CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++14 -g
LDFLAGS = -L /usr/local/lib -levent

HEADERS = bytes.hpp poller.hpp stream.hpp var.hpp

main: main.o relight-impl.o $(HEADERS)
	$(CXX) $(LDFLAGS) -o main main.o relight-impl.o
clean:
	rm -rf -- *.o main
