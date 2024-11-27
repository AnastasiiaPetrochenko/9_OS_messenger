#include "client.h"



Client::Client()
    : QObject()
{
    this->connection      = NULL;
}

int Client::SendSocketMessage(std::string message){

    int res = send(connection, message.c_str(), message.size(), 0) == SOCKET_ERROR;
    return res;


}

bool Client::OpenSocket(const char *ip, int port)
{
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);

    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        return false;
    }


    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    connection = socket(AF_INET, SOCK_STREAM, 0);

    int code = ::connect(connection, (SOCKADDR*)&addr, sizeof(addr));
    if (code)
    {
        closesocket(connection);
        connection = NULL;
        return false;
    }
    return true;
}
void Client::CloseSocket()
{
    if (connection)
    {
        shutdown(connection, SD_BOTH);
        closesocket(connection);
        connection = NULL;
    }
}
