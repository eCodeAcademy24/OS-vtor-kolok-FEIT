#include <stdio.h>
#include <stdlib.h>

#include <linux/module.h>
#include <linux/init.h>

#include "Problem 3.h"

char* memory;
int read, write, writeAvailable;

static int driverInit(void);
static void driverCleanup(void);

module_init(driverInit);
module_exit(driverCleanup);

static int driverInit(void)
{
    memory = (char*)malloc(512 * sizeof(char));
    read = 0;
    write = 0;
    writeAvailable = 1;

    if (register_chrdev(60, "Driver", &driverOperations) == 0) {
        printk("<3>Unable to register driver\n");
    }

    return 0;
}

static void driverCleanup(void)
{
    printk("<6>Destroying driver\n");
    unregister_chrdev(60, "Driver");
    return;
}