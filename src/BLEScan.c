#include <fcntl.h>

//includes ang globals at
#include "bluerandom.h"

//Functions implemented here:
//hci_request (from Glock45 github)
//setBLEinquiryScanParameters()
//BLEinquiryScan()
//inquiryScanNearbyDevices()

struct hci_request ble_hci_request(uint16_t ocf, int clen, void * status, void * cparam) {
    struct hci_request rq;
    memset(&rq, 0, sizeof (rq));
    rq.ogf = OGF_LE_CTL;
    rq.ocf = ocf;
    rq.cparam = cparam;
    rq.clen = clen;
    rq.rparam = status;
    rq.rlen = 1;
    return rq;
}


//Set BLE parameters. 
void setBLEinquiryScanParameters(int descriptor) {
    int status, retParam;
    le_set_scan_parameters_cp scan_params_cp;
    memset(&scan_params_cp, 0, sizeof (scan_params_cp));
    scan_params_cp.type = 0x00;
    scan_params_cp.interval = htobs(0x0010);
    scan_params_cp.window = htobs(0x0010);
    scan_params_cp.own_bdaddr_type = 0x00; // Public Device Address (default).
    scan_params_cp.filter = 0x00; // Accept all. Maybe here we can filter EVT_EXTENDED_INQUIRY or INQUIRY_WITH_RSSI

    struct hci_request scan_params_rq = ble_hci_request(OCF_LE_SET_SCAN_PARAMETERS, LE_SET_SCAN_PARAMETERS_CP_SIZE, &status, &scan_params_cp);

    retParam = hci_send_req(descriptor, &scan_params_rq, 1000);
    if (retParam < 0) {
        hci_close_dev(descriptor);
        perror("Failed to set scan parameters data.");
        exit(-2);
    }
    // Enable scanning.

    le_set_scan_enable_cp scan_cp;
    memset(&scan_cp, 0, sizeof (scan_cp));
    scan_cp.enable = 0x01; // Enable flag.
    scan_cp.filter_dup = 0x00; // Filtering disabled.

    struct hci_request enable_adv_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);

    retParam = hci_send_req(descriptor, &enable_adv_rq, 1000);
    if (retParam < 0) {
        hci_close_dev(descriptor);
        perror("Failed to enable scan.");
        exit(-3);
    }
    //test: Non blocking socket
    int fl;
    fl = fcntl(descriptor, F_GETFL);
    if (fcntl(descriptor, F_SETFL, fl | O_NONBLOCK) < 0) {
        printf("Error on Non blocking option\n");
        exit(-4);
    }//else
    //	printf("OK!\n");


}

//Wrapper for extraction methods
// 0 Odd or Even Difference
// 1 Early von Neumann
// 2 Just Odd or Even
int extractBitFromRSSI(unsigned char rssiOld, unsigned char rssi, int method) {    
    if (method == 0)
        return diferencaParOuImpar(rssi, rssiOld);
    else if (method == 1)
        return vonNeumannExtraction(rssi, rssiOld);
    else if (method == 2)
        return oddOrEvenExtraction(rssi);
    else
        return -1; //Hã?
}

//SCAN BLE
int BLEinquiryScan(int descriptor) {
    int status, ret, retBit = 0;
    le_set_scan_enable_cp scan_cp;

    struct hci_filter nf;
    hci_filter_clear(&nf);
    hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
    hci_filter_set_event(EVT_LE_META_EVENT, &nf);

    if (setsockopt(descriptor, SOL_HCI, HCI_FILTER, &nf, sizeof (nf)) < 0) {
        hci_close_dev(descriptor);
        perror("Could not set socket options");
        return -5;
    }

    uint8_t buf[HCI_MAX_EVENT_SIZE];
    evt_le_meta_event * meta_event;
    le_advertising_info * info;
    int len;

    unsigned char rssiOld = 0, rssiRecntly, random;
    int contBits = 0; //counter 0 to 8 in order to know when a byte is produced

    while (1) {
        len = read(descriptor, buf, sizeof (buf));
        if (len >= HCI_EVENT_HDR_SIZE) {
            meta_event = (evt_le_meta_event*) (buf + HCI_EVENT_HDR_SIZE + 1);
            if (meta_event->subevent == EVT_LE_ADVERTISING_REPORT) {
                uint8_t reports_count = meta_event->data[0];
                void * offset = meta_event->data + 1;
                while (reports_count--) {

                    info = (le_advertising_info *) offset;

                    //Read RSSI
                    if (rssiOld == 0) {
                        rssiOld = (unsigned char) info->data[info->length]; 
                    } else {
                        //extraction function from extraction.c
                        rssiRecntly = (unsigned char) info->data[info->length];
                        
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

                        rssiOld = 0; //readings extracted two at a time

                        if (contBits >= 8) { 
                            printf("%c", random);                   //OUTPUT BYTE
                            random = 0;
                            contBits = 0;                            
                        }
                    }

                    offset = info->data + info->length + 2;
                }
            }
        }
    }

    // Disable scanning
    memset(&scan_cp, 0, sizeof (scan_cp));
    scan_cp.enable = 0x00; // Disable flag

    struct hci_request disable_adv_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);
    ret = hci_send_req(descriptor, &disable_adv_rq, 1000);
    if (ret < 0) {
        hci_close_dev(descriptor);
        perror("Failed to disable scan.");
        return -6;
    }


    return 0;
}

