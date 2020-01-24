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


//unused
void printBinary(int n){
  int k;
  int c; 

  for (c = 7; c >= 0; c--) {
     k = n >> c;
     if (k & 1)
       printf("1");
     else
       printf("0");
  }   
}




//Odd or Even Difference
//possible problem: 2 devices with almost always same RSSI. Then the same difference will repeat.
//we could tackle this problem testing only differencies with same BT_ADDR
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


//"Early" Von neumann (last bit of two consecutive RSSI readings)
int vonNeumannExtraction(unsigned char rssiAtual, unsigned char rssiAnterior){
	unsigned char desloc1,desloc2;
	desloc1 = (rssiAtual & 0x1); 
	desloc2 = (rssiAnterior & 0x1);
	//(00 || 11)
	if (desloc1 == desloc2){
		//discard result
        return -1;
	}else{
		if (desloc2 == 0){  //01
			return 0;
		}else{
			return 1;       //10
		}		
	}
	
}

//Just odd or even. For each rssi reading. 
int oddOrEvenExtraction(unsigned char rssiAtual){
	if (rssiAtual % 2 == 0)
		return 0;
	else
		return 1;
}
