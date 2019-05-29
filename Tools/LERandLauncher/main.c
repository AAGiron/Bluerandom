/*
 * Purpose: send a LE Rand command in order to test it against FIPS and others
 * Inspired by:
 *   hcitool.c static void cmd_cmd(int dev_id, int argc, char **argv) 
 *
 *  gcc -o lerand main.c -lbluetooth
 */

/* 
 * File:   main.c
 * Author: giron
 *
 * Created on 4 de Abril de 2019, 08:59
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

//here the print result is backwards (same as wireshark shows) - lerand below is little endian.
//see inside the sendLERandCommmand function comments

void printBytes(unsigned char *responseBuffer, int length) {
    int i; //it is seven because the response buffer is a packet (not only the number)
    for (i = 7; i < length; i++)
        printf("%c", responseBuffer[i]); // /dev/random print approach :-)
    /* printf("\n");
     for (i = 7; i < length ; i ++)
         printf("%hhx", responseBuffer[i]);*/
}


//(Should be) Same as:
// $ sudo hcitool cmd 0x08 0x0018 0x2018
//                 OGF  OCF   Opcode
//LE Rand           8   0x18  0x2018 

uint64_t sendLERandCommand(int devDescriptor) {
    uint8_t ogf;
    uint16_t ocf;
    uint64_t lerand = 0;

    //parameters
    int length = 2;
    unsigned char responseBuffer[HCI_MAX_EVENT_SIZE], *ptr = responseBuffer;

    ogf = 0x08;
    ocf = 0x0018;

    //opcode will be inside buffer
    responseBuffer[0] = 0x20;
    responseBuffer[1] = 0x18;


    //SEND COMMAND
    if (hci_send_cmd(devDescriptor, ogf, ocf, length, responseBuffer) < 0) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    //Wait for the response of the controller
    //answer from controller should be length == 15
    length = read(devDescriptor, responseBuffer, sizeof (responseBuffer));
    if (length < 0) {
        perror("Read failed");
        exit(EXIT_FAILURE);
    }
    //Status: Success (0x00). Must be zero at byte 6
    if (length == 15 && responseBuffer[6] == 0x00) {
        //print buffer as we get it
        printBytes(responseBuffer, length);

        //get Rand from Buffer 
        //     //control data (7 bytes) rand (8 bytes)
        //0000   04 0e 0c 01 18 20 00   d1 67 86 d6 ec fa ca 7f         quin quad trilhões bilhoes milhoes mil
        //Random Number (wireshark ordering): d16786d6ecfaca7f      ours is little endian
        memcpy(&lerand, (responseBuffer + 7), sizeof (uint64_t));
    }


    return lerand;
}

//setup filters to receive events packets from microcontroller

void setFilters(int devDescriptor) {
    struct hci_filter flt;

    hci_filter_clear(&flt);
    hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
    hci_filter_all_events(&flt);
    if (setsockopt(devDescriptor, SOL_HCI, HCI_FILTER, &flt, sizeof (flt)) < 0) {
        perror("HCI filter setup failed");
        exit(EXIT_FAILURE);
    }
}

//open dev

int openDev(int *device) {
    int ret;
    //From BTbook:
    *device = hci_get_route(NULL);
    ret = hci_open_dev(*device);

    if (ret < 0) {
        printf("\"There's a hole in the sky\"... Error on opening HCI device. ");
        exit(-1);
    }
    return ret;
}

int main(int argc, char** argv) {
    int devId, devDescriptor;
    uint64_t lerand;
    //open Dev
    devDescriptor = openDev(&devId);

    //clear filters?
    setFilters(devDescriptor);

    while (1)
        //call command (actually lerand is not used in my application)
        lerand = sendLERandCommand(devDescriptor);
    //show results
    //printf("Random Number: %" PRIu64 "\n", lerand);
    //printf("Random Number: %" PRIx64 "\n", lerand);

    //close dev    
    hci_close_dev(devDescriptor);

    return (EXIT_SUCCESS);
}

