#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char* argv[])
{
    key_t key;
    int type, memoryID, seconds;
    int cutoff = 0;
    char* memory;
    char input[1000];

    if (argc < 3) {
        printf("Insufficient number of argument sent to the command line\n");
        exit(-1);
    }

    srand(time(NULL));

    type = atoi(argv[1]);
    key = (key_t)atoi(argv[2]);

    if ((memoryID = shmget(key, 1024, 0666 | IPC_CREAT)) < 0) {
        printf("Unable to create shared memory\n");
        exit(-1);
    }

    memory = (char*)shmat(memoryID, NULL, 0);

    while (1) {
        if (type == 1) {
            printf("Please write something into memory\n");
            scanf("%s", &input);
            strcpy(memory, input);

            seconds = rand() % 10 + 1;
            sleep(seconds);
        }
        else if (type == 2) {
            strcpy(memory, "");
            sleep(5);
        }
        else if (type == 3) {
            if (strlen(memory) != 0) {
                printf("%s\n", memory);
                cutoff = 0;
            }
            else {
                ++cutoff;

                if (cutoff >= 3) {
                    break;
                }
            }

            seconds = rand() % 10 + 1;
            sleep(seconds);
        }
    }

    shmdt(memory);
    shmctl(memoryID, IPC_RMID, NULL);
    return 0;
}