#include "client.h"
#include <QMessageBox>


id_t Client::idCounter = 1;

Client::Client()
{
    this->connection = NULL;
}

Client::Client(const QString &name, SOCKET socket)
{
    this->name = name;
    this->connection = socket;
}

Client::Client(const Client &other)
    : QObject()
{
    this->name = other.name;
    this->connection = other.connection;
}

Client &Client::operator=(const Client &other)
{
    this->name = other.name;
    this->connection = other.connection;
    return *this;
}

Client::~Client()
{
    CloseSocket();
}

id_t Client::GetNewId()
{
    ++idCounter;
    return idCounter - 1;
}

void Client::OpenSocket()
{
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        throw QString("WSAStartup failed");
    }

    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, 0);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    while ((connection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr)) == INVALID_SOCKET) {}
    emit NewSocket();
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

bool Client::Send(Client::ConnectionType type, const MessageData<> *output)
{
    switch (type)
    {
    case SOCKET_CONNECTION:
        return SendSocket(output);
    default:
        return false;
    }
}

bool Client::ReceiveSocket(MessageData<> *input)
{
    return recv(connection,
                (char*)input,
                sizeof(*input),
                0) != SOCKET_ERROR;
}

bool Client::SendSocket(const MessageData<> *output)
{
    return send(connection,
                (char*)output,
                sizeof(*output),
                0) != SOCKET_ERROR;
}
