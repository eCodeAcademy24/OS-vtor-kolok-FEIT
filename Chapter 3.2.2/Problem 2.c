#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    pid_t secondProcess, thirdProcess;
    int i;
    int counter = 0;
    int firstPipe[2];
    int secondPipe[2];
    char* input;

    input = (char*)malloc(100 * sizeof(char));

    if (pipe(firstPipe) == -1) {
        printf("Unable to create pipe between the processes\n");
        exit(-1);
    }

    printf("Please enter some text\n");
    scanf("%s", input);

    secondProcess = fork();

    if (secondProcess == 0) {
        close(firstPipe[1]);

        if (dup2(firstPipe[0], 0) == -1) {
            printf("Unable to redirect standard input\n");
            exit(-1);
        }

        close(firstPipe[0]);

        if (pipe(secondPipe) == -1) {
            printf("Unable to redirect standard input\n");
            exit(-1);
        }

        scanf("%s", input);
        thirdProcess = fork();

        if (thirdProcess == 0) {
            close(secondPipe[1]);

            if (dup2(secondPipe[0], 0) == -1) {
                printf("Unable to redirect standard input\n");
                exit(-1);
            }

            close(secondPipe[0]);
            scanf("%s", input);

            for (i = 0; i < strlen(input); ++i) {
                if (isdigit(input[i]) > 0) {
                    ++counter;
                }
            }

            printf("There are %d digits in the string\n", counter);
        }
        else {
            close(secondPipe[0]);

            if (dup2(secondPipe[1], 1) == -1) {
                printf("Unable to redirect standard output\n");
                exit(-1);
            }

            close(secondPipe[1]);
            printf("%s", input);
        }
    }
    else {
        close(firstPipe[0]);

        if (dup2(firstPipe[1], 1) == -1) {
            printf("Unable to redirect standard output\n");
            exit(-1);
        }

        close(firstPipe[1]);
        printf("%s", input);
    }

    free(input);
    return 0;
}