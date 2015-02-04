#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>

#include <iostream>
#include <string.h>

using namespace std;

void ls_a (DIR *dirp) {
    dirent *direntp;
    int i = 1;
    while((direntp = readdir(dirp))) {
        if (direntp == NULL && errno != 0) perror("readdir error");
        printf("%-14s", direntp->d_name);
        ++i;

        if (i % 6 == 0) cout << endl;
    }
    cout << endl;
    if (closedir(dirp) == -1) perror("closedir error");
}

void reg_ls (DIR *dirp) {
    dirent *direntp;
    int i = 1;
    while((direntp = readdir(dirp))) {
        if (direntp == NULL && errno != 0) perror("readdir error");
        if (direntp->d_name[0] != '.') {
            printf("%-14s", direntp->d_name);
            ++i;
        }

        if (i % 6 == 0) cout << endl;
    }
    cout << endl;
    if (closedir(dirp) == -1) perror("closedir error");
}

int main(int argc, char** argv) {
    char const *dirName = ".";

    DIR *dirp = opendir(dirName);
    if (dirp == NULL) perror("opendir error");

    //reg_ls(dirp);
    ls_a(dirp);

    /*dirent *direntp;
    while ((direntp = readdir(dirp))) {
        if (direntp == NULL && errno != 0) perror("readdir error");
        printf("%10s", direntp->d_name);
        //cout << direntp->d_name;
    }
    cout << endl;
    if (closedir(dirp) == -1) perror("closedir error");*/

    return 0;
}
