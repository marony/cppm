cppm: cppm.cpp

test: cppm
	./cppm -test
	./test.sh

clean:
	rm -f cppm *.o *~ tmp*
