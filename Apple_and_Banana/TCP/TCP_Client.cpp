#include "pch.h"
#include "TCP_Client.h"


bool TCP_Client::Open() {
	if (IsOpened())
		Close();

	WSADATA wsaData;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	int iResult;

	printf("服务器端IP：%s\n", ServerIP.c_str());

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup失败，错误代码: %d\n", iResult);
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(ServerIP.c_str(), PORT.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo失败，错误代码: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("套接字创建失败，错误代码: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("无法连接服务器。\n");
		WSACleanup();
		return false;
	}

	return true;
}

bool TCP_Client::Close() {
	int iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("连接关闭失败。错误代码: %d\n", WSAGetLastError());
	}
	// 清理
	closesocket(ConnectSocket);
	WSACleanup();
	return iResult != SOCKET_ERROR;
}

bool TCP_Client::Send(std::string str) {
	// Send an initial buffer
	int iResult = send(ConnectSocket, str.c_str(), str.length(), 0);
	if (iResult == SOCKET_ERROR) {
		printf("数据发送失败。错误代码: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return false;
	}
	printf("发送字节数: %ld\n", iResult);
	return true;
}
