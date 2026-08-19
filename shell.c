#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
        if(args[0] != NULL && strcmp(args[0], "exit") == 0){
            write(STDOUT_FILENO, "exit", 4);
            break;
        }

        for(int j = 0; j < i; j++){
            write(STDOUT_FILENO, args[j], strlen(args[j]));
            write(STDOUT_FILENO, "\n", 1);
        }

    }

    return 0;
}