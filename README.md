# rshell project

*Program Overview*

rshell: This is a basic shell program that uses execvp to
carry out commands ONLY from the bin folder.

ls: This implements the ls command where rshell is the root
folder. It only takes in flags -a, -l, and -R. Any other flag will
be treated as if it were an invalid flag.

*How to use*

rshell:

1. The program will have a prompt.

2. Type in a linux command.

3. To exit the program, type in "exit".

ls:

1. Executing this program without arguments will be as if
   you're calling "ls".

2. Type in flags and file/directory names after calling "bin/ls"
        example: bin/ls -a src

*Installation guide*

1. Clone this repository using
   http://github.com/churslic/rshell.git
   Or download the zip.

2. Type in "make" (Executables will be in the bin folder)

*Bugs/Limitations/Issues*

rshell:

1. This program does not exactly match regular bash commands.
   For example typing in "ls#pwd" into bash would not execute,
   but in this rshell program it will ignore #pwd and execute ls.

ls:

1. The formatting is done rather basically. It's default output
   is to print out with only 5 columns of minimum 14 width. If
   the file name were to exceed this length, then the output will
   look crazy.
