#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    pid_t children[10];
    key_t key = 12345;
    int i, N, memoryID, fileDescriptor;
    int counter = 0;
    char* sharedMemory;
    char* mappedMemory;
    char command[100];

    if (argc < 2) {
        printf("Insufficient number of arguments sent to the command line\n");
        exit(-1);
    }

    N = atoi(agrv[1]);

    if ((memoryID = shmget(key, 1024, 0666 | IPC_CREAT)) < 0) {
        printf("Unable to create shared memory\n");
        exit(-1);
    }

    sharedMemory = (char*)shmat(memoryID, NULL, 0);
    sprintf(sharedMemory, "%d", N);

    if ((fileDescriptor = open("Memory.txt", O_RDWR | O_CREAT, 0666)) == -1) {
        printf("Unable to create file for mapping\n");
        exit(-1);
    }

    if (ftruncate(fileDescriptor, 100 * sizeof(char)) == -1) {
        printf("Unable to truncate memory to appropriate size\n");
        exit(-1);
    }

    mappedMemory = (char*)mmap(NULL, 100 * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0);

    if (mappedMemory == MAP_FAILED) {
        printf("Unable to create mapped memory\n");
        exit(-1);
    }

    for (i = 0; i < N; ++i) {
        children[i] = fork();

        if (children[i] == 0) {
            break;
        }
    }

    if (i < N) {
        if ((memoryID = shmget(key, 1024, 0666 | IPC_CREAT)) < 0) {
            printf("Unable to create shared memory\n");
            exit(-1);
        }

        sharedMemory = (char*)shmat(memoryID, NULL, 0);

        while (atoi(sharedMemory) != i) {
            sleep(1);
        }

        execlp(mappedMemory, mappedMemory, NULL);
        printf("Invalid function call, program cannot be executed\n");
    }
    else {
        while (counter < N) {
            printf("Please enter the command for process %d\n", counter);
            scanf("%s", &command);

            strcpy(mappedMemory, command);
            sprintf(sharedMemory, "%d", counter);

            waitpid(children[counter], NULL, WAIT_MYPGRP);
            ++counter;
        }

        printf("All processes have finished\n");

        shmdt(sharedMemory);
        shmctl(memoryID, IPC_RMID, NULL);
        munmap(mappedMemory, 100 * sizeof(char));
    }

    return 0;
}