#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    pid_t children[10];
    int fileDescriptorRead;
    int fileDescriptorWrite;
    int i;
    char* buffer;
    char* remainingBuffer;
    size_t bufferSize = 512;
    ssize_t bytesRead;

    buffer = (char*)malloc(bufferSize * sizeof(char));
    remainingBuffer = (char*)malloc(bufferSize * sizeof(char));

    if (argc < 3) {
        printf("Insufficient amount of arguments sent to the command line\n");
        exit(-1);
    }

    for (i = 0; i < argc - 2; ++i) {
        children[i] = fork();

        if (children[i] == 0) {
            break;
        }
    }

    if (i < argc - 2) {
        if ((fileDescriptorRead = open(argv[1], O_RDONLY, 0666)) == -1) {
            printf("Unable to open file for reading\n");
            exit(-1);
        }

        if ((fileDescriptorWrite = open(argv[i + 2], O_WRONLY | O_CREAT, 0666)) == -1) {
            printf("Unable to open file for writing\n");
            exit(-1);
        }

        if (dup2(fileDescriptorWrite, 1) == -1) {
            printf("Unable to redirect standard output\n");
            exit(-1);
        }

        close(fileDescriptorWrite);

        while ((bytesRead = read(fileDescriptorRead, buffer, bufferSize)) == bufferSize) {
            printf("%s", buffer);
        }

        strncpy(remainingBuffer, buffer, bytesRead);
        printf("%s", remainingBuffer);
    }
    else {
        for (i = 0; i < argc - 2; ++i) {
            waitpid(children[i], NULL, WAIT_MYPGRP);
        }

        close(fileDescriptorRead);
    }

    free(buffer);
    free(remainingBuffer);
    return 0;
}