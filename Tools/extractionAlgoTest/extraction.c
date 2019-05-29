#include <stdio.h>
#include <stdlib.h>

void printBinary(int n){
  int k;
  int c; 
	printf("\t");
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

//Approach 2
//possible problem: 2 devices with almost always same RSSI. Then the same difference will repeat a lot.
//we could attack this problem testing only differencies with same BT_ADDR
void diferencaParOuImpar( ){
		unsigned char pacoteSaida = 0;
		unsigned char rssiOld,rssiAtual;
		int diferenca, read1, i;
		scanf("%d",&read1);
		rssiOld = (int) read1;
		for (i = 0; i < 8; i++) {
			scanf("%d",&read1);			

			rssiAtual = (int) read1;
			diferenca = rssiAtual - rssiOld;
			if ( (diferenca) != 0) {
 				//leituras diferentes.
				if (diferenca % 2 == 0) //par: inserir 0;
					pacoteSaida = (pacoteSaida << 1);
				else	//impar: inserir 1
					pacoteSaida = (pacoteSaida << 1)+1;

				if (i == 7){
					printBinary(pacoteSaida);printf("\n");
				//	printf("saida= %d\n",pacoteSaida);
				}
			}else
				i --; //go back... dangerous...				
			rssiOld = rssiAtual;
		}	
}


//Approach 3: Early Von neumann ( bit a bit considered)
//não confundir com o bias removal, embora a lógica seja a mesma.
void vonNeumannExtraction(unsigned char rssiAtual, unsigned char rssiAnterior){
	unsigned char desloc1,desloc2, compare = 0;
	desloc1 = (rssiAtual << 7); 
	desloc2 = (rssiAnterior << 7);
	//(00 || 11)
	if ( ((desloc1  >> 7 == 0) && (desloc2  >> 7 ==0)) ||
		((desloc1  >> 7 == 1) && (desloc2 >> 7 ==1)) ){
		//discard
	}else{
		if ( desloc1 >> 7 == 0  ){			
			printf("0");
		}else{
			printf("1");
		}
		

	}
	printf("\n");		
}


int main(){	
	while (1){
		//vonNeumannExtraction(181,180);
		diferencaParOuImpar();
		//nbitsRSSI();
	}
	return 0;
}

/*
Approach1:
        XOR = RSSIatual  ^ RSSIold      
        if (XOR ==0)
                 //descarta... leitura igual
        else
                MenosSignificativos = (XOR << 4) >> 4; (length? Suponho que o info no beacon seja um byte...) 
        PacoteSaida = (PacoteSaida << 4) ||  MenosSignificativos 

Approach2: less throughput
    if RSSIatual == RSSIold
              continue;  //descarta leitura....
    else
        Diferença = RSSIatual - RSSIold
        if (Diferença % 2 == 0)
                 b = 0;
        else
                 b = 1;

Uma terceira approach poderia combinar a 1 com leitura do TxPower...
Poderia fazer a Von Neumann direto:

     if (00 || 11)
	//discard
     else
	if (LSB_RSSI_atual == 0 && LSB_RSSI_anterior == 1)
		//output 0
	else
		//output 1
*/
