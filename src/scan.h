#ifndef SCAN_H
#define SCAN_H

#ifdef __cplusplus
extern "C" {
#endif

//Time of the inquiry: default 8 
#define lengthInquiry 1

//functions
void setInquiryScanParameters(int socketDescriptor) ;
int inquiryScan(int socketDescriptor) ;
    

#ifdef __cplusplus
}
#endif

#endif /* SCAN_H */

