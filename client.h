#ifndef CLIENT_H
#define CLIENT_H

#include "datalib.h"
#include <winsock2.h>
#include <iostream>
#include <Ws2tcpip.h>
#include <windows.h>
#include <QObject>
#include <QString>
#include <QQueue>

#define LOCAL_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 1111

class Client : public QObject
{
    Q_OBJECT
private:
    MessageData<> *iMsgSocket;  // Used only for socket communication

    QQueue<MessageData<>> inputMessages;  // Queue for incoming messages
    SOCKET connection;  // Socket for communication
    id_t id;  // Client ID
    QString name;  // Client's name

public:
    Client(id_t id, const QString &name);
    ~Client();

    inline void SetId(id_t id) { this->id = id; }
    inline id_t GetId() const { return id; }
    inline QString GetName() const { return name; }

    // Take input message from queue (returns true if queue contains message)
    bool GetInputMessage(MessageData<> *&msg);

    // Returns true if text contains flood (repeating text)
    static bool ContainsFlood(const QString &text);

    // Opens socket with given address and port
    bool OpenSocket(const char *ipAddress = LOCAL_IP_ADDRESS, unsigned short port = DEFAULT_PORT);

    // Closes the socket
    void CloseSocket();

    // Receives a message from the socket
    bool ReceiveSocket(MessageData<> *&msg);

    // Sends a message over the socket
    int SendSocket(MessageData<> &msg);

signals:
    // Signal to inform that a new message is received
    void NewMessage();
};

#endif // CLIENT_H
