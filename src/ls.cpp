#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>

using namespace std;


void print_l (struct stat &s) {
    if(s.st_mode & S_IFDIR) cout << 'd';
    else if(s.st_mode & S_IFLNK) cout << 'l';
    else cout << '-';
}

void ls_l (DIR *dirp) {
//int stat(const char *path, struct stat *buf);
    dirent *direntp;
    struct stat s;
    vector<char *> v;

    while((direntp = readdir(dirp))) {
        if(stat(direntp->d_name, &s) == -1) perror("stat error");

    }
    cout << endl;
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
    sort(it, v.end());
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

    sort(v.begin(), v.end());
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

int main(int argc, char** argv) {
    //For the case that the only command would be "bin/ls"
    if(argc == 1) {
        const char *dirName = ".";
        DIR *dirp = opendir(dirName);
        if(dirp == NULL) perror("opendir error");

        print_ls(dirp);

        if(closedir(dirp) == -1) perror("closedir error");
    }
    //For the case that there may be flags or commands typed
    //in after "bin/ls". Must first check if argv[1] has a flag
    else {
        if(argv[1][0] == '-') {
            const char *dirName = ".";
            DIR *dirp = opendir(dirName);
            if(dirp == NULL) perror("opendir error");

            print_ls_a(dirp);

            if(closedir(dirp) == -1) perror("closedir error");
        }
        else {
            cout << "FIX ME" << endl;
        }
    }

    return 0;
}
