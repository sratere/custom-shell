#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(){
    char buf[1024];

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
        
        write(STDOUT_FILENO, buffer, bytes_read);
        write(STDOUT_FILENO, "\n", 1);
    }

    exit(EXIT_SUCCESS);
}