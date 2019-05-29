#ifndef _H_BLESCAN
#define _H_BLESCAN

//Functions
void setBLEinquiryScanParameters(int descriptor);
int BLEinquiryScan(int descriptor);

///request HCI, needed for BLEinquiryScanParameters.
struct hci_request ble_hci_request(uint16_t ocf, int clen, void * status, void * cparam);

//wrappers
int extractBitFromRSSI(unsigned char rssiOld, unsigned char rssi, int method);

#endif


