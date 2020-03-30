// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include "tftp_client.h"
#include <iostream>

#define PORT     7
#define MAXLINE 1024
#define BUFLEN 512	//Max length of buffer


using namespace std;

typedef struct
{
	uint8_t id;
	uint8_t data[8];
} net_data;


void die(char *s)
{
	perror(s);
	exit(1);
}

void mysleep_ms(int milisec)
{
    struct timespec res;
    res.tv_sec = milisec/1000;
    res.tv_nsec = (milisec*1000000) % 1000000000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &res, NULL);
}


int main(void)
{
	struct sockaddr_in si_me, si_other, server;
	clock_t start, stop;
	int s, s2, i, slen = sizeof(si_other) , recv_len;
	char buf[BUFLEN];
	struct timespec time;
	float value;
	net_data send_data, recv_data;
	char * ip = "192.168.000.002";
	int port = 69;
	char* source = "Sensorboard.bin";
	char* destination = "Sensorboard.bin";
	TFTPClient client(ip, port);

	/*send_data.id = 0x10;
	//create a UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}

	//create a UDP socket
	if ((s2=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket2");
	}

	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    if (inet_aton("192.168.0.2", &si_other.sin_addr) == 0)
    {
		fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }



	//bind socket to port
	if( bind(s , (struct sockaddr*) &si_me, sizeof(si_me) ) == -1)
	{
		die("bind");
	}

	//keep listening for data

	clock_gettime(CLOCK_MONOTONIC_RAW, &time);
	printf("time: %i\n\r", time.tv_nsec/1000ul);
	if (sendto(s, &send_data, sizeof(net_data), 0, (const struct sockaddr *)&si_other, slen) == -1)
	{
		die("sendto()");
	}

	//try to receive some data, this is a blocking call
	if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, (unsigned int*)&slen)) == -1)
	{
		die("recvfrom()");
	}
	memcpy(&recv_data, buf, sizeof(net_data));
	//print details of the client/peer and the data received
	printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	printf("Data: %x\n" , recv_data.id);

	//now reply the client with the same data
	//if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
	//{
	//	die("sendto()");
	//}
	mysleep_ms(100);

*/
	cout << "Starting TFTP client\n";

	if (client.connectToServer() != 1)
	{
		cout << "Error while connecting to server " << ip << endl;
		return 0;
	}
	cout << "Trying to send file " << source << " to " << ip << " " << destination << endl;

	if (client.sendFile(source, destination))
	{
		cout << "File sent successfully\n";
	}
	else
	{
		cout << "Error has occured in file transfer\n";
	}

	close(s);
	return 0;
}
