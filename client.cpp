// client.cpp

#include "client.h"
#include <QMessageBox>

Client::Client()
{
    this->connection = NULL;
}

Client::Client(QListWidgetItem *listItem, const QString &name, QTextEdit *textEdit, SOCKET socket)
{
    this->listItem = listItem;
    this->name = name;
    this->textEdit = textEdit;
    this->connection = socket;
}

Client::~Client()
{
    CloseSocket();
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

bool Client::ReceiveSocket(MessageData<> *input)
{
    return recv(connection, (char*)input, sizeof(*input), 0) != SOCKET_ERROR;
}

bool Client::SendSocket(const MessageData<> *output)
{
    return send(connection, (char*)output, sizeof(*output), 0) != SOCKET_ERROR;
}
