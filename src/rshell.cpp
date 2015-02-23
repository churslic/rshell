#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <queue>
using namespace std;

/**
 * trans_string function
 * @summary: transforms the string so that there are spaces in
 * between the connectors
 * @param: the entire command line
 * @return: returns nothing
 * **/
void trans_string (string& command);

/**
 * execute function
 * @summary: Uses execvp to call commands. It also uses a pipe to
 * communicate between the child and the parent to see whether or
 * not execvp failed.
 * @parameter: char **argv is command that wants to be used.
 * @return: returns a boolean indicating whether execvp failed or
 * not.
 * **/
bool execute (char **argv);

/**
 * connectors function
 * @summary: Takes care of conditionals and appropriately executes
 * commands
 * @parameters: string con is used to tell which connector came
 * before, bool& result is used to keep track of whether the
 * command executed, char **argv is the command line
 * @return: returns nothing
 * **/
void connectors (string con, bool& result, char **argv);

void out_redir(char **argv, const char *arg2, const char *str);

void in_redir(char **argv, const char *arg2);

void forking(int* pip1, int* pip2, queue<char**> &v);

void piping(char **argv, queue<char**> &v);

bool is_con(const char* s);

/**
 * parse function
 * @summary: Will take the command and store each string of
 * characters into an array. strtok ignores all whitespace.
 * Function will also break once # is found, since anything after
 * that can be ignored. The function will also check for connectors
 * and execute commands.
 * @param: char *cmd is the entire line that was input. char **argv
 * will be the array that stores each individual string of
 * characters.
 * @return: Does not return anything
 * **/
void parse (char *cmd, char **argv);


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

        parse(cmd, argv);

        cout << name << "@" << host << ":~$ ";
        getline(cin, command);
    }

    return 0;
}

void trans_string (string& command) {
    string::iterator it;

    for (it = command.begin(); it != command.end(); ++it) {
        if (*it == ';') {
	        it = command.insert(it, ' ');
            it += 2;
            it = command.insert(it, ' ');
        }
    }

    for (it = command.begin(); it != command.end(); ++it) {
        if (*it == '#') {
            it = command.insert(it, ' ');
            it += 2;
            it = command.insert(it, ' ');
        }
    }

    for (it = command.begin(); it != command.end(); ++it) {
        if (*it == '&' && *(it + 1) == '&') {
            it = command.insert(it, ' ');
            it += 3;
            it = command.insert(it, ' ');
        }
    }

    for (it = command.begin(); it != command.end(); ++it) {
        if (*it == '|' && *(it + 1) == '|') {
            cout << "*it: " << *it << endl;
            cout << "*(it + 1): " << *(it+1) << endl;
            it = command.insert(it, ' ');
            it += 3;
            it = command.insert(it, ' ');
        }
    }

    for(it = command.begin(); it != command.end(); ++it) {
        if(*it == '|' && *(it+1) != '|' && *(it+1) != ' ') {
            it = command.insert(it, ' ');
            it += 2;
            it = command.insert(it, ' ');
        }
    }

    for(it = command.begin(); it != command.end(); ++it) {
        if(*it == '>' && *(it + 1) == '>') {
            it = command.insert(it, ' ');
            it += 3;
            it = command.insert(it, ' ');
        }
    }

    for(it = command.begin(); it != command.end(); ++it) {
        if(*it == '>' && *(it+1) != '>' && *(it+1) != ' ') {
            it = command.insert(it, ' ');
            it += 2;
            it = command.insert(it, ' ');
        }
    }

    for(it = command.begin(); it != command.end(); ++it) {
        if(*it == '<') {
            it = command.insert(it, ' ');
            it += 2;
            it = command.insert(it, ' ');
        }
    }
}


bool execute (char **argv) {
    char str[] = "false";
    char readbuffer[6];

    //Creating a pipe. I want to communicate with parent whether or
    //not execvp failed
    int fd[2];
    if (pipe(fd) == -1) perror("");

    int pid = fork();
    if (pid == -1) {
        perror("");
        exit(1);
    }

    else if (pid == 0) {
        //Close the read end of the pipe
        if (close(fd[0]) == -1) perror("");

	    //If execvp fails we'll write into the pipe
        if(execvp(argv[0], argv) == -1) {
	        write(fd[1], str, (strlen(str)+1));
            perror("");
        }

        exit(1);
    }
    else {
        //Close the write end of the pipe
	    if (close(fd[1]) == -1) perror("");

	    //Want to wait for the child process to finish
        if (wait(0) == -1) perror("");

        //Now we can read in data from the child
        if (read(fd[0], readbuffer, sizeof(readbuffer)) == -1) {
            perror("");
        }

        //Check to see what readbuffer has stored in it
        if (strcmp(readbuffer, "false") == 0) return false;
        else return true;
    }
}


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


void out_redir(char **argv, const char* arg2, const char* str) {
    int pid;
    int fd;

    if((pid = fork()) == -1) {
        perror("");
        exit(1);
    }
    else if(pid == 0) { //Child process
        //Open or create the second file
        if(strcmp(str, ">") == 0) {
            fd = open(arg2, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
            if(fd == -1) {
                perror("");
                exit(1);
            }
        }
        else {
            fd = open(arg2, O_RDWR|O_CREAT|O_APPEND,
                      S_IRUSR|S_IWUSR);
            if(fd == -1) {
                perror("");
                exit(1);
            }
        }

        //Close stdout, and make fd the new stdout
        if(dup2(fd, 1) == -1) {
            perror("");
            exit(1);
        }

        if(execvp(argv[0], argv) == -1) perror("");
        exit(1);
    }
    else { //Parent process
        if(wait(0) == -1) perror("");
    }
}

void in_redir(char **argv, const char *arg2) {
    int pid;
    int fd;

    if((pid = fork()) == -1) {
        perror("");
        exit(1);
    }
    else if(pid == 0) { //Child process
        fd = open(arg2, O_RDONLY);
        if(fd == -1) {
            perror("");
            exit(1);
        }

        //Close stdin, and make fd the new stdin
        if(dup2(fd, 0) == -1) {
            perror("");
            exit(1);
        }

        if(execvp(argv[0], argv) == -1) perror("");
        exit(1);
    }
    else { //Parent process
        if(wait(0) == -1) perror("");
    }
}

void forking(int* pip1, int* pip2, queue<char**> &v) {
    int pid;
    if((pid = fork()) == -1) {
        perror("fork err");
        exit(1);
    }
    else if(pid == 0) { //Child process
        if(pip1 != NULL) {
            //That means we have something to read
            if(dup2(pip1[0], 0) == -1)
                perror("dup err");
            if(close(pip1[0]) == -1)
                perror("close err");
            if(close(pip1[1]) == -1)
                perror("close err");
        }

        if(pip2 != NULL) {
            //that means we have something to write
            if(dup2(pip2[1], 1) == -1)
                perror("dup err");
            if(close(pip2[0]) == -1)
                perror("close err");
            if(close(pip2[1]) == -1)
                perror("close err");
        }

        if(execvp(v.front()[0], v.front()) == -1)
            perror("");
        exit(1);
    }
    else { //Parent process
        if(wait(0) == -1)
            perror("");
    }

    int savestdin;
    if((savestdin = dup(0)) == -1)
        perror("");
    if(dup2(savestdin, 0) == -1)
        perror("");

}

void piping(char **argv, queue<char**> &v) {
    int pip1[2];
    int pip2[2];

    if(pipe(pip2) == -1)
        perror("");

    //There is no input pipe
    forking(NULL, pip2, v);
    v.pop();

    pip1[0] = pip2[0]; //So lpipe[0] has the read end
    pip1[1] = pip2[1]; //So lpipe[1] has the write end

    for(unsigned int i = 1; i < v.size() - 1; ++i) {
        cout << "do stuff" << endl;
    }

    forking(pip1, NULL, v);

    if(close(pip1[0]) == -1)
        perror("");
    if(close(pip1[1]) == -1)
        perror("");
    if(close(pip2[0]) == -1)
        perror("");
    if(close(pip2[1]) == -1)
        perror("");
}

bool is_con(const char* s) {
    if(strcmp(s, ";") == 0) return true;
    else if(strcmp(s, "&&") == 0) return true;
    else if(strcmp(s, "||") == 0) return true;
    else if(strcmp(s, "<") == 0) return true;
    else if(strcmp(s, ">") == 0) return true;
    else if(strcmp(s, ">>") == 0) return true;
    else if(strcmp(s, "#") == 0) return true;

    return false;
}

void parse (char *cmd, char **argv) {
    char *token = strtok(cmd, " \t\r\n");
    argv[0] = token;

    if (strcmp(token, "exit") == 0) exit(0);

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
                if (connector == "first") exit(0);
                else if (connector == ";") exit(0);
                else if (connector == "&&" && exec_result) exit(0);
                else if (connector == "||" && !exec_result) exit(0);
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
            else if(strcmp(token, ">") == 0) {
                argv[i-1] = NULL;

                token = strtok(NULL, " \t\r\n");
                const char* arg2 = token;
                out_redir(argv, arg2, ">");
                i = 0;
            }
            else if(strcmp(token, ">>") == 0) {
                argv[i-1] = NULL;

                token = strtok(NULL, " \t\r\n");
                const char* arg2 = token;
                out_redir(argv, arg2, ">>");
                i = 0;
            }
            else if(strcmp(token, "<") == 0) {
                argv[i-1] = NULL;

                token = strtok(NULL, " \t\r\n");
                const char* arg2 = token;
                in_redir(argv, arg2);
                i = 0;
            }
            else if(strcmp(token, "|") == 0) {
                argv[i-1] = NULL;

                queue<char**> v;
                v.push(argv);

                char **arg2 = new char*;
                int j = 0;

                token = strtok(NULL, " \t\r\n");

                while(token != NULL && !is_con(token)) {
                    arg2[j] = token;
                    ++j;
                    if(strcmp(token, "|") == 0) {
                        arg2[j] = NULL;
                        v.push(arg2);
                        j = 0;
                    }
                    token = strtok(NULL, " \t\r\n");
                }
                arg2[j] = NULL;
                v.push(arg2);

                piping(argv, v);
                delete[] arg2;
                i = 0;
            }
        }
    }
    if(connector == "first" || token != NULL)
        connectors(connector, exec_result, argv);
}
