test: test.cpp LxSerial.hpp
	g++ test.cpp -o test -w -I.

clean:
	rm -rf *.o;rm test