#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    pid_t child;
    int i, vowels, fileSharedMemory, fileSynchronizeMemory;
    char* sharedMemory;
    char* synchronizeMemory;
    char word[100];

    if ((fileSharedMemory = open("Shared Memory.txt", O_RDWR | O_CREAT, 0666)) == -1) {
        printf("Unable to create file for mapping\n");
        exit(-1);
    }

    if ((fileSynchronizeMemory = open("Synchronize Memory.txt", O_RDWR | O_CREAT, 0666)) == -1) {
        printf("Unable to create file for mapping\n");
        exit(-1);
    }

    if (ftruncate(fileSharedMemory, 100 * sizeof(char)) == -1) {
        printf("Unable to truncate memory to appropriate size\n");
        exit(-1);
    }

    if (ftruncate(fileSynchronizeMemory, 100 * sizeof(char)) == -1) {
        printf("Unable to truncate memory to appropriate size\n");
        exit(-1);
    }

    sharedMemory = (char*)mmap(NULL, 100 * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, fileSharedMemory, 0);
    synchronizeMemory = (char*)mmap(NULL, 100 * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, fileSynchronizeMemory, 0);

    if (sharedMemory == MAP_FAILED) {
        printf("Unable to create mapped memory\n");
        exit(-1);
    }

    if (synchronizeMemory == MAP_FAILED) {
        printf("Unable to create mapped memory\n");
        exit(-1);
    }

    strcpy(synchronizeMemory, "Parent Busy");
    child = fork();

    if (child == 0) {
        while (1) {
            while (strcmp(synchronizeMemory, "Parent Busy") == 0) {
                sleep(1);
            }

            if (strcmp(sharedMemory, "Quit") == 0) {
                exit(0);
            }

            vowels = 0;

            for (i = 0; i < strlen(sharedMemory), ++i) {
                if (sharedMemory[i] == 'A' || sharedMemory[i] == 'E' || sharedMemory[i] == 'I' || sharedMemory[i] == 'O' || sharedMemory[i] == 'U' ||
                    sharedMemory[i] == 'a' || sharedMemory[i] == 'e' || sharedMemory[i] == 'i' || sharedMemory[i] == 'o' || sharedMemory[i] == 'u') {
                    ++vowels;
                }
            }

            if (vowels == 1) {
                printf("There is %d vowel in the word %s\n", vowels, sharedMemory);
            }
            else {
                printf("There are %d vowels in the word: %s\n", vowels, sharedMemory);
            }

            strcpy(synchronizeMemory, "Parent Busy");
        }
    }
    else {
        while (1) {
            while (strcmp(synchronizeMemory, "Child Busy") == 0) {
                if (waitpid(child, NULL, WNOHANG) == child) {
                    printf("Child process finished\n");

                    munmap(sharedMemory, 100 * sizeof(char));
                    munmap(synchronizeMemory, 100 * sizeof(char));

                    exit(0);
                }

                sleep(1);
            }

            printf("Please enter a new word for coding\n");
            scanf("%s", &word);

            strcpy(sharedMemory, word);
            strcpy(synchronizeMemory, "Child Busy");
        }
    }

    return 0;
}