#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>

#include <iostream>

using namespace std;

int main() {
    char *dirName = ".";

    Dir *dirp = opendir(dirName);
    if (dirp == NULL) perror("opendir error");

    dirent *direntp;
    while (direntp = readdir(dirp)) {
        if (direntp == NULL) perror("readdir error");
        cout << direntp->d_name << endl;
    }
    if (closedir(dirp) == -1) perror("closedir error");

    return 0;
}
