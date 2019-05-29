/* 
 * File:   main.c
 * Author: giron
 *
 * Created on 18 de Abril de 2019, 14:36
 * 
 * 
 * the main purpose of this implementation is serve a bluerandom test
 * Bluerandom (as entropy source) serving AES_PRNG
 * 
 * In order to simulate a comparison with LE Rand, which is "probably sha-based or AES-based..."
 * Because microcontrollers are kinda closed sheets...
 * 
 * Uses mcrypt. 
 * 
 * Also, beware: LE Rand is a uint64_t type, so we assuming that they release to stdout the high-order (most significant) bytes...
 * and we output only 8 bytes.
 * 
 * The input is the entropy (from bluerandom) for the IV and for the key.
 *      - Plaintext is made of? Zeros?
 *      - IV size is the same of the block which is always 128 bit (= 16 bytes)
 *      - keylength may be 128, 192 or 256 (16, 24, 32 bytes)
 *      ------ SO WE WOULD NEED 16 + 32 = 48 (AES256) or 16+16=32 (AES128) bytes of entropy :O (just like stallings said... haha)
 * 
 *      SEE STALLINGS (pg 190 table 7.4) FOR keylen, seedlen and reseed_interval
 * 
 * 
 *  An example from https://github.com/vincenthz/hs-cprng-aes/blob/master/Crypto/Random/AESCtr.hs
 *      This guy uses 64 bytes of entropy: 16 bytes IV, 16 bytes counter, 32 bytes key
 * "By default, this generator will automatically reseed after generating 1 megabyte of data."
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mcrypt.h>
#include <stdint.h>
#include <bluetooth/bluetooth.h>

//128 or 256
#define AES_OPTION 128
#define AES_BLOCK_SIZE 16
//#define ENTROPY_SIZE 32  //see comments above
#define SEED_THRESHOLD 1048576 //1MB
//#define SEED_THRESHOLD 800000 //to get at most 100 000 calls
//#define OUTPUT_QUANTITY_BYTES 1       //1 Gbit of data to test. But we can set it on rngtest -c...




//print in order 0..length
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

//Init Mcrypt module and IV
//returns IVsize
int initMcryptModule(MCRYPT *td, char* IV, char *algo, char *algoMode) {
    int i;
    *td = mcrypt_module_open(algo, NULL, algoMode, NULL);
    if (*td == MCRYPT_FAILED) {
        printf("INIT FAILED CLIENTE");
        return -1;
    }

    //IV size from mcrypt
    int IVsize = mcrypt_enc_get_iv_size(*td);    
    return IVsize;
}

//set IV and KEY for the first time
//or update it when seed_threshold is reached
void setMcryptParameters(MCRYPT *td, char *key, int keysize, char* IV){
    int r;
    if (r = mcrypt_generic_init(*td, key, keysize, IV) < 0) {
        mcrypt_perror(r);
        printf("MCRYPT INIT FAILED");
        exit(-1);
    }
}

//init IV (seed or reseed it)
void initOrUpdateIV(char* IV, int IVsize) {    
        
    //read from stdin    
    int reads = fread(IV, 1, IVsize, stdin);

    if (reads < IVsize) {        
        printf("Not enough data for the IV\n");
        exit(reads);
    }
}

//init key (seed or reseed it)
void initOrUpdateKey(char *key, int keysize) {

    int reads = fread(key, 1, keysize, stdin);
    if (reads < keysize) {        
        printf("Not enough data for the IV\n");
        exit(reads);
    }
}

//init the "plaintext" (in the beginning) or update the aes ciphered buffer with entropy data (seed or reseed it).
//check comments if "zero filling" is sufficient.
void initOrUpdateAES_buffer(char *buffer, int aesblocksize) {
    //question: all zeros for the aes_buffer (the "plaintext" to cipher) in the beginning? nop
    //init block    
    int reads = fread(buffer, 1, aesblocksize, stdin);
    if (reads < aesblocksize) {        
        printf("Not enough data for the AES block\n");
        exit(reads);
    }    
    //or comment all above and fill it with zeros
    //char *plaintextZero = calloc(AES_BLOCK_SIZE,1); //all zero
    //memcpy(aes_buffer, plaintextZero, AES_BLOCK_SIZE);
    //free (plaintextZero);
}

////////////////////////////////////////////////////////////////////////////////

//reads from stdin and AES it, but to stdout only uint64 at a time.
int main(int argc, char** argv) {
    MCRYPT td;
    long int threshold_count = 0; 
    long int countMB = 0; //in MB: value * 1048576
    //AES-256 params
    int keysize;
    char algo[20];
    char *mode = "ctr";
    char *key;
    char *IV;
    char *aes_buffer = malloc(AES_BLOCK_SIZE);    
    
    if (AES_OPTION == 256) {
        strcpy(algo, "rijndael-256");
        keysize = 32;
    } else {
        if (AES_OPTION == 192) {
            strcpy(algo, "rijndael-192");
            keysize = 24;
        } else {
            strcpy(algo, "rijndael-128");
            keysize = 16;
        }
    }
    //init mcrypt: 
    int IVsize = initMcryptModule(&td, IV, algo, mode);

    //init (seed) IV
    IV = malloc(IVsize);
    initOrUpdateIV(IV, IVsize);

    //init (seed) Key
    key = malloc(keysize);
    initOrUpdateKey(key, keysize);

    //init aes block of data to be ciphered. (with zeros? With entropy data? choose one)
    initOrUpdateAES_buffer(aes_buffer, AES_BLOCK_SIZE);
    
    //Init Alg params
    setMcryptParameters(&td, key, keysize, IV);
                
    setbuf(stdout, NULL);
    //generation steps
    //while (!feof(stdin)) { //this while should be different
    while (1) {
        //Cifrar
        mcrypt_generic(td, aes_buffer, AES_BLOCK_SIZE);
        //print it out                  LE RAND prints uint64_t, so here is 16 / 2 bytes
        printMSBBytes(aes_buffer, AES_BLOCK_SIZE / 2);
                                       
        threshold_count += AES_BLOCK_SIZE;
        if (threshold_count >= (SEED_THRESHOLD - AES_BLOCK_SIZE)) {             //RESEED
            initOrUpdateKey(key, keysize);            
            // In libmcrypt the counter is the given IV which is incremented at each step.
            initOrUpdateIV(IV,IVsize);
            
            //update context?
            setMcryptParameters(&td,key,keysize,IV); 
            threshold_count = 0 ;
            countMB++;
        }
    }

    mcrypt_generic_end(td);
    free(key);
    free(aes_buffer);
    free(IV);
    
    printf("COUNT:%ld MB", countMB);

    return (EXIT_SUCCESS);
}

