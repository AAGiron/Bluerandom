//adapted from: https://github.com/rfinnie/twuewand/blob/master/rndaddentropy/rndaddentropy.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/random.h>
#include <unistd.h>
struct rand_pool_info2 {
    int entropy_count;
    int buf_size;
    char buf[1];
};


#define SIZE_BUFFER 1
/*
 * 
 */
int main(int argc, char** argv) {

    struct rand_pool_info2 info;

    int randfd;
    if ((randfd = open("/dev/random", O_WRONLY)) < 0) {
        perror("/dev/random");
        return (1);
    }
        
    setbuf(stdout, NULL);
    while (1){
    //while ((count = fread(entropy.buf, 1, sizeof (entropy.buf), stdin)) > 0) {
        int reads;
        
        //keep reading from stdin. 
        reads = fread(info.buf, 1, SIZE_BUFFER, stdin);
        //printf("reads:%d\n",reads);
        if (reads < 0 ){
            printf("Not enough entropy data.\n");    
            exit(reads);
        }else{
            if (reads ==0 ){
                //printf("Zero data read.\n");    
                //exit(reads);
                continue;   
            }
        }                
        //system("cat /proc/sys/kernel/random/entropy_avail");
        
       // sleep(1);
        
        info.entropy_count = reads * 8; //ATTENTION HERE. Data from bluerandom gets 7.994...
        info.buf_size = reads;
        if (ioctl(randfd, RNDADDENTROPY, &info) < 0) {
            perror("RNDADDENTROPY");
            printf("ERRO! RNDADDENTROPY\n");
            return (1);
        }
        //printf("OIOIOI:\n");
        //system("cat /proc/sys/kernel/random/entropy_avail");
        
    }
    //close(randfd);

    return (EXIT_SUCCESS);
}

