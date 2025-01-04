#ifndef DRIVER_H
#define DRIVER_H

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
    int memoryFlag = 1;

    if (memoryFlag == 1 && strlen(firstMemory) - firstRead < count) {
        memoryFlag = 2;
    }

    if (memoryFlag == 2 && strlen(secondMemory) - secondRead < count) {
        memoryFlag = 3;
    }

    if (memoryFlag == 3 && strlen(thirdMemory) - thirdRead < count) {
        printk("<1>Unable to read that many bytes from any memory. Starting from the beginning\n");

        memoryFlag = 1;
        firstRead = 0;
        secondRead = 0;
        thirdRead = 0;
    }

    if (memoryFlag == 1) {
        copy_to_user(buffer, firstMemory + firstRead, count);
        firstRead += count;
        return count;
    }
    else if (memoryFlag == 2) {
        copy_to_user(buffer, secondMemory + secondRead, count);
        secondRead += count;
        return count;
    }
    else if (memoryFlag == 3) {
        copy_to_user(buffer, thirdMemory + thirdRead, count);
        thirdRead += count;
        return count;
    }
    else {
        return 0;
    }
}

ssize_t driverWrite(struct file* file, const char* buffer, size_t count, loff_t* offset)
{
    int memoryFlag = 1;

    if (memoryFlag == 1 && 512 - firstWrite < count) {
        memoryFlag = 2;
    }

    if (memoryFlag == 2 && 512 - secondWrite < count) {
        memoryFlag = 3;
    }

    if (memoryFlag == 3 && 512 - thirdMemory < count) {
        printk("<1>Unable to write that many bytes to any memory. Starting from the beginning\n");

        memoryFlag = 1;
        firstWrite = 0;
        secondWrite = 0;
        thirdWrite = 0;
    }

    if (memoryFlag == 1) {
        copy_from_user(firstMemory + firstWrite, buffer, count);
        firstWrite += count;
        return count;
    }
    else if (memoryFlag == 2) {
        copy_from_user(secondMemory + secondWrite, buffer, count);
        secondWrite += count;
        return count;
    }
    else if (memoryFlag == 3) {
        copy_from_user(thirdMemory + thirdWrite, buffer, count);
        thirdWrite += count;
        return count;
    }
    else {
        return 0;
    }
}

#endif // DRIVER_H