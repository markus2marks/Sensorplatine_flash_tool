#include "tftp_packet.h"
#include "tftp_client.h"

using namespace std;

TFTPClient::TFTPClient(char* ip, int port) : m_net(port,"192.168.0.5",ip)
{

	TFTP_Packet packet;
	server_ip = ip;
	server_port = port;
	socket_descriptor = -1;

}

int TFTPClient::connectToServer()
{
	cout << "Connecting to " << server_ip << " on port " << server_port << endl;
	if(m_net.net_com_connect() == true)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int TFTPClient::sendBuffer(char *buffer)
{

    return send(socket_descriptor, buffer, (int)strlen(buffer), 0);

}

int TFTPClient::sendPacket(TFTP_Packet* packet)
{
	m_net.net_com_sendto((char*)packet->getData(), packet->getSize());
	return 1;
}

bool TFTPClient::getFile(char* filename, char* destination)
{

	TFTP_Packet packet_rrq, packet_ack;
	ofstream file((const char*)destination, ofstream::binary);

	char buffer[TFTP_PACKET_DATA_SIZE];

	packet_rrq.createRRQ(filename);

	sendPacket(&packet_rrq);

	int last_packet_no = 1;
	int wait_status;
	int timeout_count = 0;

	while (true)
	{
		wait_status = waitForPacketData(last_packet_no, TFTP_CLIENT_SERVER_TIMEOUT);
		if (wait_status == TFTP_CLIENT_ERROR_PACKET_UNEXPECTED)
		{
			received_packet.dumpData();
			file.close();
			return false;

		}
		else if (wait_status == TFTP_CLIENT_ERROR_TIMEOUT)
		{

			//- nebesulaukem paketo

			timeout_count++;

			if (timeout_count < 2)
			{ //- kadangi tai pirmas timeout`as, tai bandom pasiusti paskutini ACK

				sendPacket(&packet_ack);
				continue;

			}
			else
			{
				cout << "Connection timeout" << endl;
				file.close();
				return false;
			}
		}

		if(last_packet_no != received_packet.getNumber())
		{
			//- paketas kazkur paklydo!

			/* TFTP recognizes only one error condition that does not cause
			   termination, the source port of a received packet being incorrect.
			   In this case, an error packet is sent to the originating host. */

			/* Taip negali nutikti, nes pas mus naudojamas ACK Lock`as */

			cout << "This should not happen!" << endl; //- aisku kada kadanors tai vis tiek atsitiks :)

		}
		else
		{

			//- paketas tvarkoj
			received_packet.dumpData();

			last_packet_no++;

			//- jei tai susitvarkes timeoutinis paketas, tai pasidziaukim ir leiskim tai pakartot
			if(timeout_count == 1)
			{
				timeout_count = 0;
			}

			if(received_packet.copyData(4, buffer, TFTP_PACKET_DATA_SIZE))
			{
				file.write(buffer, received_packet.getSize() - 4);

				//- tirkinam, ar gauti duomenis yra mazesni nei buferio dydis
				//- jei taip, tai sis paketas buvo paskutinis
				//- A data packet of less than 512 bytes signals termination of a transfer.

				if (received_packet.getSize() - 4 < TFTP_PACKET_DATA_SIZE)
				{
					/* The host acknowledging the final DATA packet may terminate its side
					   of the connection on sending the final ACK. */
					packet_ack.createACK((last_packet_no - 1));

					if (sendPacket(&packet_ack))
					{
						break;
					}
				}
				else
				{

					//- ne paskutinis, tai siunciam ACK
					//- Each data packet contains one block of data, and must be acknowledged by 
					//- an acknowledgment packet before the next packet can be sent.
					
					packet_ack.createACK((last_packet_no - 1)); //- siunciam toki paketo numeri, kuri gavom paskutini
					
					sendPacket(&packet_ack);

					DEBUGMSG("Acknoledgement sent");

				}
			}
		}
	}

	file.close();

	return true;

}

int TFTPClient::waitForPacket(TFTP_Packet* packet, int timeout_ms)
{

	packet->clear();

	fd_set fd_reader;		  // soketu masyvo struktura
	timeval connection_timer; // laiko struktura perduodama select()

	connection_timer.tv_sec = timeout_ms / 100; // s
	connection_timer.tv_usec = 0; // neveikia o.0 timeout_ms; // ms 

	FD_ZERO(&fd_reader);
	// laukiam, kol bus ka nuskaityti
	FD_SET(socket_descriptor, &fd_reader);
	//int select_ready = select(socket_descriptor + 1, &fd_reader, NULL, NULL, &connection_timer);


	cout << "select" << endl;
	//- turim sekminga event`a

	int receive_status;
	receive_status = m_net.net_com_receive((char*)packet->getData(),TFTP_PACKET_MAX_SIZE);
	cout << "receive_status: " << receive_status << endl;
	if (receive_status == 0)
	{
		cout << "Connection was closed by server\n";
		return TFTP_CLIENT_ERROR_CONNECTION_CLOSED;
    }

	if (receive_status == SOCKET_ERROR)
	{
		DEBUGMSG("recv() error in waitForPackage()");
		return TFTP_CLIENT_ERROR_RECEIVE;
	}
	
	packet->setSize(receive_status);

	return TFTP_CLIENT_ERROR_NO_ERROR;

}

bool TFTPClient::waitForPacketACK(int packet_number, int timeout_ms)
{

	TFTP_Packet received_packet;

	if (waitForPacket(&received_packet, timeout_ms))
	{
		if (received_packet.isError())
		{

			cout << "ACK expected, but got Error" << endl;

			errorReceived(&received_packet);

			return false;
		}

		if (received_packet.isACK())
		{
			cout << "ACK for packet " << received_packet.getNumber() << "(expected: " << packet_number << ")" << endl;
			return true;
		}

		if (received_packet.isData())
		{
			cout << "DATAK for packet " << received_packet.getNumber() << "(expected: " << packet_number << ")" << endl;
			return true;
		}
		cout << "Unhandled packet" << endl;
	}
	else
	{
		DEBUGMSG("We have an error in waitForPacket()");
	}
	return true;
}

int TFTPClient::waitForPacketData(int packet_number, int timeout_ms)
{

	int wait_status = waitForPacket(&received_packet, timeout_ms);

	if (wait_status == TFTP_CLIENT_ERROR_NO_ERROR)
	{
		if (received_packet.isError())
		{
			errorReceived(&received_packet);
			return TFTP_CLIENT_ERROR_PACKET_UNEXPECTED;
		}

		if (received_packet.isData())
		{
			return TFTP_CLIENT_ERROR_NO_ERROR;
		}
	}
	return wait_status;
}

bool TFTPClient::sendFile(char* filename, char* destination)
{
	
	TFTP_Packet packet_wrq, packet_data;
	ifstream file(filename, ifstream::binary);
	char memblock[TFTP_PACKET_DATA_SIZE];

	if (!file.is_open() || !file.good())
	{
		cout << "Unable to open file " << filename << endl;

		return false;
	}

	packet_wrq.createWRQ(destination);
	packet_wrq.dumpData();

	sendPacket(&packet_wrq);

	int last_packet_no = 0;
//	int last_error = 0;

	while (true)
	{
		if (waitForPacketACK(last_packet_no++, TFTP_CLIENT_SERVER_TIMEOUT))
		{
			cout << "send file" << endl;
			file.read(memblock, TFTP_PACKET_DATA_SIZE);

			packet_data.createData(last_packet_no, (char*)memblock, file.gcount());

			sendPacket(&packet_data);

			if (file.eof())
			{
				break;
			}
		}
		else
		{
			cout << "Server has timed out" << endl;
			file.close();
			return false;
		}
	}
	file.close();
	return true;
}

void TFTPClient::errorReceived(TFTP_Packet* packet)
{

	int error_code = packet->getWord(2);

	cout << "Error! Error code: " << error_code << endl;
	cout << "Error message: ";
	
	switch (error_code)
	{

		case 1: cout << TFTP_ERROR_1; break;
		case 2: cout << TFTP_ERROR_2; break;
		case 3: cout << TFTP_ERROR_3; break;
		case 4: cout << TFTP_ERROR_4; break;
		case 5: cout << TFTP_ERROR_5; break;
		case 6: cout << TFTP_ERROR_6; break;
		case 7: cout << TFTP_ERROR_7; break;
		case 0: 
		default: cout << TFTP_ERROR_0; break;
	}
	cout << endl;
	this->~TFTPClient();
}

TFTPClient::~TFTPClient()
{

	m_net.net_com_close();

}

void DEBUGMSG(char *msg)
{
    #ifdef DEBUG
	std::cout << msg << "\n";
    #endif
}
