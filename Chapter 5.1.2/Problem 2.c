#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char* argv[])
{
    srand(time(NULL));

    pid_t children[2];
    key_t sharedKey, synchronizedKey;
    int i, j, counter, sharedMemoryID, synchronizedMemoryID;
    char* sharedMemory;
    char* synchronizedMemory;
    char* input;

    sharedKey = (key_t)(rand() % 10000 + 1);
    synchronizedKey = (key_t)(rand() % 10000 + sharedKey);

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

    input = (char*)malloc(200 * sizeof(char));
    strcpy(synchronizedMemory, "Parent Busy");

    for (i = 0; i < 2; ++i) {
        children[i] = fork();

        if (children[i] == 0) {
            break;
        }
    }

    if (i < 2) {
        if (i == 0) {
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

                while (strcmp(synchronizedMemory, "Parent Done") != 0) {
                    sleep(1);
                }

                counter = 0;
                strcpy(synchronizedMemory, "First Child Busy");

                for (j = 0; j < strlen(sharedMemory); ++j) {
                    if (isalpha(sharedMemory[j])) {
                        ++counter;
                    }
                }

                printf("The first child process found %d letters in the text\n", counter);
                strcpy(synchronizedMemory, "First Child Done");
            }
        }
        else {
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

                while (strcmp(synchronizedMemory, "First Child Done") != 0) {
                    sleep(1);
                }

                counter = 0;
                strcpy(synchronizedMemory, "Second Child Busy");

                for (j = 0; j < strlen(sharedMemory); ++j) {
                    if (isdigit(sharedMemory[j])) {
                        ++counter;
                    }
                }

                printf("The second process found %d digits in the text\n", counter);
                strcpy(synchronizedMemory, "All Children Done");
            }
        }
    }
    else {
        while (1) {
            printf("Please enter some text\n");
            fgets(input, 200, stdin);

            if (strncmp(input, "Quit", 4) != 0) {
                strcpy(sharedMemory, input);
                strcpy(synchronizedMemory, "Parent Done");

                while (strcmp(synchronizedMemory, "All Children Done") != 0) {
                    sleep(1);
                }

                strcpy(synchronizedMemory, "Parent Busy");
            }
            else {
                shmdt(sharedMemory);
                shmdt(synchronizedMemory);

                shmctl(sharedMemoryID, IPC_RMID, NULL);
                shmctl(synchronizedMemoryID, IPC_RMID, NULL);

                free(input);
                exit(0);
            }
        }
    }

    shmdt(sharedMemory);
    shmdt(synchronizedMemory);

    return 0;
}