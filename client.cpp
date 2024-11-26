#include "client.h"

Client::Client()
{
    this->connection = NULL;
}

void Client::CloseSocket()
{
    if (connection != NULL)
    {
        shutdown(connection, SD_BOTH);
        closesocket(connection);
        connection = NULL;
    }
}

SOCKET Client::OpenSocket()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        return NULL;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ADDRESS); // Приймає підключення з будь-якої адреси
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        return NULL;
    }

    return serverSocket;
}



