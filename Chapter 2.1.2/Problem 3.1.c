#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    int descriptor, i, j;
    int* memory;

    if ((descriptor = open("Memory.txt", O_RDWR | O_CREAT, 0666)) == -1) {
        printf("Unable to create file for mapping\n");
        exit(-1);
    }

    if (ftruncate(descriptor, 1000 * sizeof(int)) == -1) {
        printf("Unable to truncate memory to appropriate size\n");
        exit(-1);
    }

    memory = mmap(NULL, 1000 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);

    if (memory == MAP_FAILED) {
        printf("Unable to create mapped memory\n");
        exit(-1);
    }

    for (i = 2, j = 0; i <= 2000; i += 2, ++j) {
        memory[j] = i;
    }

    munmap(memory, 1000 * sizeof(int));
    return 0;
}