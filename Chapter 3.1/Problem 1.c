#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    int fileDescriptors[10];
    int i;
    char* buffer;
    size_t bufferSize = 512;
    ssize_t bytesRead;

    buffer = (char*)malloc(bufferSize * sizeof(char));

    if (argc < 3) {
        printf("Insufficient number of arguments sent to the command line\n");
        exit(-1);
    }

    if ((fileDescriptors[0] = open(argv[1], O_RDONLY, 0666)) == -1) {
        printf("Unable to open file for reading\n");
        exit(-1);
    }

    for (i = 1; i < argc - 1; ++i) {
        if ((fileDescriptors[i] = open(argv[i + 1], O_WRONLY | O_CREAT, 0666)) == -1) {
            printf("Unable to open file for writing\n");
            exit(-1);
        }
    }

    while ((bytesRead = read(fileDescriptors[0], buffer, bufferSize)) == bufferSize) {
        for (i = 1; i < argc - 1; ++i) {
            write(fileDescriptors[i], buffer, bufferSize);
        }
    }

    for (i = 1; i < argc - 1; ++i) {
        write(fileDescriptors[i], buffer, bytesRead);
    }

    for (i = 0; i < argc - 1; ++i) {
        close(fileDescriptors[i]);
    }

    free(buffer);
    return 0;
}