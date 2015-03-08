#Rshell Project
**Program Overview**
*rshell*: This is a basic shell program that uses execv to call commands.
*ls*: this implements the ls command where rshell is the root folder.  It only takes in flags -a, -l, and -R.  Any other flag will be treated as if it were an invalid flag.
*cp*: Copies files and compares the runtime of different methods of copying.

**How to Use**
*rshell*: Treat this shell as if it were bash.  However, there are some limitations to this program, which are discussed below.  In order to exit the program, simply type in "exit".
*ls*: Treat this program as if it were the ls command.  In the rshell directory, call `bin/ls` and type in extra arguments after.

**Installation Guide**

 1. Clone this repository using, http://github.com/churslic/rshell.git . Or else download the zip on the same webpage.
 2. Type in `make`. All executables will be put in a bin folder.  Alternatively, you may type make and then the executable you wish to use.

**Bugs/Limitations/Issues**
*rshell*:

 - Connectors: For example, typing in `ls#pwd` into bash would not execute, but in this program, it will ignore `#pwd` and execute `ls`.  This program will also not allow connectors to be the last argument typed in.  For example `ls; pwd;`  or  `ls ||` are not allowed.
 - Exit:  Normally `exit | cat file1` would actually print out the content of `file1`, but in this shell, any time `exit` is the very first command, everything after it is ignored and the program will terminate.
 - Input/Output redirection: Multiple input calls OR multiple output calls will be treated as errors.  For example, `cat < file1 < file2` or `cat file1 > file2 > file3` will be treated as an error.

*ls*:

 - The formatting is done rather basically. It's default output is to print out with only 5 columns of minimum 14 width.  If the file name were to exceed this length, then the output will look crazy.
