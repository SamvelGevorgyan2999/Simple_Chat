#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

/*
	Initialize winsock

	create socket

	connect to the server

	send/recv

	close the socket

*/



bool Initaialize() {

	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void Send_Message(SOCKET s) {
	std::cout << "Enter your chat name: " << std::endl;
	std::string name;
	std::getline(std::cin, name);
	std::string message;

	while (1) {
		std::getline(std::cin, message);
		std::string msg = name + " : " + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR) {
			std::cout << "Error sending message " << std::endl;
			break;
		}

		if (message == "quit") {
			std::cout << "Stopping the application " << std::endl;
			break;
		}

	}

	closesocket(s);
	WSACleanup();
}

void ReceiveMessage(SOCKET s) {
	char buffer[4096];
	int recvlength;
	std::string msg = "";
	while (1) {
		recvlength = recv(s, buffer, sizeof(buffer), 0);
		if (recvlength <= 0) {
			std::cout << "Disconnected from server " << std::endl;
			break;
		}
		else {
			msg = std::string(buffer, recvlength);
			std::cout << msg << std::endl;
		}
	}

	closesocket(s);
	WSACleanup();
}

int main() {

	if (!Initaialize()) {
		std::cout << "Initialize winsock failed " << std::endl;
		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		std::cout << "invalid socket created " << std::endl;
		return 1;
	}
	int port = 12345;
	std::string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		std::cout << "Not able to connect to server " << std::endl;
		std::cout << ": " << WSAGetLastError();
		closesocket(s);
		WSACleanup();
		return 1;
	}

	std::cout << "Successfully connected to server " << std::endl;

	std::thread senderthread(Send_Message, s);
	std::thread receiverthread(ReceiveMessage, s);

	senderthread.join();
	receiverthread.join();

	return 0;
}