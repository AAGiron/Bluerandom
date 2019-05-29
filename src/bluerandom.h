#ifndef _H_BLUERANDOM
#define _H_BLUERANDOM

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <math.h>  

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>


#include "BLEScan.h"
#include "scan.h"
#include "extraction.h"



//functions
int openDev(int *device);

//struct: device list
struct deviceAddr {
    int id;
    char addr[18];
};

#endif


