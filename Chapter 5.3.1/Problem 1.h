#ifndef DRIVER_H
#define DRIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/sched.h>

int driverOpen(struct inode* inode, struct file* file);
int driverRelease(struct inode* inode, struct file* file);
ssize_t driverRead(struct file* file, char* buffer, size_t count, loff_t* offset);
ssize_t driverWrite(struct file* file, const char* buffer, size_t count, loff_t* offset);

struct file_operations driverOperations = {
    open: driverOpen,
    read: driverRead,
    write: driverWrite,
    release: driverRelease,
};

int driverOpen(struct inode* inode, struct file* file)
{
    printk("<6>Opening the driver\n");
    return 0;
}

int driverRelease(struct inode* inode, struct file* file)
{
    printk("<6>Releasing the driver\n");
    return 0;
}

ssize_t driverRead(struct file* file, char* buffer, size_t count, loff_t* offset)
{
    int size = strlen(memory);
    srand(time(NULL));

    if (flag == 0) {
        flag = -1;

        if (count <= size) {
            copy_to_user(buffer, memory, count);
            return count;
        }
        else {
            copy_to_user(buffer, memory, size);
            return size;
        }
    }
    else if (flag == 1) {
        flag = -1;

        if (count <= size) {
            copy_to_user(buffer, memory + size - count, count);
            return count;
        }
        else {
            copy_to_user(buffer, memory, size);
            return size;
        }
    }
    else {
        int read = rand() % size;
        flag = -1;

        if (count <= size - read) {
            copy_to_user(buffer, memory + read, count);
            return count;
        }
        else {
            copy_to_user(buffer, memory + read, size);
            return size;
        }
    }
}

ssize_t driverWrite(struct file* file, const char* buffer, size_t count, loff_t* offset)
{
    if (strcmp(buffer, "0") == 0) {
        flag = 0;
        return 0;
    }
    else if (strcmp(buffer, "1") == 0) {
        flag = 1;
        return 0;
    }

    if (write + count < 10240) {
        copy_from_user(memory + write, buffer, count);
        write += count;
        return count;
    }
    else {
        int amount = 10240 - write;

        copy_from_user(memory + write, buffer, amount);
        write = 0;
        return amount;
    }
}

#endif // DRIVER_H