#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    pid_t secondProcess, thirdProcess;
    int firstPipe[2];
    int secondPipe[2];
    int number, average;
    int counter = 0;
    int sum = 0;
    int difference = 0;
    char* input;

    input = (char*)malloc(20 * sizeof(char));

    if (pipe(firstPipe) == -1) {
        printf("Unable to create pipe between the processes\n");
        exit(-1);
    }

    printf("Please enter a number\n");
    scanf("%d", &number);

    secondProcess = fork();

    if (secondProcess == 0) {
        close(firstPipe[1]);

        if (pipe(secondPipe) == -1) {
            printf("Unable to create pipe between the processes\n");
            exit(-1);
        }

        thirdProcess = fork();

        if (thirdProcess == 0) {
            close(secondPipe[1]);

            while (1) {
                read(secondPipe[0], input, 20);
                number = atoi(input);

                if (difference == 0) {
                    printf("The difference between the last two averages is %d\n", number);
                    difference = number;
                }
                else {
                    difference = number - difference;
                    printf("The difference between the last two averages is %d\n", difference);
                }
            }
        }
        else {
            close(secondPipe[0]);

            while (1) {
                read(firstPipe[0], input, 20);
                number = atoi(input);

                ++counter;
                sum += number;
                average = sum / counter;

                sprintf(input, "%d", average);
                write(secondPipe[1], input, strlen(input));
            }
        }
    }
    else {
        close(firstPipe[0]);

        sprintf(input, "%d", number);
        write(firstPipe[1], input, strlen(input));

        while (1) {
            printf("Please enter a number\n");
            scanf("%d", &number);

            sprintf(input, "%d", number);
            write(firstPipe[1], input, strlen(input));
        }
    }

    return 0;
}