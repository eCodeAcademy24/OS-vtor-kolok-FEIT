#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    pid_t child;
    int descriptor, i;
    int* memory;

    child = fork();

    if (child == 0) {
        execlp("./Problem 3.1", "./Problem 3.1", NULL);
        printf("Invalid function call, program cannot be executed\n");
    }
    else {
        waitpid(child, NULL, WAIT_MYPGRP);

        if ((descriptor = open("Memory.txt", O_RDWR)) == -1) {
            printf("Unable to open file for mapping\n");
            exit(-1);
        }

        memory = mmap(NULL, 1000 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);

        if (memory == MAP_FAILED) {
            printf("Unable to create mapped memory\n");
            exit(-1);
        }

        for (i = 0; i < 1000; ++i) {
            printf("%d\t", memory[i]);
        }

        printf("\n");
        munmap(memory, 1000 * sizeof(int));
    }

    return 0;
}