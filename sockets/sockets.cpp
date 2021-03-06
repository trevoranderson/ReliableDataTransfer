#include "sockets.h"
#include "Server.h"
#include "Client.h"
#include <iostream>
#include <string>
#include <mutex>
std::mutex outmut;

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512

#define DEFAULT_SERVER_PORT "54321"
std::string bufferToString(char * buffer, int buffLen)
{
	//char * message = new char[buffLen + 1];
	//memcpy(message, &buffer[0], buffLen);
	//message[buffLen] = 0;
	//string ret = message;
	//delete message;
	//return ret;
	std::string ret;
	ret.resize(buffLen);
	memcpy(&ret[0], &buffer[0], buffLen);
	return ret;
}
int client()
{
	const char * servername = "localhost";

	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;



	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(servername, DEFAULT_SERVER_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
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
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// Send an initial buffer
	RDT_Header first;
	first.seqNum = 0;
	first.ackNum = 3;
	first.fin = 0;
	first.len = 0;
	first.data = "main.cpp";
	iResult = send(ConnectSocket, first.toString().c_str(), first.toString().length(), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	//printf("Bytes Sent: %ld\n", iResult);


	Client client;
	std::ofstream outfile;

	outfile.open("test.txt", std::ios_base::app);

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		outmut.lock();
		std::cout << "Sever Sent:" << std::endl;
		RDT_Header(bufferToString(recvbuf, iResult)).printReadable();
		outmut.unlock();
		RDT_Header last(bufferToString(recvbuf, iResult));
		outfile << last.data;
		if (iResult > 0 && !last.fin)
		{


			std::vector<RDT_Header> toSend = client.nextPacket(last);
			if (toSend.size() > 0)
			{
				iResult = send(ConnectSocket, toSend[0].toString().c_str(), toSend[0].toString().length(), 0);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}
			}
			//	printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0 || last.fin)
		{
			RDT_Header l;
			l.fin = 1;
			l.len = 0;
			iResult = send(ConnectSocket, l.toString().c_str(), l.toString().length(), 0);
			printf("Client () Connection closed\n");
			break;
		}
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}
int server()
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_SERVER_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// No longer need server socket
	closesocket(ListenSocket);



	// Receive the first request and build a server object
	Server server;
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		//printf("Bytes received: %d\n", iResult);
		RDT_Header first(bufferToString(recvbuf, iResult));
		/*	first.data = "main.cpp";
			first.seqNum = 1;
			first.fin = 0;
			first.ackNum = 3;
			first.len = first.data.length();*/
		server = Server(first.data, 16);
		std::vector<RDT_Header> response = server.nextPackets(first);
		iSendResult = send(ClientSocket, response[0].toString().c_str(), response[0].toString().length(), 0);
		outmut.lock();
		std::cout << "Client Sent:" << std::endl;
		RDT_Header(bufferToString(recvbuf, iResult)).printReadable();
		outmut.unlock();
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		//	printf("Bytes sent: %d\n", iSendResult);
	}
	else
	{
		return 1;
	}

	// Receive until the peer shuts down the connection
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		// Parse into useful format
		RDT_Header last(bufferToString(recvbuf, iResult));

		if (iResult > 0 && !last.fin) {
			//	printf("Bytes received: %d\n", iResult);

			std::vector<RDT_Header> responses = server.nextPackets(last);

			// Echo the buffer back to the sender
			if (responses.size() > 0)
			{
				iSendResult = send(ClientSocket, responses[0].toString().c_str(), responses[0].toString().length(), 0);
				outmut.lock();
				std::cout << "Client Sent:" << std::endl;
				RDT_Header(bufferToString(recvbuf, iResult)).printReadable();
				outmut.unlock();
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
			}
			//	printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0 || last.fin)
		{
			RDT_Header l;
			l.fin = 1;
			iSendResult = send(ClientSocket, l.toString().c_str(), l.toString().length(), 0);
			printf("Server() Connection closing...\n");
			break;
		}
		else  {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}
