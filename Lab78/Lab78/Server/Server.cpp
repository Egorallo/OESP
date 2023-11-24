#include <Winsock2.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")

constexpr int DEFAULT_PORT = 12345;

struct ClientInfo {
    SOCKET socket;
    std::string clientId;
};

std::vector<ClientInfo> clients;


std::string GenerateClientId() {
    auto now = std::time(nullptr);

    // Используем текущее время для создания уникального идентификатора
    std::srand(static_cast<unsigned>(now));
    int randomNum = std::rand();

    return std::to_string(randomNum);
}


void HandleClient(ClientInfo clientInfo) {
    char buffer[2056];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(clientInfo.socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            std::cerr << "Client " << clientInfo.clientId <<  " disconnected" << std::endl;
            closesocket(clientInfo.socket);
            return;
        }

        buffer[bytesReceived] = '\0'; // Добавим нулевой символ для корректного вывода строки

        std::cout << "Received from client " << clientInfo.clientId << ": " << buffer << std::endl;

        for (const auto& client : clients) {

            std::string messageWithId = clientInfo.clientId + ": " + buffer;
            int bytesSent = send(client.socket, messageWithId.c_str(), messageWithId.size(), 0);

            if (bytesSent == SOCKET_ERROR)
                std::cout << "Error sending message to client" << ".\n";
        }
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind server socket" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error in listen function" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port " << DEFAULT_PORT << "..." << std::endl;

    std::vector<std::thread> clientThreads;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept client connection" << std::endl;
            continue;
        }

        std::string clientId = GenerateClientId();

        std::cout << "Client connected: " << clientId <<  std::endl;

        clients.push_back({ clientSocket, clientId});

        std::thread(HandleClient, clients.back()).detach();

    }

    // Дожидаемся завершения всех потоков
    for (auto& thread : clientThreads) {
        thread.join();
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
