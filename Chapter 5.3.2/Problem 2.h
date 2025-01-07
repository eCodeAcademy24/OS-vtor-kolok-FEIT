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
    int size = strlen(memory);

    if (lockRead == 0) {
        lockRead = 1;

        if (count <= size - read) {
            copy_to_user(buffer, memory + read, count);
            read += count;
            return count;
        }
        else {
            int amount = size - read;

            copy_to_user(buffer, memory + read, amount);
            read = 0;
            return amount;
        }
    }
    else {
        printk("<1>Driver is currently locked. You cannot read from it!\n");
        return 0;
    }
}

ssize_t driverWrite(struct file* file, const char* buffer, size_t count, loff_t* offset)
{
    if (strcmp(buffer, "Unlock Write") == 0) {
        lockWrite = 0;
        return 0;
    }
    else if (strcmp(buffer, "Unlock Read") == 0) {
        lockRead = 0;
        return 0;
    }

    if (lockWrite == 0) {
        lockWrite = 1;

        if (write + count < 1024) {
            copy_from_user(memory + write, buffer, count);
            write += count;
            return count;
        }
        else {
            int amount = 1024 - write;

            copy_from_user(memory + write, buffer, amount);
            write = 0;
            return amount;
        }
    }
    else {
        printk("<1>Driver is currently locked. You cannot write to it!\n");
        return 0;
    }
}

#endif // DRIVER_H