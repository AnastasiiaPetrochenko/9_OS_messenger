#ifndef CLIENT_H
#define CLIENT_H

#include "datalib.h"
#include <windows.h>
#include <Ws2tcpip.h>
#include <winsock.h>
#include <winsock2.h>


#include <QObject>
#include <QString>

#define ADDRESS "127.0.0.1"
#define PORT 1111

class Client : public QObject
{
    Q_OBJECT
    SOCKET connection;

public:
    Client();

    void SetSocket(SOCKET &socket) { connection = socket; }
    SOCKET GetSocket() const { return connection; }

    SOCKET OpenSocket();
    void CloseSocket();
};

#endif // CLIENT_H
