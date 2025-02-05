#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <thread>
#include <vector>

#pragma comment(lib, "ws2_32.lib")


bool Initialize() {
	WSADATA data;

	return WSAStartup(MAKEWORD(2, 2), &data) == 0;

}

void InteractWithClient(SOCKET clientSocket, std::vector<SOCKET>& clients) {
	//send / recv client
	
	std::cout << "Clinet connected " << std::endl;
	char buffer[4096];
	
	while (1) {
		int bytesrcved = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesrcved <= 0) {
			std::cout << "Client disconnected " << std::endl;
			break; 
		}

		std::string message(buffer, bytesrcved);
		std::cout << "Message from client : " << message << std::endl;

		for (auto client : clients) {
			if (client != clientSocket) {
				send(client, message.c_str(), message.length(), 0);
			}	
		}
	}

	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}

	closesocket(clientSocket);

}

int main() {

	if (!Initialize()) {
		std::cout << "winsock initialization failed " << std::endl;
		return 1;
	}

	std::cout << "server program " << std::endl;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET) {
		std::cout << "socket creation failed " << std::endl;
		return 1;
	 }

	//create address structure
	int port = 12345;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	//convert the ip address (0.0.0.0) put it inside teh sin_family in binary format
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		std::cout << "setting address structure failed " << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}


	//bind
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		std::cout << "bind failed " << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//listen 
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "listen failed " << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Server has started listening on port : " << port << std::endl;
	std::vector<SOCKET> clients;


	while(1){
		//accept
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			std::cout << "invalid client socket " << std::endl;
		}

		clients.push_back(clientSocket);
		std::thread t1(InteractWithClient, clientSocket, std::ref(clients));
		t1.detach();
	}

	closesocket(listenSocket);



	WSACleanup();
	return 0;
}



