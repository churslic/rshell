#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <string>
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
        if (*it == '#') {
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
 * @summary: Uses execvp to call commands. It also uses a pipe to communicate
 * between the child and the parent to see whether or not execvp failed.
 * @parameter: char **argv is command that wants to be used.
 * @return: returns a boolean indicating whether execvp failed or not.
 * **/
bool execute (char **argv) {
    char str[] = "false";
    char readbuffer[6];

    //Creating a pipe. I want to communicate with parent whether or
    //not execvp failed
    int fd[2];
    if (pipe(fd) == -1) perror("There was an error with pipe");

    int pid = fork();
    if (pid == -1) {
        perror("fork fail");
        exit(1);
    }

    else if (pid == 0) {
        //Close the read end of the pipe
        if (close(fd[0]) == -1) perror("There was an error with close");

	    //If execvp fails we'll write into the pipe
        if(execvp(argv[0], argv) == -1) {
	        write(fd[1], str, (strlen(str)+1));
            perror("There was an error in execvp");
        }

        exit(1);
    }
    else {
        //Close the write end of the pipe
	    if (close(fd[1]) == -1) perror("There was an error with close");

	    //Want to wait for the child process to finish
        if (wait(0) == -1) perror("wait() error");

        //Now we can read in data from the child
        if (read(fd[0], readbuffer, sizeof(readbuffer)) == -1) {
            perror("There was an error with read");
        }

        //Check to see what readbuffer has stored in it
        if (strcmp(readbuffer, "false") == 0) return false;
        else return true;
    }
}

/**
 * connectors function
 * @summary: Takes care of conditionals and appropriately executes commands
 * @parameters: string con is used to tell which connector came before.
 * bool& result is used to keep track of whether the command executed.
 * char **argv is the command line
 * @return: returns nothing
 * **/
void connectors (string con, bool& result, char **argv) {
    if (con == ";") {
        result = execute(argv);
    }
    else if (con == "&&") {
        if (result) {
            result = execute(argv);
        }
        else {
            result = false;
        }
    }
    else if (con == "||") {
        if (!result) {
            result = execute(argv);
        }
        else {
            result = false;
        }
    }
    //This means that there were no connectors
    else if (con == "first") {
        execute(argv);
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
bool parse (char *cmd, char **argv) {
    char *token = strtok(cmd, " \t\r\n");
    argv[0] = token;

    if (strcmp(token, "exit") == 0) return false;

    int i = 1;
    string connector = "first";
    bool exec_result = true;

    while (token != NULL) {
        token = strtok(NULL, " \t\r\n");
        argv[i] = token;
        ++i;

        if (token != NULL) {
            if (strcmp(token, "#") == 0) {
                argv[i-1] = NULL;
                break;
            }
            else if (strcmp(token, "exit") == 0) {
                if (connector == "first") return false;
                else if (connector == ";") return false;
                else if (connector == "&&" && exec_result) return false;
                else if (connector == "||" && !exec_result) return false;
            }
            else if (strcmp(token, ";") == 0) {
                argv[i-1] = NULL;
                if (connector == "first") {
                    exec_result = execute(argv);
                    connector = ";";
                    i = 0;
                }
                else {
                    connectors(connector, exec_result, argv);
                    connector = ";";
                    i = 0;
                }
            }
            else if (strcmp(token, "&&") == 0) {
	            argv[i-1] = NULL;
		        if (connector == "first") {
		            exec_result = execute(argv);
			        connector = "&&";
			        i = 0;
		        }
		        else {
		            connectors(connector, exec_result, argv);
			        connector = "&&";
			        i = 0;
		        }
            }
	        else if (strcmp(token, "||") == 0) {
	            argv[i-1] = NULL;
		        if (connector == "first") {
		            exec_result = execute(argv);
			        connector = "||";
		            i = 0;
		        }
		        else {
		            connectors(connector, exec_result, argv);
			        connector = "||";
			        i = 0;
		        }
            }
        }
    }
    connectors(connector, exec_result, argv);
    return true;
}

int main(int argc, char **argv) {
    char *login = getlogin();
    char hostname[30];
    gethostname(hostname, 30);

    string name;
    for (int i = 0; login[i] != '\0'; ++i) {
        name.push_back(login[i]);
    }

    string host;
    for (int i = 0; hostname[i] != '\0'; ++i) {
        host.push_back(hostname[i]);
    }

    cout << name << "@" << host << ":~$ ";
    string command;
    getline(cin, command);

    while(command != "exit") {
        trans_string(command);
        char *cmd = new char[command.length() + 1];
        strcpy(cmd, command.c_str());

        bool ex = parse(cmd, argv);
        if(!ex) break;

        cout << name << "@" << host << ":~$ ";
        getline(cin, command);
    }

    return 0;
}
