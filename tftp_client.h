#ifndef TFTPCLIENT
#define TFTPCLIENT

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "tftp_packet.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include "net_com.h"

#define TFTP_CLIENT_SERVER_TIMEOUT 2000

#define TFTP_CLIENT_ERROR_TIMEOUT 0
#define TFTP_CLIENT_ERROR_SELECT 1
#define TFTP_CLIENT_ERROR_CONNECTION_CLOSED 2
#define TFTP_CLIENT_ERROR_RECEIVE 3
#define TFTP_CLIENT_ERROR_NO_ERROR 4
#define TFTP_CLIENT_ERROR_PACKET_UNEXPECTED 5


#define TFTP_ERROR_0 "Not defined, see error message (if any)"
#define TFTP_ERROR_1 "File not found"
#define TFTP_ERROR_2 "Access violation"
#define TFTP_ERROR_3 "Disk full or allocation exceeded"
#define TFTP_ERROR_4 "Illegal TFTP operation"
#define TFTP_ERROR_5 "Unknown transfer ID"
#define TFTP_ERROR_6 "File already exists"
#define TFTP_ERROR_7 "No such user"

#define SOCKET_ERROR -1

class TFTPClient
{

	private:

		char* server_ip;
		int	server_port;

		//- kliento socketo descriptorius
		int socket_descriptor;

		//- socket'o endpoint'u strukturos
		struct sockaddr_in client_address;
		struct sockaddr_in server_address;
		int connection;

		TFTP_Packet received_packet;
		Net_com m_net;
	protected:

		int sendBuffer(char *);
		int sendPacket(TFTP_Packet* packet);

	public:

		TFTPClient(char* ip, int port);
		~TFTPClient();

		int connectToServer();
		bool getFile(char* filename, char* destination);
		bool sendFile(char* filename, char* destination);

		int waitForPacket(TFTP_Packet* packet, int timeout_ms = TFTP_CLIENT_SERVER_TIMEOUT);
		bool waitForPacketACK(int packet_number, int timeout_ms = TFTP_CLIENT_SERVER_TIMEOUT);
		int waitForPacketData(int packet_number, int timeout_ms = TFTP_CLIENT_SERVER_TIMEOUT);

		void errorReceived(TFTP_Packet* packet);

};

class ETFTPSocketCreate: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Unable to create a socket";
	}
};

class ETFTPSocketInitialize: public std::exception {
  virtual const char* what() const throw() {
    return "Unable to find socket library";
  }
};

void DEBUGMSG(char*);

#endif
