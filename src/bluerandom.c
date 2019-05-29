//Blue Random: attempt on generating random numbers from bluetooth rssi nearby devices
//includes and globals at:
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
//running example: sudo ./bluerandom
int main(int argc, char **argv) {
    int retDescriptor1, retDescriptor2, retBLEScan, retScan, status;
    int deviceAdaptor1, deviceAdaptor2;

    //to test
    //setbuf(stdout, NULL);

    //////////open dev through HCI 
    retDescriptor1 = openDev(&deviceAdaptor1);
    
    //////////BLE SCAN    
    setBLEinquiryScanParameters(retDescriptor1);

    retBLEScan = BLEinquiryScan(retDescriptor1);
    if (retBLEScan != 0)
        printf("Error at BLE Scan \n");
        exit(-8);
    }
    
    //close            
    close(deviceAdaptor1); 


    return 0;
}

