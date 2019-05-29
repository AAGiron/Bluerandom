/* 
 * File:   main.c
 * Author: giron
 *
 * Created on 18 de Abril de 2019, 14:43
 * 
 * 
 * the main purpose of this implementation is serve a bluerandom test
 * Bluerandom (as entropy source) serving SHA based PRNG
 * 
 * In order to simulate a comparison with LE Rand, which is "probably sha-based or AES-based..."
 * Because microcontrollers are kinda closed sheets...
 * 
 *                                                                                                   
 */

#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdint.h>

#include <inttypes.h>
#include <unistd.h>


/*
 From LRNG 2018
Naturally the amount of entropy the DRNG can hold is defined by the DRNG’s security strength. For example, an HMAC SHA-256 DRNG or the
ChaCha20-based DRNG can only hold 256 bits of entropy. Seeding that DRNG with more entropy without pulling random numbers from it will
not increase the entropy level in that DRNG.

 * From Stallings book: "(Chapter 12: SP 800-90 e no ISO 18031) seedlen = tamanho em bits de V >= k + 64, onde k é um nível de segurança desejado, expresso em bits"
 * 
 * So this ENTROPY_SIZE variable will get 64 bits. To produce 64 bits at a time (LE Rand command is uint64 also).
 *  
 * Maybe we should test with an implementation of SHA-1 as well. Or not. 
 */

#define ENTROPY_SIZE 8  //in bytes. 64 bits
//#define SEED_THRESHOLD 800000 //100 000 calls
#define SEED_THRESHOLD 1048576 //100 000 calls
/*
 * About the threshold: when 1048576 bytes (1 MB) are produced, 131.072 sha's were executed.
 */


//Retorno 0: sem erros
int hashSHA256(unsigned char* message, unsigned long t, unsigned char* md) {
    SHA256_CTX context;
    if (!SHA256_Init(&context))
        return 1;

    if (!SHA256_Update(&context, message, t))
        return 1;

    if (!SHA256_Final(md, &context))
        return 1;

    return 0;
}

void updateSeed(unsigned char *V){
    int reads;
    //read from stdin    
    reads = fread(V, 1, (ENTROPY_SIZE ), stdin);
    if (reads < ENTROPY_SIZE ){
        printf("Not enough entropy data.\n");    
        exit(reads);
    }    
}

void printMSBBytes(unsigned char *responseBuffer, int length) {
    int i;

    for (i = 0; i < length; i++) {
        printf("%c", responseBuffer[i]); // /dev/random print approach :-)   
        //    printf("%hhx",responseBuffer[i]);
    }

    /* printf("%d_AESPRNG:|",++i);
        int j;
        for (j = 0; j < AES_BLOCK_SIZE; j++){
            printf("%hhx",responseBuffer[j]);
        }
        printf("|\n");        */
}

//print in order length..0
void printLSBBytes(unsigned char *responseBuffer, int length) {
    int i;

    for (i = length-1; i >= 0; i--) {
        printf("%c", responseBuffer[i]); // /dev/random print approach :-)   
        //    printf("%hhx",responseBuffer[i]);
    }
}

int main(int argc, char** argv) {

    unsigned char md[SHA256_DIGEST_LENGTH];
    unsigned char *V = malloc(ENTROPY_SIZE + 1);
    
    long int threshold_count = 0; 
    long int countMB = 0; //in MB: value * 1048576
    //    uint64_t *rand;
    //  int i = 0;
    
    updateSeed(V);
    

    setbuf(stdout, NULL);
    while (1) {

        //printf("%d_M:|%s|\n",++i,M);
        //sha it.
        if (hashSHA256(V, ENTROPY_SIZE, md) > 0) {
            printf("CRITICAL\n");
            exit(-1);
        }
                

        //print it out.                       
        //rand = (uint64_t *) md;
        //printf("64 bits of SHA: %" PRIx64 "\n", *rand);

        //8 bytes because le rand prints 8 bytes at a time. (also in NIST SP 800-90: print left-most...)
        printMSBBytes(md, 8);
        
        memcpy( V,  (md) , 8);
        
        threshold_count += SHA256_DIGEST_LENGTH;
        if (threshold_count >= (SEED_THRESHOLD - SHA256_DIGEST_LENGTH)) {
            updateSeed(V);
            threshold_count = 0 ;
            countMB++;
        }        
    }

    free(V);
    printf("COUNT:%ld MB", countMB);

    return (EXIT_SUCCESS);
}

