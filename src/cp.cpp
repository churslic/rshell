#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "Timer.h"

using namespace std;

void option_1(const char *source, const char *dest) {
    Timer t;
    double wallClock;
    double usrTime;
    double sysTime;
    t.start();

    struct stat s;
    if(stat(source, &s) == -1) {
        perror("");
        return;
    }

    struct stat x;
    if(stat(dest, &x) != -1) perror("File already exists");

    ifstream src;
    src.open(source);

    ofstream dst;
    dst.open(dest);

    char c;
    while(src.get(c)) {
        dst << c;
    }
    dst.close();
    src.close();

    t.elapsedWallclockTime(wallClock);
    t.elapsedUserTime(usrTime);
    t.elapsedSystemTime(sysTime);
    cout << "Wallclock Time: " << wallClock << endl;
    cout << "User Time: " << usrTime << endl;
    cout << "System Time: " << sysTime << endl;
}

void RWonechar(const char *mysrc, const char* mydst) {
    Timer t;
    double wallClock;
    double usrTime;
    double sysTime;
    t.start();

	struct stat s;
	struct stat d;
	if(stat(mysrc, &s) == -1)
	{
		perror("stat() error, source");
		exit(1);
	}
	if(stat(mydst, &d) != -1)
	{
		perror("stat() error, dst");
		exit(1);
	}
	//-----------------------------------
	char buffer[1];
	int rd;
	int closesrc;
	int closedst;

	int opensrc = open(mysrc, O_RDONLY);
	if(opensrc == -1)
	{
		perror("open() error src");
		exit(1);
	}

    int opendst = open(mydst, O_RDWR | O_CREAT, 0644);
	if(opendst == -1)
	{
		perror("open() error dst");
		exit(1);
	}

	while((rd = read(opensrc,buffer, 1)))
	{
		if(rd == -1)
		{
			perror("read() error");
			exit(1);
		}

		rd = write(opendst,buffer, 1);

		if (rd == -1)
		{
			perror("write() error");
			exit(1);
		}
	}

	closesrc = close(opensrc);
	if(closesrc == -1)
	{
		perror("close() error");
		exit(1);
	}

	closedst = close(opendst);
	if(closedst == -1)
	{
		perror("close() error");
		exit(1);
	}


    t.elapsedWallclockTime(wallClock);
    t.elapsedUserTime(usrTime);
    t.elapsedSystemTime(sysTime);
    cout << "Wallclock Time: " << wallClock << endl;
    cout << "User Time: " << usrTime << endl;
    cout << "System Time: " << sysTime << endl;
}

void RWbuff(const char *mysrc, const char* mydst) {
    Timer t;
    double wallClock;
    double usrTime;
    double sysTime;
    t.start();

	struct stat s;
	struct stat d;
	if(stat(mysrc, &s) == -1)
	{
		perror("stat() error, source");
		exit(1);
	}
	if(stat(mydst, &d) != -1)
	{
		perror("stat() error, dst");
		exit(1);
	}
	//-----------------------------------
	char buffer[BUFSIZ];
    int rd;
    int closesrc;
	int closedst;

	int opensrc = open(mysrc, O_RDONLY);
	if(opensrc == -1)
	{
		perror("open() error src");
		exit(1);
	}

    int opendst = open(mydst, O_RDWR | O_CREAT, 0644);
	if(opendst == -1)
	{
		perror("open() error dst");
		exit(1);
	}

	while((rd = read(opensrc,buffer, BUFSIZ)))
	{
		if(rd == -1)
		{
			perror("read() error");
			exit(1);
		}

		rd = write(opendst,buffer, BUFSIZ);
        if (rd == -1)
		{
			perror("write() error");
			exit(1);
		}
	}

	closesrc = close(opensrc);
	if(closesrc == -1)
	{
		perror("close() error");
		exit(1);
	}

	closedst = close(opendst);
	if(closedst == -1)
	{
		perror("close() error");
		exit(1);
	}


    t.elapsedWallclockTime(wallClock);
    t.elapsedUserTime(usrTime);
    t.elapsedSystemTime(sysTime);
    cout << "Wallclock Time: " << wallClock << endl;
    cout << "User Time: " << usrTime << endl;
    cout << "System Time: " << sysTime << endl;
}

int main(int argc, char **argv) {
    if(argc < 3) {
        cout << "Usage: cp <source> <dest> (optional)" << endl;
        return 1;
    }
    else if(argc == 3) {
        RWbuff(argv[1], argv[2]);
    }
    else if(argc == 4) {

        const char* temp = strcat(argv[2], "_1");
        option_1(argv[1], temp);

        const char* temp1 = strcat(argv[2], "_2");
        RWonechar(argv[1], temp1);

        const char* temp2 = strcat(argv[2], "_3");
        RWbuff(argv[1], temp2);
    }
    else {
        cout << "Usage: cp <source> <dest> (optional)" << endl;
        return 1;
    }

    return 0;
}
