//Bluerandom: attempt on generating random numbers from bluetooth rssi nearby devices
//Author: Alexandre Augusto Giron

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
*/

//includes at:
#include "bluerandom.h"

//Make a connection to local adapter.
int openDev(int *device) {
    int ret;

    *device = hci_get_route(NULL);
    ret = hci_open_dev(*device);

    if (ret < 0) {
        printf("Error on opening HCI device. ");
        printf("%s\n", strerror(errno));
        exit(-1);
    }
    return ret;
}

//Main function 
//compilation: make     (gcc ... -lbluetooth -lm)
//running example: sudo ./bluerandom
int main() {
    int retDescriptor1, retBLEScan;
    int deviceAdaptor1;

    //to test
    setbuf(stdout, NULL);

    //////////open dev through HCI 
    retDescriptor1 = openDev(&deviceAdaptor1);
    
    //////////BLE SCAN    
    setBLEinquiryScanParameters(retDescriptor1);

    retBLEScan = BLEinquiryScan(retDescriptor1);

    //ending
    if (retBLEScan != 0) {
        printf("Error at BLE Scan \n");
        exit(retBLEScan);
    }
    
    //close            
    hci_close_dev(retDescriptor1);
    close(deviceAdaptor1); 


    return 0;
}

