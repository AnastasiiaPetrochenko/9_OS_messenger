#include "client.h"

Client::Client(const QString &name) : QObject(), connection(INVALID_SOCKET), name(name)
{
    iMsgSocket = new MessageData<>();
}

Client::~Client()
{
    delete iMsgSocket;
    CloseSocket();
}

bool Client::OpenSocket(const char *ipAddress, int port)
{
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);

    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        return false;
    }

    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr(ipAddress);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    connection = socket(AF_INET, SOCK_STREAM, 0);
    if (connection == INVALID_SOCKET || ::connect(connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    {
        closesocket(connection);
        connection = INVALID_SOCKET;
        return false;
    }
    return true;
}

void Client::CloseSocket()
{
    if (connection != INVALID_SOCKET)
    {
        shutdown(connection, SD_BOTH);
        closesocket(connection);
        connection = INVALID_SOCKET;
    }
}

int Client::SendSocket(MessageData<> &msg)
{
    if (connection == INVALID_SOCKET)
        return SOCKET_ERROR;

    int result = send(connection, (char*)&msg, sizeof(msg), 0);
    return result == SOCKET_ERROR ? SOCKET_ERROR : 0;
}

bool Client::ReceiveSocket(MessageData<> *&msg)
{
    if (connection == INVALID_SOCKET)
        return false;

    int result = recv(connection, (char*)iMsgSocket, sizeof(*iMsgSocket), 0);
    if (result == SOCKET_ERROR || result == 0)
        return false;

    msg = iMsgSocket;
    emit NewMessage();
    return true;
}
