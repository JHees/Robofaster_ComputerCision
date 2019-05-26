#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class TCP_Client
{
public:
	TCP_Client(std::string server_name) : ServerIP(server_name) {}
	~TCP_Client() { Close(); }
	bool Open();
	bool Close();
	bool Send(std::string str);
	bool IsOpened() { return ConnectSocket != INVALID_SOCKET; }
private:
	SOCKET ConnectSocket = INVALID_SOCKET;
	std::string ServerIP;
	const std::string PORT = "127";
};