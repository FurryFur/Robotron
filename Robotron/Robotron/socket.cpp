//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A wrapper class for windows sockets.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "socket.h"

#include <iostream>

CSocket::CSocket()
	:m_hSocket(0)
{
	ZeroMemory(&m_socketAddress, sizeof(m_socketAddress));
}

CSocket::~CSocket()
{
	closesocket(m_hSocket);
}

//Implicit Assumption: We are creating only UDP sockets.....
bool CSocket::initialise(unsigned short portNumber)
{
	int error;
	//Create a UDP socket 
	m_hSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_hSocket)
	{
		error = WSAGetLastError();
		std::cout << "Unable to create socket\n";
		std::cout << "Error code: " << error;
		return false;
	}
	else
	{
		// Set Non-Blocking
		u_long mode = 1;
		int result = ioctlsocket(m_hSocket, FIONBIO, &mode);
		if (result != NO_ERROR)
			printf("ioctlsocket failed with error: %ld\n", result);
		else
			std::cout << "Successfully created the socket" << std::endl;
	}

	// now bind the socket to the address and port number supplied.
	m_socketAddress.sin_family = AF_INET;
	m_socketAddress.sin_port = htons(portNumber);
	m_socketAddress.sin_addr.S_un.S_addr = INADDR_ANY;
	//Qs 1 : Change if to while; incrementing the port number every time!
	while (0 != bind(m_hSocket, reinterpret_cast<sockaddr*>(&m_socketAddress), sizeof(m_socketAddress)))
	{
		// Cannot bind to the specified IP and port; 
		//Try with an incremented port number
		portNumber++;

		m_socketAddress.sin_port = htons(portNumber);

	}
	std::cout << "Successfully bound the socket " << "to " << getLocalAddress() << ":" << portNumber << std::endl;
	
	return true;
}

SOCKET CSocket::getSocketHandle()
{
	return m_hSocket;
}

std::string CSocket::getLocalAddress()
{
	char localHostname[256];
	char localAddress[256];
	struct sockaddr_in* sockaddr_localIP;

	gethostname(localHostname, 256);

	struct addrinfo hints, *result, *p;
	int returnValue;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;


	returnValue = getaddrinfo(localHostname, NULL, &hints, &result);

	/* loop over all returned results to get the address */
	for (p = result; p != NULL; p = p->ai_next) {
		sockaddr_localIP = (struct sockaddr_in *) p->ai_addr;
		inet_ntop(AF_INET, &(sockaddr_localIP->sin_addr), localAddress, 256);
	}

	return std::string(localAddress);
}

int CSocket::enableBroadcast()
{
	int broadcastOption = 1;
	int reult = setsockopt(m_hSocket, SOL_SOCKET, SO_BROADCAST, 
		reinterpret_cast<char*>(&broadcastOption), sizeof(broadcastOption));
	if (reult == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		std::cout << "Unable to enable broadcast option on the socket" << std::endl;
		std::cout << "Error code: " << error;
		closesocket(m_hSocket);
		return error;
	}
	return reult;
}


int CSocket::disableBroadcast()
{
	int broadcastOption = 0;
	int result = setsockopt(m_hSocket, SOL_SOCKET, SO_BROADCAST, 
		reinterpret_cast<char*>(&broadcastOption), sizeof(broadcastOption));
	if (result == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		std::cout << "Unable to disable broadcast option on the socket" << std::endl;
		std::cout << "Error code: " << error;
		closesocket(m_hSocket);
		return error;
	}
	return result;
}

void CSocket::setRemotePort(unsigned short remotePort)
{
	m_remotePort = remotePort;
}

void CSocket::setRemoteAddress(unsigned long remoteAddress)
{
	m_remoteIPAddress = remoteAddress;
}