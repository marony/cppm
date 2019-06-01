CFLAGS=-Wall -std=c++11
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.c=.o)
EXECUTABLE=cppm

cppm: $(OBJS)
	$(CXX) -o $(EXECUTABLE) $(OBJS) $(LDFLAGS)

$(OBJS): $(wildcard *.h)

test: $(EXECUTABLE)
	./$(EXECUTABLE) -test
	./test.sh

clean:
	rm -f $(EXECUTABLE) *.o *~ tmp*
