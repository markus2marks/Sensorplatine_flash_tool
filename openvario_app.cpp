// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/timeb.h>
#include <time.h>
#include "tftp_client.h"
#include <iostream>
#include "net_com.h"
#include <curses.h>


#define PORT     7
#define MAXLINE 1024
#define BUFLEN 512	//Max length of buffer

using namespace std;
char nmea_string[256];


typedef struct
{
	uint8_t id;
	uint8_t data[8];
} net_data;

struct sensor_data
{
	uint8_t id;
	uint32_t timestamp;
	float sensor1;
	float sensor2;
	float sensor3;
	float sensor4;
	float sensor5;
	float sensor1_raw;
	float sensor2_raw;
	float sensor3_raw;
	float sensor4_raw;
	float sensor5_raw;
	int32_t sensor6;
	int32_t sensor7;
	float temp1;
	float temp2;
	float temp3;
	float temp4;
	float temp5;
	int32_t temp6;
	int32_t temp7;
	float accel_x;
	float accel_y;
	float accel_z;
	float gyro_x;
	float gyro_y;
	float gyro_z;
};

void mysleep_ms(int milisec)
{
    struct timespec res;
    res.tv_sec = milisec/1000;
    res.tv_nsec = (milisec*1000000) % 1000000000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &res, NULL);
}

int getMilliCount(void)
{
	timeb tb;
	ftime(&tb);
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}

void readValues(Net_com* net)
{
	struct sensor_data rx_data;
	int temp_time;
	int temp_timestamp;
	initscr();
	cbreak();
	nodelay(stdscr, TRUE);
	while(getch() <= 0)
	{

		int r = net->net_com_receive(&rx_data, sizeof(struct sensor_data));
		if(r > 0)
		{
			cout << (getMilliCount() - temp_time) << "ms\t"<< "timestamp: " << (rx_data.timestamp - temp_timestamp) << "\tid:" << (int)rx_data.id << " p1:" << (int)rx_data.sensor1 << " p2:" << (int)rx_data.sensor2 << " p3:" << (int)rx_data.sensor3 << "\n\r";
			temp_timestamp = rx_data.timestamp;
		}
		temp_time = getMilliCount();
	}
	endwin();
}

void diag_request(Net_com* diag)
{
	net_data send_data, recv_data;
	cout << "check Session" << endl;


	send_data.id = 0x30;
	diag->net_com_sendto(&send_data, sizeof(net_data));
	diag->net_com_receive(&recv_data, sizeof(net_data));
	cout << "Data: " << (int)recv_data.id << endl;
	cout << "Data: " << (int)recv_data.data[0] << endl;

	cout << "Session: " << (int)recv_data.data[0] <<  "\n";

}

void flash_program(Net_com* net)
{
	int tftp_port = 69;
	char * ip = "192.168.0.3";
	char* source = "/mnt/c/Users/marku/OneDrive/Dokumente/ARM/Sensorbox-Ethernet/BUILD/Sensorbox-Ethernet.bin";
	char* destination = "/mnt/c/Users/marku/OneDrive/Dokumente/ARM/Sensorbox-Ethernet/BUILD/Sensorbox-Ethernet.bin";
	TFTPClient client(ip, tftp_port);
	net_data send_data, recv_data;

//	send_data.id = 0x40;
//	cout << "Starting Bootloader\n";
//	net->net_com_sendto(&send_data, sizeof(net_data));
	cout << "check Session" << endl;
	do
	{

		send_data.id = 0x30;
		net->net_com_sendto(&send_data, sizeof(net_data));
		net->net_com_receive(&recv_data, sizeof(net_data));
		cout << "Data: " << (int)recv_data.id << endl;
		cout << "Data: " << (int)recv_data.data[0] << endl;
	}while(recv_data.id != 0x50);

	cout << "Session: " << (int)recv_data.data[0] <<  "\n";

	if(recv_data.data[0] == 0x02)
	{
		cout << "Starting Bootloader" << endl;
		send_data.id = 0x40;
		net->net_com_sendto(&send_data, sizeof(net_data));
		cout << "wait for Bootloader\n";
		usleep(4000000UL);

		do
		{
			send_data.id = 0x30;
			net->net_com_sendto(&send_data, sizeof(net_data));
			net->net_com_receive(&recv_data, sizeof(net_data));
			cout << "Data: " << (int)recv_data.id << endl;
			cout << "Data: " << (int)recv_data.data[0] << endl;

		}
		while((recv_data.id != 0x50) || (recv_data.data[0] != 0x01));
	}
	send_data.id = 0x10;
	cout << "Starting TFTP Server\n";
	net->net_com_sendto(&send_data, sizeof(net_data));
	net->net_com_receive(&recv_data, sizeof(net_data));
	cout << "Data: " << (int)recv_data.id << endl;

	cout << "Starting TFTP client\n";

	if (client.connectToServer() != 1)
	{
		cout << "Error while connecting to server " << ip << endl;

	}
	else
	{
		cout << "Trying to send file " << source << " to " << ip << " " << destination << endl;

		if (client.sendFile(source, destination))
		{
			cout << "File sent successfully" << endl;
			send_data.id = 0x20;
			cout << "starting program" << endl;
			net->net_com_sendto(&send_data, sizeof(net_data));
		}
		else
		{
			cout << "Error has occured in file transfer\n";
		}
	}
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


	char * ip = "192.168.0.2";
	int port = 69;
	char* source = "Application.bin";
	char* destination = "Application.bin";
	TFTPClient client(ip, port);
	Net_com net(7,"192.168.0.5","192.168.0.3");
	Net_com diag(8,"192.168.0.5","192.168.0.3");
	send_data.id = 0x10;

	char input;
	net.net_com_connect();
	diag.net_com_connect();

	while(true)
	{
		cout << "Choise:" << endl;
		cout << "1: read values" << endl;
		cout << "2: start bootloader" << endl;
		cout << "3: get Session" << endl;
		input = getchar();
		switch(input)
		{
			case '1':
				readValues(&net);
				break;

			case '2':
				flash_program(&diag);
				break;

			case '3':
				diag_request(&diag);
				break;

			default:
				cout << "error!" << endl;
				break;
		}
	}
	return 0;
}
