#include "customFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

extern char** environ;

char* customGetenv(const char* name){
    if(name == NULL && environ == NULL){
        return NULL;
    }

    size_t nameLen = strlen(name);
    for( int i = 0; environ[i] != NULL; i++){//loop through all environment variables
        if(strncmp(environ[i], name, nameLen) == 0){//check for the name of the environment variable
            if(environ[i][nameLen] == '='){ //check for the equal sign after the name of the environment variable
                return &environ[i][nameLen + 1];//look past the equal sign, thats the value of the environment variable
            }
        }
    }

    return NULL;//havent found the environment variable
}

int customExecvp(char* command, char* args[]){//command is the first argument in args and is actually a file path
    //look for a slash character in command(check if its a path)
    if(strchr(command, '/') != NULL){
        return execve(command, args, environ);//execve is a kernel level function
    }

    //fetch PATH
    char* path = customGetenv("PATH");
    if(path == NULL){
        return -1;
    }

    char pathCopy[1024];
    strncpy(pathCopy, path, sizeof(pathCopy) - 1);
    pathCopy[sizeof(pathCopy) - 1] = '\0';

    //split into dirs
    char* dir = strtok(pathCopy, ":");
    while(dir != NULL){
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, command); //always doing a shallow search for the command in the PATH directories

        if(access(fullPath, X_OK) == 0){//check if the file is executable
            return execve(fullPath, args, environ);
        }

        dir = strtok(NULL, ":");//next dir
    }

    return -1;//command not found in PATH
}