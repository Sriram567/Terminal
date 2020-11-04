1. I have written the code for 'ls' command in ls.c file.
2. To print the permissions of files and directories I have created a new file permissions.c file and wrote the logic in that.
3. To implememt pinfo command Ihave created pinfo.c file.
4. I have assumed that only Directories are given as input to the ls command since it is explicity mentioned for Directory Name.
5. I have created a file  named  history.c . I contains the code regarding history of commands.
6. I have created a nightwatch.c , it contains the code regarding the nightswatch command. I have assumed that the order of the parameters will be given constatn in nightswatch command.
7. Since cd command cannot accept more than one arguments , if more are provided i have printed an error.
8. I have included much of the interaction part in 1.c file.

HOW TO RUN:
1. type make on the terminal . It generates a executable file .
2. Run the executable generated.


WORK :
1. I have used stat for implementing ls command, chdir for cd command , getpwd for pwd command . I have used execvp for executing other commands. I have used setpgid syscall to run background process. I am creating a file to store the history of commands. I am using /proc/[pid]/stat and /proc/[pid]/exe for implemeting oinfo command. I am using signals to generate message when a background processs exited. I am using /proc/loadavg and /proc/interripts to implement nightswatch command.

2. For the kjob command it have put a smiling character if the command kills the specified process successfully.Even if the process is made to exit forcefully I have put a smiling character to if the killing process is successful.


3. For parsing the commands I have passed it to the function checkre2 which parses by @ and $ symbols then it is passed to Gotopipe function which parses the command at pipes(|) which then it is passed to the checkre function which checks for the redirection present  in command , if presents it execute dup2 on STDIN or STDOUT to meet the requirements. 

4. To execute the fg command I am using tcsetpgrp to give the STDIN and STDOUT control to the background process groups. After interrupt or terminating I have given back the control to the foreground parent process. For bg and kjob command I have used kill system call to send the signal to specified process. 

5. I have used the return values from commands to print the smiley symbols. 

6. For implementing pipes I have used dup2 for changing STDIN and STDOUT to pipes to make the commands read from pipes and write to the pipes.

7. For setting environmental variables I have used setenv command.For unsetting I have used unsetenv command. 

8. I have assumed that every non o exit is unsuccessful.

9. To implementing the CTRL Z and CTRL C i have ignored the corresponding signals for parent process and reset them to normal in the child processes .By pressing ctrl D the program exits .

