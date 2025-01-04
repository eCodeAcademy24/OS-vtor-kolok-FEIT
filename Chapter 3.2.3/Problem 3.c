#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    pid_t child;
    int processPipe[2];
    int fileDescriptor;
    char* input;
    char* buffer;

    input = (char*)malloc(100 * sizeof(char));
    buffer = (char*)malloc(100 * sizeof(char));

    if (pipe(processPipe) == -1) {
        printf("UNable to create pipe between processes\n");
        exit(-1);
    }

    child = fork();

    if (child == 0) {
        close(processPipe[1]);

        if (dup2(processPipe[0], 0) == -1) {
            printf("Unable to redirect standard input\n");
            exit(-1);
        }

        close(processPipe[0]);

        if ((fileDescriptor = open("File.txt", O_RDONLY, 0666)) == -1) {
            printf("Unable to open file for reading\n");
            exit(-1);
        }

        scanf("%s", input);
        read(fileDescriptor, buffer, strlen(input));

        if (strcmp(input, buffer) == 0) {
            printf("The contents of the file and the input are equal\n");
        }
        else {
            printf("The contents of the file and the input are not equal\n");
        }
    }
    else {
        close(processPipe[0]);

        if (dup2(1, fileDescriptor) == -1) {
            printf("Unable to redirect standard output\n");
            exit(-1);
        }

        if (dup2(processPipe[1], 1) == -1) {
            printf("Unable to redirect standard output\n");
            exit(-1);
        }

        close(processPipe[1]);

        write(fileDescriptor, "Please enter some text\n", 24);
        scanf("%s", input);
        printf("%s", input);
    }

    free(input);
    free(buffer);
    return 0;
}