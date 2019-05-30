//Blue Random: attempt on generating random numbers from bluetooth rssi nearby devices
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

