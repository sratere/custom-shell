#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "customFunctions.h"
#include <fcntl.h>

int main(){
    char buf[1024];
    char *args[100];

    while(1){
        write(STDOUT_FILENO, "custom-shell> ", 14);
        
        char buffer[1024];
        ssize_t bytes_read = read(STDIN_FILENO, buffer, 1024);
        if(bytes_read == 0) break;
        if(bytes_read < 0) {
            //some error
            perror("error reading the input");
            break;
        }

        if(buffer[bytes_read - 1] == '\n') {
            buffer[bytes_read - 1] = '\0';
        }

        if(strcmp(buffer, "exit\0") == 0){
            break;
        }

        write(STDOUT_FILENO, "buffer contents:", 17);
        write(STDOUT_FILENO, buffer, bytes_read); //just a printer to check
        write(STDOUT_FILENO, "\n", 1);

        int i = 0;
        args[i] = strtok(buffer, " ");
        while(args[i] != NULL){
            i++;
            args[i] = strtok(NULL, " ");
        }

        //another printer to check
        write(STDOUT_FILENO, "args contents:\n", 15);

        if(args[0] != NULL && strcmp(args[0], "exit") == 0){//exit functionality
            break;
        }

        if(args[0] != NULL && strcmp(args[0], "cd") == 0){//cd functionality
            if(args[1] != NULL){
                if(chdir(args[1]) != 0){ //mnonzero means failure
                    write(STDERR_FILENO, "cd failed\n", 10);
                }
            } else {
                write(STDERR_FILENO, "cd: missing argument\n", 21);
            }
            continue;// to restart the loop
        }

        for(int j = 0; j < i; j++){
            write(STDOUT_FILENO, args[j], strlen(args[j]));
            write(STDOUT_FILENO, "\n", 1);
        }

        //execute the command in a child process
        pid_t pid = fork();
        if(pid < 0){
            write(STDERR_FILENO, "fork failed\n", 12);
            exit(EXIT_FAILURE);
        } else if(pid == 0){
            for(int j = 0; j < i; j++){
                if(strcmp(args[j], ">") == 0){
                    args[j] = NULL; // terminate the command arguments
                    if(args[j+1] == NULL){
                        write(STDERR_FILENO, "missing file for output redirection\n", 36);
                        exit(EXIT_FAILURE);
                    }
                    int fd = open(args[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if(fd < 0){
                        write(STDERR_FILENO, "failed to open file for output redirection\n", 44);
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDOUT_FILENO); // redirect stdout to the file
                    close(fd);
                    break;
                } else if(strcmp(args[j], "<") == 0){
                    args[j] = NULL; // terminate the command arguments
                    if(args[j+1] == NULL){
                        write(STDERR_FILENO, "missing file for input redirection\n", 36);
                        exit(EXIT_FAILURE);
                    }
                    int fd = open(args[j + 1], O_RDONLY);
                    if(fd < 0){
                        write(STDERR_FILENO, "failed to open file for input redirection\n", 43);
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDIN_FILENO); // redirect stdin from the file
                    close(fd);
                    break;
                }
                if(customExecvp(args[0], args) == -1){
                write(STDERR_FILENO, "command not found\n", 18);    
                exit(EXIT_FAILURE);
            }
            }
        } else if(pid > 0){
            waitpid(pid, NULL, 0);//wait for command in child process to finish
        }
    }

    return 0;
}