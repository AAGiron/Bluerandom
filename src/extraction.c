#include "bluerandom.h"


 //to test
//#include <stdio.h>
//#include <stdlib.h>
//#include <curses.h>
//#include <time.h>

void printBinary(int n){
  int k;
  int c; 
//	printf("\t");
  for (c = 7; c >= 0; c--) {
     k = n >> c;
     if (k & 1)
       printf("1");
     else
       printf("0");
  }   
}




//Aproach 1: nao tem bons resultados eu acho.
/*void nbitsRSSI(){
	int read1, i, qntosBits, bitsMenosSig=2;
	unsigned char rssiAtual, rssiOld;
	unsigned char pacoteSaida = 0;
	rssiOld = 0;
	qntosBits = 8 - bitsMenosSig;
		pacoteSaida = 0;
		for (i = 0; i < 8; i+= bitsMenosSig) {
			scanf("%d",&read1);			

			rssiAtual = (int) read1;
//		printBinary(rssiOld);
//		printf("\n");
//		printBinary(rssiAtual);
//		printf("\n\t_________\n");	
			if ( (rssiAtual ^ rssiOld) != 0) {
				//leituras diferentes. Empacotar estes x bitsMenosSign até fechar o byte
			
					   // espaçar x bits			pegar x bits menos Sign deslocando 8-x
				pacoteSaida = (pacoteSaida << bitsMenosSig)  | ((rssiAtual << qntosBits)>> qntosBits);

//				printBinary(pacoteSaida);
//				printf("\n");
				if (i == 8 - bitsMenosSig)
					printBinary(pacoteSaida);printf("\n");
				//	printf("saida= %d\n",pacoteSaida);
			}else
				i -= bitsMenosSig; //go back... dangerous...
			rssiOld = rssiAtual;
		}
}*/

//Approach 2: Odd or Even Difference
//possible problem: 2 devices with almost always same RSSI. Then the same difference will repeat a lot.
//we could attack this problem testing only differencies with same BT_ADDR
int diferencaParOuImpar(unsigned char rssiAtual, unsigned char rssiOld){
		int diferenca;
	
		diferenca = rssiAtual - rssiOld;
		if ( (diferenca) != 0) {
 			//leituras diferentes.
			if (diferenca % 2 == 0) //par: inserir 0;
				return 0;
			else	//impar: inserir 1
				return 1;
						
		}else
            return -1;	//discard result: no RSSI variation
}


//Approach 3: Early Von neumann ( bit a bit considered)
int vonNeumannExtraction(unsigned char rssiAtual, unsigned char rssiAnterior){
	unsigned char desloc1,desloc2;
	desloc1 = (rssiAtual << 7); 
	desloc2 = (rssiAnterior << 7);
	//(00 || 11)
	if ( ((desloc1  >> 7 == 0) && (desloc2  >> 7 ==0)) ||
		((desloc1  >> 7 == 1) && (desloc2 >> 7 ==1)) ){
		//discard result
        return -1;
	}else{
		if ( desloc1 >> 7 == 0  ){			
			return 0;
		}else{
			return 1;
		}		
	}
	
}

//Another test: just odd or even. For each rssi reading. 
int oddOrEvenExtraction(unsigned char rssiAtual){
	if (rssiAtual % 2 == 0)
		return 0;
	else
		return 1;
}



//Approach 3: Early Von neumann Improved 
//In order to improve throughput, use N bits at a time, not just one. 
//Fixme: way that it is implemented (and produceOneByteTest... what a salad), bit quantity greater than 8 will be discarded at the second time.
int vonNeumannExtractionImproved(unsigned char rssiAtual, unsigned char rssiAnterior, int *cont, int r){

    //get bits of RssiAtual and RSSI Anterior 
/*
    char rssi0 = rssiAtual & 0x01;
    char rssi1 = rssiAtual & 0x02;
    char rssi2 = rssiAtual & 0x04;
    char rssi3 = rssiAtual & 0x08;
    char rssi4 = rssiAtual & 0x16;
    char rssi5 = rssiAtual & 0x32;
    char rssi6 = rssiAtual & 0x64;
    char rssi7 = rssiAtual & 0x128;
    char rssia0 = rssiAnterior & 0x01;
    char rssia1 = rssiAnterior & 0x02;
    char rssia2 = rssiAnterior & 0x04;
    char rssia3 = rssiAnterior & 0x08;
    char rssia4 = rssiAnterior & 0x16;
    char rssia5 = rssiAnterior & 0x32;
    char rssia6 = rssiAnterior & 0x64;
    char rssia7 = rssiAnterior & 0x128;
*/
    unsigned char rssi0, rssia0;
    int diferenca,i, qdeBitsDiferenca;
    int count=*cont;
    int r1 = r;    //just if some bits produced last time were not used.

    if (rssiAtual > rssiAnterior)	
    	diferenca = rssiAtual - rssiAnterior;
    else
        diferenca = rssiAnterior - rssiAtual;

	if ( diferenca == 0) {
        return -1;
    }

  //&& count < quantidade de bits(diferenca)
    qdeBitsDiferenca = ceil(log2(diferenca));
//    printf("\tDif:%d\n", diferenca);
//    for (i = 0; i < 8 && count < qdeBitsDiferenca; i ++){
     for (i = 1; i < 3 ; i ++){ //Testes realizados: i < 8, i < 4, não passaram no FIPS 140-2
        rssi0 = rssiAtual & ((int)pow(2,i));
        rssia0 = rssiAnterior & ((int)pow(2,i));
        if (rssi0 ^ rssia0)  {
            count++;

            if (rssia0 == 0)
                r1 <<= 1;
            else
                r1 = (r1 << 1) + 1;
        }                
    }  

    *cont = count; 
    
    return r1;

}


/*
Approach1: not good.
        XOR = RSSIatual  ^ RSSIold      
        if (XOR ==0)
                 //descarta... leitura igual
        else
                MenosSignificativos = (XOR << 4) >> 4; (length? Suponho que o info no beacon seja um byte...) 
        PacoteSaida = (PacoteSaida << 4) ||  MenosSignificativos 

Approach2: better but less throughput
    if RSSIatual == RSSIold
              continue;  //descarta leitura....
    else
        Diferença = RSSIatual - RSSIold
        if (Diferença % 2 == 0)
                 b = 0;
        else
                 b = 1;

Uma terceira approach poderia combinar a 1 com leitura do TxPower...
Quarta approach poderia fazer a Von Neumann direto:

     if (00 || 11)
	//discard
     else
	if (LSB_RSSI_atual == 0 && LSB_RSSI_anterior == 1)
		//output 0
	else
		//output 1
*/

//Comentar: just for testing
/*int countVariationBits=0;
int r = 0;
unsigned char produceOneByteTest(){
    unsigned char random = 0;
    char c;
    int rssi1 = 160;
    int rssi2 = 160;
    int i, j;

    int cont, contBits = 0;

    while (1){
        srand((int)time(NULL));
        i =  rand() % 10;
        j = rand() % 10;
        r = vonNeumannExtractionImproved(rssi1 + j, rssi2 + i, &countVariationBits, r);

        printf("a(%d)=",rssi1 + j);printBinary(rssi1 + j);printf("\n");
        printf("o(%d)=",rssi2 + i);printBinary(rssi2 + i);printf("\n");
        printf("r="); printBinary(r);
        printf("Cont=%d\n",countVariationBits); 

        if (r >= 0) { //here retBit is not a bit
            
            if (contBits + countVariationBits > 8){
                //save some bits for next time
                random = (random << (8 - contBits) ) | (r);
                printf("R="); printBinary(random);
                countVariationBits = countVariationBits - (8 - contBits);
                r >>= (8 - contBits);
                printf("sobrou r="); printBinary(r);
                contBits = 8;
            }else {
                random = (random << countVariationBits) | (r);
                printf("R="); printBinary(random);
                contBits += countVariationBits;
                countVariationBits = 0;
                r = 0;
            }
        }else { //readings iguais. r = -1. next
            r = 0;
            countVariationBits = 0;
        }
        if (contBits >= 8) { //should not be greater... rs
//  		        printf("\t\tRANDOM PRODUCED:%c\n", random);                       //PRINT RESULT BYTE                
                
                return random;            	
        }
        c = getchar();      

    }

    return random;
}

*/

//COMENTAR: Just for testing purposes
/*int main (){
    int i;
    unsigned char random; 

	setbuf(stdout,NULL);

    for (i = 0; i < 10; i ++ ){            
        random = produceOneByteTest();
        printf("Random Produced:%c or %x\n", random, random);        
        
    }
    return 0;
} */


