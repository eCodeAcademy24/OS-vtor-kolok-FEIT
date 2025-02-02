#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char* argv[])
{
    int memoryID;
    int key = 12345;
    char* memory;

    if ((memoryID = shmget(key, 1024, 0666 | IPC_CREAT)) < 0) {
        printf("Unable to create shared memory\n");
        exit(-1);
    }

    memory = (char*)shmat(memoryID, NULL, 0);
    strcpy(memory, "Hi, this is a test for Shared Memory\n");

    shmdt(memory);
    return 0;
}