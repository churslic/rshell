#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
using namespace std;

void parse (char *cmd, char **argv) {
    char *token = strtok(cmd, " ");
    argv[0] = token;
    //cout << "argv[0] = " << argv[0] << endl;

    int i = 1;
    while (token != NULL) {
        token = strtok(NULL, " ");
        argv[i] = token;
        //cout << "argv[" << i << "] = " << argv[i] << endl;
        ++i;
    }
}

int main(int argc, char **argv) {
    cout << "$ ";

    char cmd[100];
    cin.getline(cmd, 100);
//    cout << cmd << endl;

// Parse cmd into argv
    parse(cmd, argv);

/*
    int pid = fork();
    if (pid == -1) {
        perror("fork fail");
    }
    // This is the child process
    else if (pid == 0) {
        if(execvp(cmd, argv)) {
            perror("there was an error in execvp");
        }
        exit(1);
    }
    // This is the parent process
    else {
        if(wait(0) == -1) {
            perror("wait() error");
        }
    }
*/
    return 0;
}
