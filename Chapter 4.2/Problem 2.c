#include <stdio.h>
#include <stdlib.h>

#include <linux/module.h>
#include <linux/init.h>

#include "Problem 2.h"

char* firstMemory;
char* secondMemory;
char* thirdMemory;

int firstRead, secondRead, thirdRead;
int firstWrite, secondWrite, thirdWrite;

static int driverInit(void);
static void driverCleanup(void);

module_init(driverInit);
module_exit(driverCleanup);

static int driverInit(void)
{
    firstMemory = (char*)malloc(512 * sizeof(char));
    secondMemory = (char*)malloc(512 * sizeof(char));
    thirdMemory = (char*)malloc(512 * sizeof(char));

    firstRead = secondRead = thirdRead = 0;
    firstWrite = secondWrite = thirdWrite = 0;

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