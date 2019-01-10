// based on: https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c#
//
#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

//#define SERIAL_PORT "/dev/filesystem_mod"
//#define SERIAL_PORT "/dev/ttyACM0"
#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD_RATE B9600

int fd;

int set_interface_attribs(int fd, int speed) {
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    // serial port parameters ///////////////////////////////////////////////////////////
    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;
    /////////////////////////////////////////////////////////////////////////////////////
    
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount) {
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}


int setup(void) {
    char *portname = SERIAL_PORT;
    //char *portname = "/dev/ttyUSB0";
    int wlen;

    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    //set_interface_attribs(fd, B115200);
    //set_mincount(fd, 0);                /* set to pure timed read */
    set_interface_attribs(fd, BAUD_RATE);

    /* simple output */
    wlen = write(fd, "Hello!\n", 7);
    if (wlen != 7) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }
    tcdrain(fd);    /* delay for output */
}


int main(void) {
	int result=0;
	result = setup();

    if ( result == 0 ) {
	    //LOOP//
	    while(1) {
		unsigned char buf[80];
		int rdlen;
		//usleep(1000);
		rdlen = read(fd, buf, sizeof(buf) - 1);
		//rdlen = read(fd, buf, 11 - 1);
		if (rdlen > 0) {
//		#ifdef DISPLAY_STRING_FORMAT
		    buf[rdlen] = 0;
		    //printf("%s", buf);
		    printf("Foi\n", buf);
		    //printf("Read %d: \"%s\"\n", rdlen, buf);
//		#else /* else: display in hexadecimal */
//		    unsigned char *p;
//		    printf("Read %d:", rdlen);
//		    for (p = buf; rdlen-- > 0; p++)
//			printf(" 0x%x", *p);
//		    printf("\n");
//		#endif
		} else if (rdlen < 0) {
		    printf("Error from read: %d: %s\n", rdlen, strerror(errno));
		}
	    }
    }
    return result;
}

//EOF
