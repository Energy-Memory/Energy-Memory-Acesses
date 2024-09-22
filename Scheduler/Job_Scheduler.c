#define _GNU_SOURCE
//#define TACLeBENCH_librairies_path "/home/mmabrouk/Documents/tacle-bench/bench/librairies/"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <time.h>

#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/adpcm_dec/adpcm_dec.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/adpcm_enc/adpcm_enc.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/ammunition/ammunition.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/anagram/anagram.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/audiobeam/audiobeam.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/cjpeg_transupp/cjpeg_transupp.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/cjpeg_wrbmp/cjpeg_wrbmp.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/dijkstra/dijkstra.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/epic/epic.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/fmref/fmref.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/g723_enc/g723_enc.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/gsm_dec/gsm_dec.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/gsm_enc/gsm_enc.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/h264_dec/h264_dec.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/huff_dec/huff_dec.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/huff_enc/huff_enc.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/mpeg2/mpeg2.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/ndes/ndes.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/petrinet/petrinet.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/rijndael_dec/rijndael_dec.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/rijndael_enc/rijndael_enc.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/statemate/statemate.h"
#include "/home/mabrouki/Documents/tacle-bench/bench/librairies/susan/susan.h"

int max (int a, int b)
{
    return a >= b ? a : b;
}

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                fprintf(stderr, "error %d from tcgetattr\n", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                fprintf(stderr, "error %d from tcsetattr\n", errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0) {
            fprintf(stderr, "error %d from tggetattr\n", errno);
            return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        /* if (tcsetattr (fd, TCSANOW, &tty) != 0)
            perror ("error %d setting term attributes", errno); */
}

int main ()
{
    setpriority(PRIO_PROCESS, 0, -20);
    
    char *portname = "/dev/ttyAMA0";

    int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0){
        fprintf (stderr, "error %d opening %s: %s\n", errno, portname, strerror (errno));
        return -1;
    }

    set_interface_attribs (fd, 115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (fd, 0);                // set no blocking

    /* main_adpcm_dec(); //OK
    main_adpcm_enc(); //OK
    main_anagram(); //LIBRARY NOT OK
    main_ammunition(); //OK (do not use, assign CPU is not working)
    main_audiobeam(); //LIBRARY NOT OK
    main_cjpeg_transupp(); //OK
    main_cjpeg_wrbmp(); //OK
    main_dijkstra(); //OK
    main_epic(); //OK
    main_fmref(); //OK
    main_g723_enc(); //OK
    main_gsm_dec(); //OK
    main_gsm_enc(); //OK
    main_h264_dec(); //OK
    main_huff_dec(); //LIBRARY NOT OK
    main_huff_enc(); //OK
    main_mpeg2(); //OK
    main_ndes(); //OK
    main_petrinet(); //OK
    main_rijndael_dec(); //OK
    main_rijndael_enc(); //OK
    main_statemate(); //OK
    main_susan(); //OK (do not use, assign CPU is not working) */

    pthread_t thA, thB, thC;
    clock_t start_t;
    struct timespec remaining_time;

    write (fd, "begin\n", 7);

    for(int i = 0; i < 1000; i++){

        start_t = clock();

        /* Releases */
        pthread_create(&thA, NULL, (void *) main_statemate, NULL);
        pthread_setschedprio(thA, -20);
        pthread_create(&thB, NULL, (void *) main_ndes, NULL);
        pthread_setschedprio(thB, -19);
        pthread_create(&thC, NULL, (void *) main_cjpeg_wrbmp, NULL);

        /* Check if there is a deadline miss */

        remaining_time.tv_nsec = 4930000 - (((clock() - start_t)/CLOCKS_PER_SEC)*1000000000);

        if (pthread_timedjoin_np(thA, NULL, &remaining_time) != 0 || pthread_timedjoin_np(thB, NULL, &remaining_time) != 0 || pthread_timedjoin_np(thC, NULL, &remaining_time) !=  0){
            printf("Deadline miss !\n");
            exit(EXIT_FAILURE);
        } 
    }

    char buf [100];
    read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read

    return 0;
}
