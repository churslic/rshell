# rshell project

*Program Overview*
This is a basic shell program that uses execvp to carry out commands ONLY from
the bin folder.

*How to use*
1. The program will have a prompt. 
2. Type in a linux command.
3. To exit the program, type in "exit".

*Installation guide*
1. make
2. bin/rshell

*Bugs/Limitations/Issues*
1. This program does not exactly match regular bash commands. For example 
   typing in "ls#pwd" into bash would not execute, but in this rshell program
   it will ignore #pwd and execute ls.
