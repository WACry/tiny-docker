all:
	g++ main.cc -o tocker

run:
	./tocker run /bin/bash

clean:
	rm tocker
