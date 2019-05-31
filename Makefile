cppm: cppm.cpp

test: cppm
	./test.sh

clean:
	rm -f cppm *.o *~ tmp*
