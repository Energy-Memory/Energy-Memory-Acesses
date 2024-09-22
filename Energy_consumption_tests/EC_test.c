#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <sched.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>


void assign_to_CPU ( int cpuid )
{
  cpu_set_t * cpusetp = NULL;
  cpusetp = CPU_ALLOC(1);
  if (cpusetp == NULL) {
    perror("CPU_ALLOC");
    exit(EXIT_FAILURE);

  }
  CPU_ZERO_S(CPU_ALLOC_SIZE(1), cpusetp);
  CPU_SET_S(cpuid, CPU_ALLOC_SIZE(1), cpusetp);
  sched_setaffinity(0, sizeof(*cpusetp), cpusetp);
  CPU_FREE(cpusetp);
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

long f_mem(int nb) {
    int* a[nb];
    for (int i = 0; i < nb; i++)
        a[i] = (int*)malloc(nb * sizeof(int));

    for (int j = 0; j < nb; j++) {
        for (int i = 0; i < nb; i++){
            a[i][j] = 5;
        }
    }
    return a[0][0];
}

long f_CPU(int nb) {
    long a = 0;
    for (int i = 0; i < nb * nb ; i++){
    	a++;
    }
    return a;
}


int main (int argv, char ** argc) {

    //Set priority to the maximum
    setpriority(PRIO_PROCESS, 0, -20);
  
    if (argv != 3) {
        perror("Wrong number of arguments");
        return EXIT_FAILURE;
    }
    
    // Enable UART communication
    char *portname = "/dev/ttyAMA0";
    int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0){
        fprintf (stderr, "error %d opening %s: %s\n", errno, portname, strerror (errno));
        return -1;
    }
    set_interface_attribs (fd, 115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (fd, 0);                // set no blocking

    int nb = atoi(argc[1]);

    // Inform the program has started
    write (fd, "begin\n", 7);

    if (!atoi(argc[2]))
        f_CPU(nb);
    else
        f_mem(nb);

    // Inform the program has ended
    write (fd, "end\n", 5);

    char buf [100];
    read (fd, buf, sizeof buf); 

    return 0;
}
