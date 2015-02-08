#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

bool mycomp (const char *a, const char *b) {
    int temp_a = 0;
    int temp_b = 0;

    int i = 0;
    int j = 0;

    //Want to skip the hidden file mark in alphabetizing
    if(a[0] == '.') ++i;
    if(b[0] == '.') ++j;

    if(a[i] == b[j]) {
        while(a[i] == b[j] && a[i] != '\0' && b[j] != '\0') {
            temp_a += tolower(a[i]);
            temp_b += tolower(b[j]);
            ++i;
            ++j;
        }
    }
    else {
        temp_a += tolower(a[i]);
        temp_b += tolower(b[j]);
    }

    return(temp_a < temp_b);
}

bool argscomp (const char* a, const char *b) {
    int temp_a = 0;
    int temp_b = 0;

    int i = 0;
    int j = 0;

    if(a[0] == '-') temp_a = -1;
    else if(a[0] == '.') ++i;

    if(b[0] == '-') temp_b = -1;
    else if(b[0] == '.') ++j;

    if(temp_a != -1 || temp_b != -1) {
        if(a[i] == b[j]) {
            while(a[i] == b[j] && a[i] != '\0' && b[j] != '\0') {
                temp_a += tolower(a[i]);
                temp_b += tolower(b[j]);
                ++i;
                ++j;
            }
        }
        else {
            temp_a += tolower(a[i]);
            temp_b += tolower(b[j]);
        }
    }

    return(temp_a < temp_b);
}

vector<const char *> create_vec_ls_a(DIR *dirp) {
    dirent *direntp;
    vector<const char *> v;

    const char *curr = ".";
    const char *parent = "..";
    v.push_back(curr);
    v.push_back(parent);

    while((direntp = readdir(dirp))) {
        if(direntp == NULL && errno != 0) perror("readdir error");

        if(strcmp(direntp->d_name, ".") != 0 &&
           strcmp(direntp->d_name, "..") != 0) {
            v.push_back(direntp->d_name);
        }
    }

    vector<const char*>::iterator it = v.begin()+2;
    sort(it, v.end(), mycomp);
    return v;
}


void print_ls_a(DIR *dirp) {
    vector<const char*> v = create_vec_ls_a(dirp);

    int x = 1;
    for(unsigned int i = 0; i < v.size(); ++i) {
        printf("%-14s", v.at(i));
        ++x;
        if(x % 6 == 0) cout << endl;
    }

    if(x % 6 != 0) cout << endl;
}


vector<const char *> create_vec_ls(DIR *dirp) {
    dirent *direntp;
    vector<const char *> v;

    while((direntp = readdir(dirp))) {
        if(direntp == NULL && errno != 0) perror("readdir error");

        if(direntp->d_name[0] != '.') {
            v.push_back(direntp->d_name);
        }
    }

    sort(v.begin(), v.end(), mycomp);
    return v;
}


void print_ls(DIR *dirp) {
    vector<const char *> v = create_vec_ls(dirp);

    int x = 1;
    for(unsigned int i = 0; i < v.size(); ++i) {
        printf("%-14s", v.at(i));
        ++x;
        if(x % 6 == 0) cout << endl;
    }

    if(x % 6 != 0) cout << endl;
}

string month (int m) {
    if(m == 0) return "Jan";
    else if(m == 1) return "Feb";
    else if(m == 2) return "Mar";
    else if(m == 3) return "Apr";
    else if(m == 4) return "May";
    else if(m == 5) return "Jun";
    else if(m == 6) return "Jul";
    else if(m == 7) return "Aug";
    else if(m == 8) return "Sep";
    else if(m == 9) return "Oct";
    else if(m == 10) return "Nov";
    else return "Dec";
}


void l_info (struct stat &s) {
    if((s.st_mode & S_IFDIR)) cout << 'd';
    else if((s.st_mode & S_IFREG)) cout << '-';
    else if((s.st_mode & S_IFLNK)) cout << 'l';

    cout << ((s.st_mode & S_IRUSR) ? 'r' : '-');
    cout << ((s.st_mode & S_IWUSR) ? 'w' : '-');
    cout << ((s.st_mode & S_IXUSR) ? 'x' : '-');

    cout << ((s.st_mode & S_IRGRP) ? 'r' : '-');
    cout << ((s.st_mode & S_IWGRP) ? 'w' : '-');
    cout << ((s.st_mode & S_IXGRP) ? 'x' : '-');

    cout << ((s.st_mode & S_IROTH) ? 'r' : '-');
    cout << ((s.st_mode & S_IWOTH) ? 'w' : '-');
    cout << ((s.st_mode & S_IXOTH) ? 'x' : '-');

    cout << ' ' << s.st_nlink << ' ';

    struct passwd *pw = getpwuid(s.st_uid);
    if(pw == NULL) perror("getpwuid error");

    struct group *gr = getgrgid(s.st_gid);
    if(gr == NULL) perror("getgrgid error");

    cout << pw->pw_name << ' ' << gr->gr_name << ' ';

    printf(" %5li ", s.st_size);

    struct tm result;
    time_t time = s.st_mtime;
    localtime_r(&time, &result);

    cout << month(result.tm_mon) << ' ';

    printf(" %2i ", result.tm_mday);
    printf("%02i%c%02i ", result.tm_hour, ':', result.tm_min);
}

void disk_blocks(vector<const char *> &v, const char *dirName) {
    struct stat s;
    int blocks = 0;

    for(unsigned int i = 0; i < v.size(); ++i) {
        string temp = dirName;
        temp.append("/");
        temp.append(v.at(i));
        const char *tempName = temp.c_str();

        if(stat(tempName, &s) == -1) perror("stat error");
        blocks += s.st_blocks;
    }
    blocks = blocks / 2;
    cout << "total: " << blocks << endl;
}

void print_ls_l (DIR *dirp, const char *dirName, bool a, bool R) {
    struct stat s;

    vector<const char *> v;
    if(a) v = create_vec_ls_a(dirp);
    else v = create_vec_ls(dirp);

    disk_blocks(v, dirName);

    for(unsigned int i = 0; i < v.size(); ++i) {
        string temp = dirName;
        temp.append("/");
        temp.append(v.at(i));
        const char* tempName = temp.c_str();

        if(stat(tempName, &s) == -1) perror("stat error");
        l_info(s);
        cout << v.at(i) << endl;
    }
}

void print_ls_R(DIR *dirp, const char *dirName) {
    char *path = NULL;
    path = getcwd(NULL, 0);
    if(path == NULL) perror("realpath error");

    printf("%s%c\n", path, ':');

    vector<const char*> v = create_vec_ls(dirp);
    int x = 1;
    for(unsigned int i = 0; i < v.size(); ++i) {
        printf("%-14s", v.at(i));
        ++x;
        if(x % 6 == 0) cout << endl;
    }
    if(x % 6 != 0) cout << endl;

    struct stat s;
    for(unsigned int i = 0; i < v.size(); ++i) {
        if(stat(v.at(i), &s) == -1) perror("stat error");
        if(s.st_mode & S_IFDIR) {
            string temp = dirName;
            temp.append("/");
            temp.append(v.at(i));

            const char *tempName = temp.c_str();

            DIR *newdirp = opendir(tempName);
            if(newdirp == NULL) perror("opendir error");

            print_ls_R(newdirp, tempName);

            if(closedir(newdirp) == -1) perror("closedir error");
        }
    }
    return;
}

void print_flag_helper (DIR *dirp, const char* dirName,
                        bool a, bool l, bool R) {
    if(a && !l && !R) print_ls_a(dirp);
    else if(l && !R) print_ls_l(dirp, dirName, a, R);
}

int main(int argc, char** argv) {
    bool a = false;
    bool l = false;
    bool R = false;
    bool flag_found = false;

    //For the case that the only command would be "bin/ls"
    if(argc == 1) {
        const char *dirName = ".";
        DIR *dirp = opendir(dirName);
        if(dirp == NULL) perror("opendir error");

        print_ls(dirp);

        if(closedir(dirp) == -1) perror("closedir error");
    }
    //For the case that there are more arguments
    else {
        //Create vector, sorted to have flags in the front
        vector<const char*> args;
        for(int i = 1; i < argc; ++i) args.push_back(argv[i]);
        sort(args.begin(), args.end(), argscomp);

        //Will go through vector up until there are no more flags
        unsigned int i = 0;
        for(; i < args.size(); ++i) {
            if(args.at(i)[0] == '-') {
                flag_found = true;
                if(strchr(args.at(i), 'a') != NULL) a = true;
                if(strchr(args.at(i), 'l') != NULL) l = true;
                if(strchr(args.at(i), 'R') != NULL) R = true;
            }
            else break;
        }

        //If a flag was found, but no valid flag input
        //This will not execute anything if there were no
        //valid flags
        if(!a && !l && !R && flag_found) {
            cout << "ls: invalid option -- \'"
                 << args.at(1)[0] << "\'" << endl;
            return 0;
        }
        //If i < args.size() then there must be more commands
        //The way the vector is sorted, the argument at i
        //should be a file name or a directory.
        if(i < args.size()) {
            for(; i < args.size(); ++i) {
                const char *dirName = args.at(i);
                DIR *dirp = opendir(dirName);
                if(dirp == NULL) perror("opendir error");

                print_flag_helper(dirp, dirName, a, l, R);

                if(closedir(dirp) == -1) {
                    perror("closedir error");
                }
            }
        }
            //If not, then flags were the only things input
        else {
            const char *dirName = ".";
            DIR *dirp = opendir(dirName);
            if(dirp == NULL) perror("opendir error");

            print_flag_helper(dirp, dirName, a, l, R);

            if(closedir(dirp) == -1) {
                perror("closedir error");
            }
        }
    }

    return 0;
}
