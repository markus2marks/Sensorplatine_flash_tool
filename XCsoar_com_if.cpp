/*
 * XCsoar_com_if.cpp
 *
 *  Created on: May 9, 2020
 *      Author: markus
 */


#include "XCsoar_com_if.h"


XCsoar_com_if::XCsoar_com_if(int port) : m_port(port)
{

}

bool XCsoar_com_if::connect_to_xcsoar()
{
	bool ret = true;


	// Socket is connected
	// Open Socket for TCP/IP communication to XCSoar
	xcsoar_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (xcsoar_sock == -1)
	{
		ret = false;
	}
	else
	{
		s_xcsoar.sin_addr.s_addr = inet_addr("127.0.0.1");
		s_xcsoar.sin_family = AF_INET;
		s_xcsoar.sin_port = htons(m_port);

		// try to connect to XCSoar
		if(connect(xcsoar_sock,(struct sockaddr *) &s_xcsoar, sizeof(s_xcsoar)) < 0)
		{
			ret = false;
		}
		// make socket to XCsoar non-blocking
		fcntl(xcsoar_sock, F_SETFL, O_NONBLOCK);

	}
	return ret;
}
