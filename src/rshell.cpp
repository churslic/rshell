#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

int main(int argc, char **argv) {
    cout << "$ ";

    char cmd[100];
    cin >> cmd;

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

    return 0;
}
