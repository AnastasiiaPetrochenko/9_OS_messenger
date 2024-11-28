#ifndef CLIENT_H
#define CLIENT_H

#include "datalib.h"

#include <winsock2.h>
#include <iostream>

#include <Ws2tcpip.h>
#include <winsock.h>
#include <winsock2.h>
#include <w32api.h>
#include <winnetwk.h>
#include <ws2spi.h>
#include <wtsapi32.h>
#include <windows.h>

#include <QObject>
#include <QString>
#include <QQueue>

#define LOCAL_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 1111

#define FLOOD_COUNT 10              // Count of characters which are recognized as flood
#define FLOOD_MAX_FRAGMENT_SIZE 4   // Maximum count of characters in repeating fragment

class Client : public QObject
{
    Q_OBJECT
private:
    MessageData<> *iMsgSocket;  // Used only for socket communication

    QQueue<MessageData<>> inputMessages;  // Queue for incoming messages
    SOCKET connection;  // Socket for communication
    id_t id;  // Client ID
    QString name;  // Client's name

    // Mailslot-related variables

public:
    Client(id_t id, const QString &name);
    Client(const Client &other);
    Client &operator=(const Client &other);
    ~Client();

    inline void SetId(id_t id) { this->id = id; }
    inline id_t GetId() const { return id; }
    inline QString GetName() const { return name; }

    // Take input message from queue
    bool GetInputMessage(MessageData<> *&msg);

    // Returns true if text contains flood (repeating text)
    static bool ContainsFlood(const QString &text);

    // Socket-related methods
    bool OpenSocket(const char *ipAddress = LOCAL_IP_ADDRESS, unsigned short port = DEFAULT_PORT);
    void CloseSocket();
    bool ReceiveSocket(MessageData<> *&msg);
    int SendSocket(MessageData<> &msg);

    // Mailslot-related methods

signals:
    // Signal to inform about adding new message to queue
    void NewMessage();
};

#endif // CLIENT_H
