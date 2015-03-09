#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <boost/tokenizer.hpp>
#include <signal.h>
#include <sys/param.h>

using namespace boost;
using namespace std;

void handler(int i);
void parse(string command);
void remove_comments(string &command);
void trans_string(string &command);

typedef tokenizer< char_separator<char> > tok;

int main() {
    if(signal(SIGINT, SIG_IGN) == SIG_ERR) perror("");

    char buf[MAXPATHLEN];
    if(getcwd(buf, MAXPATHLEN) == NULL) perror("");
    string pwd = buf;

    char *login = getlogin();
    char hostname[30];
    gethostname(hostname, 30);

    string name;
    for(int i = 0; login[i] != '\0'; ++i)
        name.push_back(login[i]);

    string host;
    for(int i = 0; hostname[i] != '\0'; ++i)
        host.push_back(hostname[i]);

    cout << name << "@" << host << ":" << pwd << "$ ";
    string command;
    getline(cin, command);

    while(command != "exit") {
        if(!command.empty()) {
            remove_comments(command);
            if(!command.empty()) {
                trans_string(command);
                parse(command);
            }
        }

        if(getcwd(buf, MAXPATHLEN) == NULL) perror("");
        pwd = buf;
        cout << name << "@" << host << ":" << pwd << "$ ";
        getline(cin, command);
    }

    return 0;
}

void handler(int i) {
    if(i == SIGINT) {
        if(kill(0, SIGINT) == -1)
            perror("");
    }
}

bool is_con(string s) {
    if(s == "&&") return true;
    else if(s == "||") return true;
    else if(s == ";") return true;
    else if(s == "#") return true;
    else if(s == "|") return true;
    else if(s == ">") return true;
    else if(s == ">>") return true;
    else if(s == "<") return true;

    return false;
}

bool is_special(string s) {
    if(s == ">") return true;
    else if(s == ">>") return true;
    else if(s == "<") return true;
    else if(s == "|") return true;

    return false;
}

vector<string> parse_path(string pathName) {
    char_separator<char> delim(":");
    tok mytok(pathName, delim);
    tok::iterator it = mytok.begin();

    vector<string> v;
    for(; it != mytok.end(); ++it) {
        if((*it).find("csshare") == string::npos &&
           (*it).find("ccali003") == string::npos)
        {
            v.push_back(*it);
        }
    }

    return v;
}

vector<string> create_path() {
    char *pathStr = getenv("PATH");
    if(pathStr == NULL) perror("");
    string pathName = pathStr;
    return parse_path(pathName);
}

string find_path(string str) {
    vector<string> exec_paths = create_path();

    for(unsigned int i = 0; i < exec_paths.size(); ++i) {
        DIR *dirp = opendir(exec_paths.at(i).c_str());
        if(dirp == NULL) {
            perror("opendir err");
            exit(1);
        }

        dirent *direntp;

        while((direntp = readdir(dirp))) {
            if(direntp == NULL && errno != 0) {
                perror("readdir err");
                exit(1);
            }
            if(strcmp(direntp->d_name, str.c_str()) == 0) {
                if(closedir(dirp) == -1) {
                    perror("closedir err");
                    exit(1);
                }

                return exec_paths.at(i);
            }
        }
        if(closedir(dirp) == -1) {
            perror("closedir err");
            exit(1);
        }
    }
    return "null";
}

vector<string> remove_spaces(string command) {
    vector<string> v;

    char_separator<char> delim(" ");
    tok mytok(command, delim);
    tok::iterator it = mytok.begin();

    for(; it != mytok.end(); ++it) {
        v.push_back(*it);
    }

    return v;
}

vector<char*> get_arguments(vector<string> &cmds) {
    vector<char*> v;
    for(unsigned int i = 0; i < cmds.size(); ++i) {
        v.push_back(&(cmds.at(i)[0]));
    }
    v.push_back(NULL);
    return v;
}

bool forking(vector<char*> &argv) {
    int pid = fork();
    if(pid == -1) {
        perror("");
        exit(1);
    }
    else if(pid == 0) {
        if(signal(SIGINT, SIG_DFL) == SIG_ERR) perror("");

        if(execv(&argv[0][0], &argv[0]) == -1)
            perror("");
        exit(1);
    }
    else {
        int status;
        if(wait(&status) == -1) {
            perror("");
            exit(1);
        }

        if(status != 0) return false;
        else return true;
    }
}

bool change_dir(vector<string> &cmds) {
    if(cmds.size() == 1) {
        cerr << "bash: syntax err" << endl;
        return false;
    }

    char buf[MAXPATHLEN];
    if(getcwd(buf, MAXPATHLEN) == NULL) perror("");
    string pwd = buf;

    char_separator<char> delim("/");
    tok mytok(cmds.at(1), delim);
    tok::iterator it = mytok.begin();

    vector<string> v;

    for(; it != mytok.end(); ++it) {
        v.push_back(*it);
    }

    for(unsigned int i = 0; i < v.size(); ++i) {
        if(v.at(i) == "..") {
            unsigned int pos = pwd.find_last_of("/");
            pwd.erase(pos, pwd.size()-pos);
        }
        else {
            pwd = pwd + "/" + v.at(i);
        }
    }

    if(chdir(pwd.c_str()) == -1) {
        perror("");
        return false;
    }

    return true;
}

bool execute(string args) {
    vector<string> cmds = remove_spaces(args);

    if(cmds.at(0) == "exit") exit(0);
    if(cmds.at(0) == "cd") return change_dir(cmds);

    string correct_path = find_path(cmds.at(0));

    if(correct_path != "null") {
        vector<char*> argv = get_arguments(cmds);
        correct_path = correct_path + "/" + cmds.at(0);

        argv.at(0) = (char*) correct_path.c_str();

        return forking(argv);
    }
    else {
        cerr << cmds.at(0) << ": command not found" << endl;
        return false;
    }
}

bool out_redir(vector<char*> &argv, const char* file,
               const char* str)
{
    int pid = fork();
    int fd;

    if(pid == -1) {
        perror("");
        exit(1);
    }
    else if(pid == 0) {
        if(signal(SIGINT, SIG_DFL) == SIG_ERR) perror("");
        if(strcmp(str, ">") == 0) {
            fd = open(file, O_RDWR|O_CREAT|O_TRUNC,
                      S_IRUSR|S_IWUSR);
            if(fd == -1) {
                perror("");
                exit(1);
            }
        }
        else {
            fd = open(file, O_RDWR|O_CREAT|O_APPEND,
                      S_IRUSR|S_IWUSR);
            if(fd == -1) {
                perror("");
                exit(1);
            }
        }

        if(dup2(fd, 1) == -1) {
            perror("");
            exit(1);
        }

        if(execv(&argv[0][0], &argv[0]) == -1) perror("");
        exit(1);
    }
    else {
        int status;
        if(wait(&status) == -1) {
            perror("");
            exit(1);
        }

        if(status != 0) return false;
        else return true;
    }
}

bool execute_out(vector<string> &args, unsigned int i,
                 const char* str)
{
    vector<string> cmds1 = remove_spaces(args.at(i));

    if(cmds1.at(0) == "exit") exit(0);
    if(cmds1.at(0) == "cd") return change_dir(cmds1);

    vector<string> cmds2 = remove_spaces(args.at(i+1));

    if(cmds2.size() > 1) {
        cerr << "Too many arguments" << endl;
        return false;
    }

    string correct_path = find_path(cmds1.at(0));

    if(correct_path != "null") {
        vector<char*> argv1 = get_arguments(cmds1);
        const char* file = cmds2.at(0).c_str();

        correct_path = correct_path + "/" + cmds1.at(0);
        argv1.at(0) = (char*) correct_path.c_str();

        return out_redir(argv1, file, str);
    }
    else {
        cout << cmds1.at(0) << ": command not found" << endl;
        return false;
    }
}

bool in_redir(vector<char*> &argv, const char *file) {
    int pid = fork();
    int fd;

    if(pid == -1) {
        perror("");
        exit(1);
    }
    else if(pid == 0) {
        if(signal(SIGINT, SIG_DFL) == SIG_ERR) perror("");
        //Create file descriptor for the file
        fd = open(file, O_RDONLY);
        if(fd == -1) {
            perror("");
            exit(1);
        }

        //Close stdin, make fd the new stdin
        if(dup2(fd, 0) == -1) {
            perror("");
            exit(1);
        }

        if(execv(&argv[0][0], &argv[0]) == -1) perror("");
        exit(1);
    }
    else {
        int status;
        if(wait(&status) == -1) {
            perror("");
            exit(1);
        }

        if(status != 0) return false;
        else return true;
    }
}

bool execute_in(vector<string> &args, unsigned int i) {
    vector<string> cmds1 = remove_spaces(args.at(i));

    if(cmds1.at(0) == "exit") exit(0);
    if(cmds1.at(0) == "cd") return change_dir(cmds1);

    vector<string> cmds2 = remove_spaces(args.at(i+1));

    if(cmds2.size() > 1) {
        cerr << "Too many arguments" << endl;
        return false;
    }

    struct stat s;
    if(stat(cmds2.at(0).c_str(), &s) == -1) {
        perror("");
        exit(1);
    }

    if(s.st_mode&S_IFREG) {
        string correct_path = find_path(cmds1.at(0));

        if(correct_path != "null") {
            vector<char*> argv1 = get_arguments(cmds1);
            const char* file = cmds2.at(0).c_str();

            correct_path = correct_path + "/" + cmds1.at(0);
            argv1.at(0) = (char*) correct_path.c_str();

            return in_redir(argv1, file);
        }
        else {
            cerr << cmds1.at(0) << ": command not found" << endl;
            return false;
        }
    }
    else {
        cerr << cmds2.at(0) << ": Is not a file" << endl;
        return false;
    }

}

bool valid_pipes(vector<string> &args) {
    for(unsigned int i = 0; i < args.size(); ++i) {
        vector<string> cmds = remove_spaces(args.at(i));
        string correct_path = find_path(cmds.at(0));

        if(correct_path == "null") {
            cerr << cmds.at(0) << ": command not found" << endl;
            return false;
        }
    }
    return true;
}

bool fork_pipe(int* in_pipe, int* out_pipe, vector<char*> &argv) {
    int pid = fork();
    if(pid == -1) {
        perror("");
        exit(1);
    }
    else if(pid == 0) {
        if(signal(SIGINT, SIG_DFL) == SIG_ERR) perror("");
        //This means we have something to read
        if(in_pipe != NULL) {
            if(dup2(in_pipe[0], 0) == -1)
                perror("dup err");
            if(close(in_pipe[1]) == -1)
                perror("close err");
            if(close(in_pipe[0]) == -1)
                perror("close err");
        }

        //This means we want to write something
        if(out_pipe != NULL) {
            if(dup2(out_pipe[1], 1) == -1)
                perror("dup err");
            if(close(out_pipe[0]) == -1)
                perror("close err");
            if(close(out_pipe[1]) == -1)
                perror("close err");
        }

        if(execv(&argv[0][0], &argv[0]) == -1) perror("");
        exit(1);
    }
    else {
        if(in_pipe != NULL) {
            if(close(in_pipe[0]) == -1)
                perror("close err");
            if(close(in_pipe[1]) == -1)
                perror("close err");
        }

        int status;
        if(wait(&status) == -1) {
            perror("");
            exit(1);
        }

        if(status != 0) return false;
        else return true;
    }
}

bool execute_pipe(vector<string> &args, unsigned int &i) {
    vector<string> cmds1 = remove_spaces(args.at(i));

    if(cmds1.at(0) == "exit") exit(0);
    if(cmds1.at(0) == "cd") return change_dir(cmds1);

    //If there is a nonexistent command anywhere,
    //pipe will not happen
    if(!valid_pipes(args)) return false;

    bool process;
    //Now let's execute the first command
    string correct_path = find_path(cmds1.at(0));

    int in_pipe[2];
    int out_pipe[2];

    if(correct_path != "null") {
        vector<char*> argv = get_arguments(cmds1);
        correct_path = correct_path + "/" + cmds1.at(0);
        argv.at(0) = (char*) correct_path.c_str();

        if(pipe(out_pipe) == -1) {
            perror("");
            exit(1);
        }

        //First execution, doesn't have an incoming pipe
        process = fork_pipe(NULL, out_pipe, argv);
        ++i;

        //Now out_pipe should have stuff in out_pipe[0]
        in_pipe[0] = out_pipe[0];
        in_pipe[1] = out_pipe[1];
    }
    else {
        cerr << cmds1.at(0) << ": command not found" << endl;
        return false;
    }

    vector<string> cmdlast = remove_spaces(args.at(i));
    correct_path = find_path(cmdlast.at(0));

    if(correct_path != "null") {
        vector<char*> argv = get_arguments(cmdlast);
        correct_path = correct_path + "/" + cmdlast.at(0);
        argv.at(0) = (char*) correct_path.c_str();

        process = fork_pipe(in_pipe, NULL, argv);
    }
    else {
        cerr << cmdlast.at(0) << ": command not found" << endl;
        return false;
    }
    return process;
}

/*bool execute_mult(vector<string> &args, unsigned int &i,
                  unsigned int n = 2)
{
    vector<string> cmds1 = remove_spaces(args.at(i));

    if(cmds1.at(0) == "exit") exit(0);

    //If there is a nonexistent command anywhere,
    //pipe will not happen
    if(!valid_pipes(args)) return false;

    bool process;
    //Now let's execute the first command
    vector<string> exec_paths = create_path();
    string correct_path = find_path(exec_paths, cmds1.at(0));

    int in_pipe[2];
    int out_pipe[2];

    if(correct_path != "null") {
        vector<char*> argv = get_arguments(cmds1);
        correct_path = correct_path + "/" + cmds1.at(0);
        argv.at(0) = (char*) correct_path.c_str();

        if(pipe(out_pipe) == -1) {
            perror("");
            exit(1);
        }

        //First execution, doesn't have an incoming pipe
        process = fork_pipe(NULL, out_pipe, argv);
        ++i;

        //Now out_pipe should have stuff in out_pipe[0]
        in_pipe[0] = out_pipe[0];
        in_pipe[1] = out_pipe[1];
    }
    else {
        cerr << cmds1.at(0) << ": command not found" << endl;
        return false;
    }

    for(unsigned int x = i; x < n-1; ++x) {
        vector<string> cmd = remove_spaces(args.at(i));
        correct_path = find_path(exec_paths, cmd.at(0));

        if(correct_path != "null") {
            vector<char*> argv = get_arguments(cmd);
            correct_path = correct_path + "/" + cmd.at(0);
            argv.at(0) = (char*) correct_path.c_str();

            if(pipe(out_pipe) == -1) {
                perror("");
                exit(1);
            }

            process = fork_pipe(in_pipe, out_pipe, argv);
            ++i;

            in_pipe[0] = out_pipe[0];
            in_pipe[1] = out_pipe[1];
        }
        else {
            cerr << cmd.at(0) << ": command not found" << endl;
            return false;
        }
    }

    vector<string> cmdlast = remove_spaces(args.at(i));
    correct_path = find_path(exec_paths, cmdlast.at(0));

    if(correct_path != "null") {
        vector<char*> argv = get_arguments(cmdlast);
        correct_path = correct_path + "/" + cmdlast.at(0);
        argv.at(0) = (char*) correct_path.c_str();

        process = fork_pipe(in_pipe, NULL, argv);
    }
    else {
        cerr << cmdlast.at(0) << ": command not found" << endl;
        return false;
    }
    return process;
}*/

bool mult_pipe(int* in_pipe, int* out_pipe, vector<char*> &argv);

bool execute_mult(vector<string> &args, vector<string> &temp,
                  unsigned int i)
{
    for(unsigned int n = 0; n < temp.size(); ++n) {
        if(temp.at(n) == "<") {

        }
        else if(temp.at(n) == ">") {

        }
        else if(temp.at(n) == ">>") {

        }
        else if(temp.at(n) == "|") {

        }
    }

    return true;
}

bool special_handler(vector<string> &args, vector<string> &conn,
                     unsigned int &i)
{
    vector<string> temp;
    unsigned int index = i;
    for(; i < conn.size(); ++i) {
        if(is_special(conn.at(i)))
            temp.push_back(conn.at(i));
        else break;
    }

    //If we just have one special connector, then it's easy
    if(temp.size() == 1) {
        if(temp.at(0) == ">") {
            return execute_out(args, index, ">");
        }
        else if(temp.at(0) == ">>") {
            return execute_out(args, index, ">>");
        }
        else if(temp.at(0) == "<") {
            return execute_in(args, index);
        }
        else if(temp.at(0) == "|") {
            return execute_pipe(args, index);
        }
    }

    //If we have more than one special connector, then it get's
    //crazy
    return execute_mult(args, temp, index);
}

void connectors(vector<string> &args, vector<string> &conn) {
    unsigned int i = 0;
    bool process = true;

    //-----Have to execute the first command-----
    //but if it's input/output stuff, have to do different stuff
    if(!conn.empty()) {
        if(is_special(conn.at(i))) {
            process = special_handler(args, conn, i);
        }
        else process = execute(args.at(i));
    }
    else process = execute(args.at(i));

    //-----Now go through the rest of the arguments-----
    for(; i < conn.size(); ++i) {
        if(conn.at(i) == ";") {
            if(i+1 != conn.size()) {
                if(is_special(conn.at(i+1))) {
                    ++i;
                    process = special_handler(args, conn, i);
                }
                else process = execute(args.at(i+1));
            }
            else process = execute(args.at(i+1));
        }
        else if(conn.at(i) == "&&") {
            if(process) {
                if(i+1 != conn.size()) {//last argument
                    //Check if next argument is a special conn
                    if(is_special(conn.at(i+1))) {
                        ++i;
                        process = special_handler(args, conn, i);
                    }
                    else process = execute(args.at(i+1));
                }
                else //What if it is the last argument?
                    process = execute(args.at(i+1));
            }
            else process = false;
        }
        else if(conn.at(i) == "||") {
            if(!process) {
                if(i+1 != conn.size()) {
                    if(is_special(conn.at(i+1))) {
                        ++i;
                        process = special_handler(args, conn, i);
                    }
                    else process = execute(args.at(i+1));
                }
                else process = execute(args.at(i+1));
            }
            else process = false;
        }
    }
}

void parse(string command) {
    //-----Check the very first command for special cases-----
    char_separator<char> delim(" \t\r\n");
    tok mytok(command, delim);
    tok::iterator it = mytok.begin();

    if(*it == "exit") exit(0);
    if(is_con(*it)) {
        cerr << "bash: syntax error" << endl;
        return;
    }

    //-----Create argument vector, may contain spaces-----
    char_separator<char> delim2(";&|#<>");
    tok mytok1(command, delim2);
    tok::iterator it1 = mytok1.begin();

    vector<string> args;
    for(; it1 != mytok1.end(); ++it1) {
        if(*it1 != " ") args.push_back(*it1);
    }

    //-----Create connector vector-----
    vector<string> conn;
    tok mytok2(command, delim);
    tok::iterator it2 = mytok2.begin();
    int syntax_err = 0;
    int multiple_special = 0;
    for(; it2 != mytok2.end(); ++it2) {
        if(is_con(*it2)) {
            conn.push_back(*it2);
            ++syntax_err;

            if(is_special(*it2) && *it2 != "|")
                ++multiple_special;

            if(multiple_special > 1) {
                cerr << "bash: syntax error" << endl;
                return;
            }

            if(syntax_err > 1) {
                cerr << "bash: syntax error" << endl;
                return;
            }
        }
        else syntax_err = 0;
    }

    if(args.size() == conn.size()) {
        cerr << "bash: syntax error" << endl;
    }
    //-----Now execute the stuff-----
    else {
        connectors(args, conn);
    }
}

void remove_comments(string &command) {
    int count = 0;
    string::iterator it = command.begin();
    for(; it != command.end(); ++it) {
        if(*it == '#') {
            command.erase(count, command.size()-count);
            return;
        }
        ++count;
    }
}

void trans_string(string &command) {
    string::iterator it;

    for(it = command.begin(); it != command.end(); ++it) {
        if(*it == ';') {
            it = command.insert(it, ' ');
            it += 2;
            it = command.insert(it, ' ');
        }
    }

    for(it = command.begin(); it != command.end(); ++it) {
        if(*it == '&' && *(it+1) == '&') {
            it = command.insert(it, ' ');
            it += 3;
            it = command.insert(it, ' ');
        }
    }

    for(it = command.begin(); it != command.end(); ++it) {
        if(*it == '|' && *(it+1) == '|') {
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
        if(*it == '>' && *(it+1) == '>') {
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
