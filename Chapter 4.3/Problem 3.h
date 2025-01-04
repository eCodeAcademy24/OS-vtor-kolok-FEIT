#ifndef DRIVER_H
#define DRIVER_H

#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/fs.>
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
    printk("<6>Release the driver\n");
    return 0;
}

ssize_t driverRead(struct file* file, char* buffer, size_t count, loff_t* offset)
{
    if (writeAvailable == 0) {
        if (512 - read > count) {
            copy_to_user(buffer, memory + 512 - read - count, count);
            read += count;
            return count;
        }
        else {
            int amount = 512 - read;

            copy_to_user(buffer, memory, amount);
            read = 512;
            write = 0;
            writeAvailable = 1;

            return amount;
        }
    }
    else {
        return 0;
    }
}

ssize_t driverWrite(struct file* file, const char* buffer, size_t count, loff_t* offset)
{
    if (writeAvailable == 1) {
        if (512 - write > count) {
            copy_from_user(memory + write, buffer, count);
            write += count;
            return count;
        }
        else {
            int amount = 512 - write;

            copy_from_user(memory + write, buffer, amount);
            write = 512;
            read = 0;
            writeAvailable = 0;

            return amount;
        }
    }
    else {
        return 0;
    }
}

#endif // DRIVER_H