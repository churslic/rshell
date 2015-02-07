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

void disk_blocks(vector<const char *> &v) {
    struct stat s;
    int blocks = 0;

    for(unsigned int i = 0; i < v.size(); ++i) {
        if(stat(v.at(i), &s) == -1) perror("stat error");
        blocks += s.st_blocks;
    }
    blocks = blocks / 2;
    cout << "total: " << blocks << endl;
}

void print_ls_l (DIR *dirp, bool a, bool R) {
    struct stat s;

    vector<const char *> v;
    if(a) v = create_vec_ls_a(dirp);
    else v = create_vec_ls(dirp);

    disk_blocks(v);

    for(unsigned int i = 0; i < v.size(); ++i) {
        if(stat(v.at(i), &s) == -1) perror("stat error");
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

void print_flag_helper (DIR *dirp, bool a, bool l, bool R) {
    if(a && !l && !R) print_ls_a(dirp);
    else if(a && l && !R) print_ls_l(dirp, a, R);
}

int main(int argc, char** argv) {
    bool a = false;
    bool l = false;
    bool R = false;

    //For the case that the only command would be "bin/ls"
    if(argc == 1) {
        const char *dirName = ".";
        DIR *dirp = opendir(dirName);
        if(dirp == NULL) perror("opendir error");

        print_ls(dirp);

        if(closedir(dirp) == -1) perror("closedir error");
    }
    //For the case that there may be flags or commands typed
    //in after "bin/ls".
    else {
        //This if statement checks to see if the very first
        //character of the first argument is a '-'. Then we know
        //that this argument and possibly the next arguments
        //are flags.
        //BUG FIX: There may be a case where we pass in:
        //  ls - a
        //This should be treated as if '-' is a file or directory
        //and 'a' is a file or directory
        if(argv[1][0] == '-' && argv[1][1] != '\0') {

            //This for loop will continue to treat the next
            //arguments as flags until there is no more '-'.
            int i = 1;
            for( ; argv[i][0] == '-' && argv[i] != NULL; ++i) {
                if(strchr(argv[i], 'a') != NULL) a = true;
                if(strchr(argv[i], 'l') != NULL) l = true;
                if(strchr(argv[i], 'R') != NULL) R = true;
            }

            //If a, l, and R evaluate to false, then we have an
            //invalid flag. Normally, ls will print out:
            //  ls: invalid option -- '(flag)'
            //where (flag) is the very first invalid flag.
            //So if we passed in ls -ez, it will print out:
            //  ls: invalid option -- 'e'
            //Passing in invalid flags also will negate anything
            //that comes after it, so "ls -z src" will not work
            if(!a && !l && !R) {
                cout << "ls: invalid option -- \'"
                     << argv[1][0] << "\'" << endl;
            }
            //If we go to this else statement, then we have flags.
            //But what if we have some directory names afterwards?
            //What if we have file names?
            else {
                if(argv[i] != NULL) {
                    for( ; argv[i] != NULL; ++i) {
                        const char *dirName = argv[i];
                        DIR *dirp = opendir(dirName);
                        if(dirp == NULL) perror("opendir error");

                        print_flag_helper(dirp, a, l, R);

                        if(closedir(dirp) == -1) {
                            perror("closedir error");
                        }
                    }
                }
                else {
                    const char *dirName = ".";
                    DIR *dirp = opendir(dirName);
                    if(dirp == NULL) perror("opendir error");

                    print_flag_helper(dirp, a, l, R);

                    if(closedir(dirp) == -1) {
                        perror("closedir error");
                    }
                }
            }
        }
        //This else statement means that we must treat the next
        //arguments as if they're a path for ls
        else {
            cout << "FIX ME" << endl;
        }
    }

    return 0;
}
