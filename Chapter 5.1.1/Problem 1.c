#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define rows 1920
#define columns 1080

int main(int argc, char* argv[])
{
    pid_t children[10];
    int i, j, k, N, offset, limit, pixel, fileDescriptor;
    int* image;

    if (argc < 2) {
        printf("Insufficient amount of arguments sent to the command line\n");
        exit(-1);
    }

    N = atoi(argv[1]);
    offset = rows / N;

    srand(time(NULL));

    if ((fileDescriptor = open("Image.txt", O_RDWR | O_CREAT, 0666)) == -1) {
        printf("Unable to create file for mapping\n");
        exit(-1);
    }

    if ((ftruncate(fileDescriptor, rows * columns * sizeof(int))) == -1) {
        printf("Unable to truncate memory to appropriate size\n");
        exit(-1);
    }

    image = (int*)mmap(NULL, rows * columns * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0);

    if (image == MAP_FAILED) {
        printf("Unable to create mapped memory\n");
        exit(-1);
    }

    for (i = 0; i < rows * columns; ++i) {
        image[i] = rand() % 256;
    }

    for (i = 0; i < N; ++i) {
        children[i] = fork();

        if (children[i] == 0) {
            break;
        }
    }

    if (i < N) {
        limit = (i + 1) * offset;

        if (i == N - 1 && limit < rows) {
            limit = rows;
        }

        for (j = i * offset; j < limit; ++j) {
            for (k = 0; k < columns; ++k) {
                pixel = image[j * columns + k];

                if (j == 0) {
                    if (k == 0) {
                        if (pixel / 2 < image[j * columns + k + 1] && pixel / 2 < image[(j + 1) * columns + k]) {
                            printf("Process %d found a matching pixel at coordinates %d and %d, with a value of %d\n", i, j, k, pixel);
                        }
                    }
                    else if (k == columns - 1) {
                        if (pixel / 2 < image[j * columns + k - 1] && pixel / 2 < image[(j + 1) * columns + k]) {
                            printf("Process %d found a matching pixel at coordinates %d and %d, with a value of %d\n", i, j, k, pixel);
                        }
                    }
                    else {
                        if (pixel / 2 < image[j * columns + k - 1] && pixel / 2 < image[j * columns + k + 1] && pixel / 2 < image[(j + 1) * columns + k]) {
                            printf("Process %d found a matching pixel at coordinates %d and %d, with a value of %d\n", i, j, k, pixel);
                        }
                    }
                }
                else if (j == rows - 1) {
                    if (k == 0) {
                        if (pixel / 2 < image[j * columns + k + 1] && pixel / 2 < image[(j - 1) * columns + k]) {
                            printf("Process %d found a matching pixel at coordinates %d and %d, with a value of %d\n", i, j, k, pixel);
                        }
                    }
                    else if (k == columns - 1) {
                        if (pixel / 2 < image[j * columns + k - 1] && pixel / 2 < image[(j - 1) * columns + k]) {
                            printf("Process %d found a matching pixel at coordinates %d and %d, with a value of %d\n", i, j, k, pixel);
                        }
                    }
                    else {
                        if (pixel / 2 < image[j * columns + k - 1] && pixel / 2 < image[j * columns + k + 1] && pixel / 2 < image[(j - 1) * columns + k]) {
                            printf("Process %d found a matching pixel at coordinates %d and %d, with a value of %d\n", i, j, k, pixel);
                        }
                    }
                }
                else {
                    if (k == 0) {
                        if (pixel / 2 < image[j * columns + k + 1] && pixel / 2 < image[(j - 1) * columns + k] && pixel / 2 < image[(j + 1) * columns + k]) {
                            printf("Process %d found a matching pixel at coordinates %d and %d, with a value of %d\n", i, j, k, pixel);
                        }
                    }
                    else if (k == columns - 1) {
                        if (pixel / 2 < image[j * columns + k - 1] && pixel / 2 < image[(j - 1) * columns + k] && pixel / 2 < image[(j + 1) * columns + k]) {
                            printf("Process %d found a matching pixel at coordinates %d and %d, with a value of %d\n", i, j, k, pixel);
                        }
                    }
                    else {
                        if (pixel / 2 < image[j * columns + k - 1] && pixel / 2 < image[j * columns + k + 1] &&
                            pixel / 2 < image[(j - 1) * columns + k] && pixel / 2 < image[(j + 1) * columns + k]) {
                            printf("Process %d found a matching pixel at coordinates %d and %d, with a value of %d\n", i, j, k, pixel);
                        }
                    }
                }
            }
        }
    }
    else {
        for (i = 0; i < N; ++i) {
            waitpid(children[i], NULL, WAIT_MYPGRP);
        }

        printf("All processes have finished\n");
    }

    return 0;
}