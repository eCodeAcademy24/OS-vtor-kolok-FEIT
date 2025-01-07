#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    pid_t firstProcess, secondProcess, thirdProcess;
    int firstPipe[2];
    int secondPipe[2];
    int thirdPipe[2];

    if (argc < 3) {
        printf("Insufficient amount of arguments sent to the command line\n");
        exit(-1);
    }

    if (pipe(firstPipe) == -1) {
        printf("Unable to create pipe between the processes\n");
        exit(-1);
    }

    firstProcess = fork();

    if (firstProcess > 0) {
        close(firstPipe[0]);

        if (dup2(firstPipe[1], 1) == -1) {
            printf("Unable to redirect standard output\n");
            exit(-1);
        }

        close(firstPipe[1]);

        execlp(argv[1], argv[1], NULL);
        printf("Error occurred while trying to execute function call\n");
    }
    else if (firstProcess == 0) {
        close(firstPipe[1]);

        if (dup2(firstPipe[0], 0) == -1) {
            printf("Unable to redirect standard input\n");
            exit(-1);
        }

        close(firstPipe[0]);

        if (pipe(secondPipe) == -1) {
            printf("Unable to create pipe between the processes\n");
            exit(-1);
        }

        secondProcess = fork();

        if (secondProcess > 0) {
            close(secondPipe[0]);

            if (dup2(secondPipe[1], 1) == -1) {
                printf("Unable to redirect standard output\n");
                exit(-1);
            }

            close(secondPipe[1]);

            execlp(argv[2], argv[2], NULL);
            printf("Error occurred while trying to execute function call\n");
        }
        else if (secondProcess == 0) {
            close(secondPipe[1]);

            if (dup2(secondPipe[0], 0) == -1) {
                printf("Unable to redirect standard input\n");
                exit(-1);
            }

            close(secondPipe[0]);

            if (pipe(thirdPipe) == -1) {
                printf("Unable to create pipe between the processes\n");
                exit(-1);
            }

            thirdProcess = fork();

            if (thirdProcess > 0) {
                close(thirdPipe[0]);

                if (dup2(thirdPipe[1], 1) == -1) {
                    printf("Unable to redirect standard output\n");
                    exit(-1);
                }

                close(thirdPipe[1]);

                execlp(argv[1], argv[1], NULL);
                printf("Error occurred while trying to execute function call\n");
            }
            else if (thirdProcess == 0) {
                close(thirdPipe[1]);

                if (dup2(thirdPipe[0], 0) == -1) {
                    printf("Unable to redirect standard input\n");
                    exit(-1);
                }

                close(thirdPipe[0]);

                execlp(argv[2], argv[2], NULL);
                printf("Error occurred while trying to execute function call\n");
            }
        }
    }

    return 0;
}