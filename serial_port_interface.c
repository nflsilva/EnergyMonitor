#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

#define BAUD_RATE B9600
#define TIMEOUT_WRITE 10000 //in uSec
#define TIMEOUT_READ 10000
#define MAX_PACKET_SIZE 20 //in bytes
#define SERIAL_PORT_DEVICE "/dev/ttyUSB0"



int serialport_read(int fd, unsigned char* packet)
{
	int n = -1;
	char bytes_remaining[1];
	int i = 1; 
	int j, k;
	int packet_index = 0;
	unsigned char packet_aux[MAX_PACKET_SIZE] = "00000000000000000000";
	while(n<=0) {
		n = read(fd, &bytes_remaining, 1);
		i = atoi(bytes_remaining);
		if(n==1)
			break;
		if(n<=0) {
			usleep(10000);
			continue;
		}
	}
	n=-1;
	printf("REMAN:%d\n", i);
	while(n<=0 && i > 0) {
	n = read(fd, &packet_aux[packet_index], 1);
		if(n<=0) {
			usleep(10000);
			continue;
		} else {
			printf("Read:%d\n", n);
			packet_index++;
			n = -1;
	      i--;
		}
	}
			 
	memcpy(packet, packet_aux, MAX_PACKET_SIZE);
	return 0;
}

int serialport_write(int fd, unsigned char* msg) {
	int l = sizeof(msg)/sizeof(unsigned char);
	int n = -1;
	while(n<=0) {
		n = write(fd, msg, l);
     	if( n==0 ) {
         usleep(10000);
      continue;
     }
	}
	return 0;
}

int serialport_init(const char* serialport, int baud)
{
	struct termios toptions;
	int fd;

	fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)  {
		perror("Unable to open serial port. Program will now exit. Bye bye!");
		return -1;
	}

	if (tcgetattr(fd, &toptions) < 0) {
		perror("Couldn't get term attributes of serial port. Program will now exit. Bye bye!");
		close(fd);
	return -1;
	}
	speed_t brate = baud;
	switch(baud) {
		case 4800:   
			brate=B4800;   
			break;
		case 9600:   
			brate=B9600;   
			break;
		#ifdef B14400
		case 14400:  
			brate=B14400;  
			break;
		#endif
		case 19200:  
			brate=B19200;  
			break;
		#ifdef B28800
		case 28800:  
			brate=B28800;  
			break;
		#endif
		case 38400:  
			brate=B38400;  
			break;
		case 57600:  
			brate=B57600;  
			break;
		case 115200: 
			brate=B115200; 
			break;
	}
	cfsetispeed(&toptions, brate);
	cfsetospeed(&toptions, brate);

	// 8N1
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;
	// no flow control
	toptions.c_cflag &= ~CRTSCTS;

	toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
	toptions.c_oflag &= ~OPOST; // make raw

	toptions.c_cc[VMIN]  = 0;
	toptions.c_cc[VTIME] = 20;

	if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
		printf("Couldn't set term attributes. Program will now exit. Bye bye!");
		close(fd);
		return -1;
	}
	return fd;
}



int main(int argc, char *argv[])
{
	int fd = 0;
	int a;
	unsigned char rx_packet[MAX_PACKET_SIZE];
	unsigned char tx_packet[MAX_PACKET_SIZE];

	fd = serialport_init(SERIAL_PORT_DEVICE, BAUD_RATE);
	if(fd==-1) {
		return -1;
	}

	while (1) {
		memset(tx_packet, 0, sizeof(tx_packet));
		memset(rx_packet, 0, sizeof(rx_packet));

		printf("Please, enter a command:\n");
		scanf("%s", &tx_packet);

		serialport_write(fd, tx_packet);
		serialport_read(fd, rx_packet);
		//printf("%d\n", rx_packet[1]);
		/*
		if(rx_packet[1] == '1')
			printf("Command succefully done!\n");
		else if(rx_packet[1] == '0')
			printf("Error while executing command\n");
		else {
			printf("What?:");
			for(a=0; a < 20; ++a)
				printf("%u ", rx_packet[a]); 
			printf("\n");
		}*/
		for(a=0; a < 20; ++a)
			printf("%u ", rx_packet[a]); 
		printf("\n");
		/*											
      unsigned long int dist = 4290000000;
      unsigned char* l = (unsigned char*)&dist;
		//printf("L[0]:%u\n", &l);		*/

		unsigned long int value = 0;
		unsigned char* p = (unsigned char*) &value;
		p[0] = rx_packet[0];
		p[1] = rx_packet[1];
		p[2] = rx_packet[2];
		p[3] = rx_packet[3];
		printf("Value: %lu\n", value);




		

	}
} // end main
