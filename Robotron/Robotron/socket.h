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

#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>

class CSocket
{
public:
	// Default Constructors/Destructors
	CSocket();
	~CSocket();

	// This function call does the act of creating a socket and binding it to the supplied port number on the local machine.
	bool initialise(unsigned short portNumber);

	// Accessor methods
	SOCKET getSocketHandle();

	// Additional Methods to enable broadcasting
	void setRemotePort(unsigned short remotePort);
	void setRemoteAddress(unsigned long remoteAddress);

	// Enable broadcasting on the socket
	int enableBroadcast();

	// Disable broadcasting on the socket
	int disableBroadcast();

private:
	//Get the local IP Address to which the socket is bound to
	std::string getLocalAddress(); //Used for printing the local addrerss in the console window.
private:
	//A handle to as of yet unbound socket
	SOCKET m_hSocket;
	//A sockaddr structure containing the local address and port to bind the socket to.
	sockaddr_in m_socketAddress;
	
	//Additional member variables : Details of the remote machine which is sending/receving data via this socket object.
	unsigned long m_remoteIPAddress;
	unsigned short m_remotePort;
};

#endif