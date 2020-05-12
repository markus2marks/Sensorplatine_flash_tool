/*
 * XCsoar_com_if.h
 *
 *  Created on: May 9, 2020
 *      Author: markus
 */

#ifndef XCSOAR_COM_IF_H_
#define XCSOAR_COM_IF_H_

#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

class XCsoar_com_if
{
	public:
		int	m_port;
		int xcsoar_sock;
		struct sockaddr_in s_xcsoar;
		XCsoar_com_if(int port);
		bool connect_to_xcsoar();
		int send_to_xcsoar(char * client_message, int length);
};



#endif /* XCSOAR_COM_IF_H_ */
