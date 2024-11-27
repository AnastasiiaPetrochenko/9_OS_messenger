#ifndef CLIENT_H
#define CLIENT_H

#include "datalib.h"
#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <QObject>
#include <QString>

#define MAILSLOT_SERVER_NAME "\\\\.\\Mailslot\\Server"
#define LOCAL_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 1111

class Client : public QObject
{
    Q_OBJECT

    SOCKET connection;

public:
    Client();

    bool OpenSocket(const char *ip = LOCAL_IP_ADDRESS, int port = DEFAULT_PORT);
    void CloseSocket();

    int SendSocketMessage(std::string message);
};

#endif // CLIENT_H
