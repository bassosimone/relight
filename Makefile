CXX = clang++
CPPFLAGS = -I /usr/local/include -D RELIGHT_NOTHROW
CXXFLAGS = -Wall -Wextra -std=c++14 -g
LDFLAGS = -L /usr/local/lib -levent

main: main.o
	$(CXX) $(LDFLAGS) -o main main.o

clean:
	rm -rf -- *.o main
