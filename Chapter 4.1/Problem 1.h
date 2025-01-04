#ifndef DRIVER_H
#define DRIVER_H

#include <stdio.h>
#include <stdlib.h>
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
    if (read + size <= 4096) {
        copy_to_user(buffer, memory + read, size);
        read += size;
        return size;
    }
    else {
        int amount = 4096 - read;

        copy_to_user(buffer, memory + read, amount);
        read = 0;
        return amount;
    }
}

ssize_t driverWrite(struct file* file, const char* buffer, size_t count, loff_t* offset)
{
    char* writeBuffer = (char*)malloc(count * sizeof(char));
    int writeSize = count;

    if (writeSize > 4096) {
        writeSize = 4096;
    }
    else if (writeSize < 64) {
        writeSize = 64;
    }

    if (writeSize % 64 > 0) {
        size = (writeSize / 64 + 1) * 64;
    }
    else {
        size = writeSize;
    }

    copy_from_user(writeBuffer, buffer, count);

    if (write + count < 4096) {
        strcpy(memory + write, writeBuffer);
        write += count;
        return count;
    }
    else {
        int amount = 4096 - write;

        strncpy(memory + write, writeBuffer, amount);
        write = 0;
        return amount;
    }
}

#endif // DRIVER_H