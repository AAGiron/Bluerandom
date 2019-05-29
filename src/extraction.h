#ifndef _H_EXTRACTION
#define _H_EXTRACTION

//Global: EXTRACTION_METHOD
// 0 Odd or Even Difference
// 1 Early von Neumann
// 2 Just Odd or Even
#define EXTRACTION_METHOD 2

//Functions: "Extraction" methods:
int diferencaParOuImpar(unsigned char rssiAtual, unsigned char rssiAnterior); //odd or even RSSI diference
int vonNeumannExtraction(unsigned char rssiAtual, unsigned char rssiAnterior);
int oddOrEvenExtraction(unsigned char rssiAtual);

//unused | testing purposes:
void printBinary(int n);
unsigned char produceOneByteTest();

#endif


