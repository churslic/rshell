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
#include <list>

using namespace std;

bool mycomp(const char *a, const char *b)
{
    int temp_a = 0;
    int temp_b = 0;

    int i = 0;
    int j = 0;
    if(a[0] == '.') ++i;
    if(b[0] == '.') ++j;

    if(tolower(a[i]) == tolower(b[j]))
    {
        while(tolower(a[i]) == tolower(b[j]) && a[i] != '\0'
              && b[j] != '\0')
        {
            temp_a += tolower(a[i]);
            temp_b += tolower(b[j]);
            ++i; ++j;
        }
    }
    else
    {
        temp_a += tolower(a[i]);
        temp_b += tolower(b[j]);
    }

    return (temp_a < temp_b);
}


int is_dir(const char *dirName, const char *parent = ".")
{
    struct stat s;

    string temp = parent;
    temp.append("/");
    temp.append(dirName);
    const char *tempName = temp.c_str();

    //Returning -1 means that the path
    //doesn't exit
    if(stat(tempName, &s) == -1)
    {
        perror(NULL);
        return -1;
    }

    //Returning 1 means that it is a dir
    //Returning 0 means that it is a file
    if((s.st_mode&S_IFDIR) != 0) return 1;
    else return 0;
}


vector<const char*> create_vec_ls(DIR *dirp)
{
    dirent *direntp;
    vector<const char *> v;

    while((direntp = readdir(dirp)))
    {
        if(direntp == NULL && errno != 0) perror("readdir error");

        if(direntp->d_name[0] != '.')
        {
            v.push_back(direntp->d_name);
        }
    }

    sort(v.begin(), v.end(), mycomp);
    return v;
}


vector<const char*> create_vec_ls_a(DIR *dirp)
{
    dirent *direntp;
    vector<const char*> v;

    const char *curr = ".";
    const char *parent = "..";
    v.push_back(curr);
    v.push_back(parent);

    while((direntp = readdir(dirp)))
    {
        if(direntp == NULL && errno != 0) perror("readdir error");

        if(strcmp(direntp->d_name, ".") != 0 &&
           strcmp(direntp->d_name, "..") != 0)
        {
            v.push_back(direntp->d_name);
        }
    }

    vector<const char*>::iterator it = v.begin()+2;
    sort(it, v.end(), mycomp);
    return v;
}


void print_ls(DIR *dirp, bool a = false)
{
    vector<const char*> v;
    if(a) v = create_vec_ls_a(dirp);
    else v = create_vec_ls(dirp);

    int x = 1;
    for(unsigned int i = 0; i < v.size(); ++i)
    {
        printf("%-14s", v.at(i));
        ++x;
        if(x % 6 == 0) cout << endl;
    }

    if(x % 6 != 0) cout << endl;
}


string month(int m)
{
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


void l_info(struct stat &s)
{
    if((s.st_mode&S_IFDIR) != 0) cout << 'd';
    else if((s.st_mode&S_IFREG) != 0) cout << '-';
    else if((s.st_mode&S_IFLNK) != 0) cout << 'l';

    cout << ((s.st_mode&S_IRUSR) ? 'r' : '-');
    cout << ((s.st_mode&S_IWUSR) ? 'w' : '-');
    cout << ((s.st_mode&S_IXUSR) ? 'x' : '-');

    cout << ((s.st_mode&S_IRGRP) ? 'r' : '-');
    cout << ((s.st_mode&S_IWGRP) ? 'w' : '-');
    cout << ((s.st_mode&S_IXGRP) ? 'x' : '-');

    cout << ((s.st_mode&S_IROTH) ? 'r' : '-');
    cout << ((s.st_mode&S_IWOTH) ? 'w' : '-');
    cout << ((s.st_mode&S_IXOTH) ? 'x' : '-');

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
    printf("%02i%c%-2i ", result.tm_hour, ':', result.tm_min);
}


void disk_blocks(vector<const char*> &v, const char *dirName)
{
    struct stat s;
    int blocks = 0;

    for(unsigned int i = 0; i < v.size(); ++i)
    {
        string temp = dirName;
        temp.append("/");
        temp.append(v.at(i));
        const char *tempName = temp.c_str();

        if(stat(tempName, &s) == -1) perror("stat error");
        blocks += s.st_blocks;
    }
    blocks = blocks / 2;
    cout << "total " << blocks << endl;
}


void print_ls_l (DIR *dirp, const char *dirName, bool a, bool R)
{
    struct stat s;
    vector<const char*> v;

    if(a) v = create_vec_ls_a(dirp);
    else v = create_vec_ls(dirp);

    disk_blocks(v, dirName);

    for(unsigned int i = 0; i < v.size(); ++i)
    {
        string temp = dirName;
        temp.append("/");
        temp.append(v.at(i));
        const char *tempName = temp.c_str();

        if(stat(tempName, &s) == -1) perror("stat error");
        l_info(s);
        cout << v.at(i) << endl;
    }
}


void print_flags(DIR *dirp, const char *dirName, bool a, bool l,
                 bool R)
{
    if(!a && !l && !R) print_ls(dirp);
    if(a && !l && !R) print_ls(dirp, a);
    if(l && !R) print_ls_l(dirp, dirName, a, R);
}


int main(int argc, char **argv)
{
    if(argc == 1)
    {
        const char *dirName = ".";
        DIR *dirp = opendir(dirName);
        if(dirp == NULL) perror("opendir error");

        print_ls(dirp);

        if(closedir(dirp) == -1) perror("closedir error");
    }
    else
    {
        //First check to see if we have flags
        vector<const char*> flags;
        for(int i = 1; i < argc; ++i)
        {
            if(argv[i][0] == '-')
                flags.push_back(argv[i]);
        }

        bool found_flag = false;
        if(!flags.empty()) found_flag = true;

        bool a = false;
        bool l = false;
        bool R = false;

        //Check to see if we have valid flags
        for(unsigned int i = 0; i < flags.size(); ++i)
        {
            if(strchr(flags.at(i), 'a') != NULL) a = true;
            if(strchr(flags.at(i), 'l') != NULL) l = true;
            if(strchr(flags.at(i), 'R') != NULL) R = true;
        }

        //If we have no valid flags, then nothing gets
        //executed
        if(!a && !l && !R && found_flag)
        {
            cout << "ls: invalid option -- \'"
                 << flags.at(1)[0] << "\'" << endl;
            return 1;
        }

        //Now let's make a vector of the file/dir names
        vector<const char*> names;
        for(int i = 1; i < argc; ++i)
        {
            if(argv[i][0] != '-')
                names.push_back(argv[i]);

        }

        bool found_names = false;
        if(!names.empty()) found_names = true;

        //If we found names, execute them
        if(found_names)
        {
            //If there is only one argument then you
            //don't have to print out the name of the dir
            //(if it's a directory)
            if(names.size() == 1)
            {
                const char *dirName = names.at(0);
                int check = is_dir(dirName);
                if(check == 1)
                {
                    DIR *dirp = opendir(dirName);
                    if(dirp == NULL) perror("");
                    print_flags(dirp, dirName, a, l, R);
                    if(closedir(dirp) == -1) perror("");
                }
                else if(check == 0)
                {
                    if(l)
                    {
                        struct stat s;
                        if(stat(dirName, &s) == -1)
                            perror("stat error");
                        l_info(s);
                        printf("%s", dirName);
                    }
                    else printf("%-14s", dirName);
                    cout << endl;
                }

            }
            //If there's multiple files/dir then dir
            //names have to be printed out
            else
            {
                vector<const char*> files;
                vector<const char*> dir;

                for(unsigned int i = 0; i < names.size(); ++i)
                {
                    const char *dirName = names.at(i);
                    int check = is_dir(dirName);
                    if(check == 1)
                        dir.push_back(names.at(i));
                    else if(check == 0)
                        files.push_back(names.at(i));
                }

                if(!files.empty())
                {
                    sort(files.begin(), files.end(), mycomp);
                    unsigned int i = 0;
                    for(; i < files.size(); ++i)
                    {
                        printf("%-14s", files.at(i));
                        if((i+1) % 6 == 0) cout << endl;
                    }
                    if(i % 6 != 0) cout << endl;
                    if(!dir.empty()) cout << endl;
                }

                if(!dir.empty())
                {
                    sort(dir.begin(), dir.end(), mycomp);
                    for(unsigned int i = 0; i < dir.size(); ++i)
                    {
                        printf("%s%c\n", dir.at(i), ':');

                        const char *dirName = dir.at(i);
                        DIR *dirp = opendir(dirName);
                        if(dirp == NULL) perror("");
                        print_flags(dirp, dirName, a, l, R);
                        if(closedir(dirp) == -1) perror("");

                        if(i != dir.size()-1) cout << endl;
                    }
                }

            }
        }
        //If we didn't find names, then execute the flags
        //At this point there must have been valid flags
        else
        {
            const char *dirName = ".";
            DIR *dirp = opendir(dirName);
            if(dirp == NULL) perror("opendir error");

            print_flags(dirp, dirName, a, l, R);

            if(closedir(dirp) == -1) perror("closedir error");
        }
    }
    return 0;
}
