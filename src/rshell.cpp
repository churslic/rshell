#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <string>
#include <vector>
using namespace std;

/**
 * trans_string function
 * @summary: transforms the string so that there are spaces in between
 * the connectors
 * @param: the entire command line
 * @return: returns nothing
 * **/
void trans_string (string& command) {

    for (string::iterator it = command.begin(); it != command.end(); ++it) {
        if (*it == ';') {
            it = command.insert(it, ' ');
            ++it;
            ++it;
            it = command.insert(it, ' ');
        }
    }

    for (string::iterator it = command.begin(); it != command.end(); ++it) {
        if (*it == '&' && *(it + 1) == '&') {
            it = command.insert(it, ' ');
            ++it;
            ++it;
            ++it;
            it = command.insert(it, ' ');
        }
    }

    for (string::iterator it = command.begin(); it != command.end(); ++it) {
        if (*it == '|' && *(it + 1) == '|') {
            it = command.insert(it, ' ');
            ++it;
            ++it;
            ++it;
            it = command.insert(it, ' ');
        }
    }
}


/**
 * execute function
 * @summary: Uses execvp to call commands.
 * @parameter: char **argv is command that wants to be used.
 * @return: returns nothing
 * **/
bool execute (char **argv) {
    bool exec_result = true;

    int pid = fork();
    if (pid == -1) {
        perror("fork fail");
        exit(1);
    }
    else if (pid == 0) {
        int fail = 0;
        fail = execvp(argv[0], argv);

        if(fail == -1) {
            perror("there was an error in execvp");
        }

        exec_result = false;
        exit(1);
    }
    else {
        if(wait(0) == -1) {
            perror("wait() error");
        }
        return exec_result;
    }
}


/**
 * parse function
 * @summary: Will take the command and store each string of characters into
 * an array. strtok ignores all whitespace. Function will also break
 * once # is found, since anything after that can be ignored. The function
 * will also check for connectors and execute commands.
 * @param: char *cmd is the entire line that was input. char **argv
 * will be the array that stores each individual string of characters.
 * @return: Does not return anything
 * **/
void parse (char *cmd, char **argv) {
    char *token = strtok(cmd, " \t\r\n");
    argv[0] = token;

    int i = 1;

    while (token != NULL) {
        token = strtok(NULL, " \t\r\n");
        argv[i] = token;
        ++i;
        ++size;

        if (token != NULL) {
            if (strcmp(token, "#") == 0) {
                argv[i-1] = NULL;
                break;
            }
        }
    }
}


int main(int argc, char **argv) {
    cout << "$ ";
    string command;
    getline(cin, command);

    while(command != "exit") {
        trans_string(command);
        char *cmd = new char[command.length() + 1];
        strcpy(cmd, command.c_str());

        parse(cmd, argv);
        execute(argv);

        cout << "$ ";
        getline(cin, command);
    }

    return 0;
}
