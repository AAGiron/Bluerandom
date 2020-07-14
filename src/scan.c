/*
 * Bluerandom scan for regular Bluetooth devices
 * (for BLE see BLEScan.c)
 * The scan mode is continously. 
 */

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

#include "bluerandom.h"
#include "scan.h"

//to test
//#include <errno.h>
//#include <libintl.h>

uint8_t buffer[HCI_MAX_FRAME_SIZE];

void setInquiryScanParameters(int socketDescriptor) {

    struct hci_filter flt;
    periodic_inquiry_cp periodic_p;
    periodic_p.num_rsp = 0x00;
    periodic_p.length = lengthInquiry;
    periodic_p.min_period = lengthInquiry + 1;
    periodic_p.max_period = lengthInquiry + 2;
    periodic_p.lap[0] = 0x33;
    periodic_p.lap[1] = 0x8b;
    periodic_p.lap[2] = 0x9e;

    hci_filter_clear(&flt);
    hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
    hci_filter_all_events(&flt);

    //set filter: all
    if (setsockopt(socketDescriptor, SOL_HCI, HCI_FILTER, &flt, sizeof (flt)) < 0) {
        printf("Cannot set filter");
        exit(-7);
    }
    
    //set Periodic Inquiry Mode
    if (hci_send_cmd(socketDescriptor, OGF_LINK_CTL, OCF_PERIODIC_INQUIRY, PERIODIC_INQUIRY_CP_SIZE, &periodic_p) < 0) {
        printf("Cannot request periodic inquiry!\n"); //sudo?
        exit(-8);
    }
    
}

int inquiryScan(int socketDescriptor) {
    int len;

    unsigned char rssiOld = 0, rssiRecntly, random;
    int contBits = 0; //counter 0 to 8 in order to know when a byte is produced
    int retBit = 0;

    while (1) {        
        len = read(socketDescriptor, buffer, sizeof (buffer));        

        //parse buffer                     
        if (len >= 3) {
                    
            //we want EVT_EXTENDED_INQUIRY_RESULT
            switch (buffer[1]) {
                case EVT_EXTENDED_INQUIRY_RESULT: //0x2F
                    
                    if (rssiOld == 0) {
                        rssiOld = (unsigned char) buffer[17];
                    } else {
                        //extraction function from extraction.c
                        rssiRecntly = (unsigned char) buffer[17];

                        retBit = extractBitFromRSSI(rssiOld, rssiRecntly, EXTRACTION_METHOD);

                        if (retBit == 0) {
                            random = (random << 1);
                            contBits++;
                        } else {
                            if (retBit == 1) {
                                random = (random << 1) + 1;
                                contBits++;
                            } 
                        }

                        rssiOld = 0; //readings taken two at a time

                        if (contBits >= 8) { 
                            printf("%c", random);                   //OUTPUT BYTE

                            random = 0;
                            contBits = 0;
                        }

                    }
                    
                    break; //end case EVT_EXTENDED_INQUIRY_RESULT 
            }

        }
                
    }

    
    if (hci_send_cmd(socketDescriptor, OGF_LINK_CTL, OCF_EXIT_PERIODIC_INQUIRY, 0, NULL) < 0) {
        printf("I cannot stop the inquisition!\n");
        return -9;
    }

    return 0;
}

