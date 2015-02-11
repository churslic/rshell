CFLAGS = -ansi -pedantic -Wall -Werror

all: rshell ls cp
	if [ ! -d bin ]; then mkdir bin; fi

rshell:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/rshell.cpp -o bin/rshell

ls:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/ls.cpp -o bin/ls

cp:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/cp.cpp -o bin/cp

clean:
	@rm -r ./bin
