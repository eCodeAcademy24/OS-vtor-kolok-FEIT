#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char* argv[])
{
    pid_t child;
    key_t sharedKey = 12345;
    key_t synchronizedKey = 54321;
    int number, lower, higher;
    int sharedMemoryID, synchronizedMemoryID;
    char* sharedMemory;
    char* synchronizedMemory;
    char input[100];

    if ((sharedMemoryID = shmget(sharedKey, 1024, 0666 | IPC_CREAT)) < 0) {
        printf("Unable to create shared memory\n");
        exit(-1);
    }

    if ((synchronizedMemoryID = shmget(synchronizedKey, 1024, 0666 | IPC_CREAT)) < 0) {
        printf("Unable to create shared memory\n");
        exit(-1);
    }

    sharedMemory = (char*)shmat(sharedMemoryID, NULL, 0);
    synchronizedMemory = (char*)shmat(synchronizedMemoryID, NULL, 0);

    strcpy(synchronizedMemory, "Parent Busy");
    child = fork();

    if (child == 0) {
        while (1) {
            if ((sharedMemoryID = shmget(sharedKey, 1024, 0666 | IPC_CREAT)) < 0) {
                printf("Unable to create shared memory\n");
                exit(-1);
            }

            if ((synchronizedMemoryID = shmget(synchronizedKey, 1024, 0666 | IPC_CREAT)) < 0) {
                printf("Unable to create shared memory\n");
                exit(-1);
            }

            sharedMemory = (char*)shmat(sharedMemoryID, NULL, 0);
            synchronizedMemory = (char*)shmat(synchronizedMemoryID, NULL, 0);

            while (strcmp(synchronizedMemory, "Parent Busy") == 0) {
                sleep(1);
            }

            number = atoi(sharedMemory);
            lower = 1;

            while (lower * 2 <= number) {
                lower *= 2;
            }

            higher = lower * 2;

            if ((higher - number) < (number - lower)) {
                sprintf(sharedMemory, "%d", higher);
            }
            else {
                sprintf(sharedMemory, "%d", lower);
            }

            strcpy(synchronizedMemory, "Parent Busy");
        }
    }
    else {
        while (1) {
            printf("Please enter a number\n");
            scanf("%s", &input);

            if (atoi(input) >= 0) {
                strcpy(sharedMemory, input);
                strcpy(synchronizedMemory, "Child Busy");

                while (strcmp(synchronizedMemory, "Child Busy") == 0) {
                    sleep(1);
                }

                number = atoi(sharedMemory);
                printf("Previous number has been rounded to %d\n", number);
            }
            else {
                shmdt(sharedMemory);
                shmdt(synchronizedMemory);

                shmctl(sharedMemoryID, IPC_RMID, NULL);
                shmctl(synchronizedMemoryID, IPC_RMID, NULL);

                exit(0);
            }
        }
    }

    shmdt(sharedMemory);
    shmdt(synchronizedMemory);

    return 0;
}