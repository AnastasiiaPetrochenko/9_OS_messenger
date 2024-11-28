#ifndef CLIENT_H
#define CLIENT_H

#include "../DataLib/datalib.h"

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

#define MAILSLOT_SERVER_NAME "\\\\.\\Mailslot\\Server"
#define LOCAL_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 1111

#define FLOOD_COUNT 10              // Count of characters which are recognized as flood
#define FLOOD_MAX_FRAGMENT_SIZE 4   // Maximum count of characters in repeating fragment

class Client : public QObject
{
    Q_OBJECT
private:
    MessageData<> *iMsgMailslot, *iMsgSocket;

    QQueue<MessageData<>> inputMessages;
    SOCKET connection;
    HANDLE hMailslotOutput;
    HANDLE hMailslotInput;
    HANDLE hQueueMutex;
    QString mailslotOutputName;
    QString mailslotInputName;
    id_t id;
    QString name;

public:
    Client(id_t id, const QString &name);
    Client(const Client &other);
    Client &operator=(const Client &other);
    ~Client();

    inline void SetId(id_t id) { this->id = id; }
    inline id_t GetId() const { return id; }
    inline QString GetName() const { return name; }
    inline QString GetMailslotOutputName() const { return mailslotOutputName; }
    inline QString GetMailslotInputName() const { return mailslotInputName; }

    // Take input message from queue (returns true if queue contains message)
    bool GetInputMessage(MessageData<> *&msg);

    // Returns true if text contains flood (repeating text)
    static bool ContainsFlood(const QString &text);

    // Returns true if socket was connected successfully
    bool OpenSocket(const char *ipAddress = LOCAL_IP_ADDRESS,
                    unsigned short port = DEFAULT_PORT);

    // Close actual socket. If socket isn't connected, it will be ignored
    void CloseSocket();

    // Returns true if input and output mailslots was successfully configured
    bool OpenMailslots();

    // Close input and ouput mailslots. If mailslots aren't connected, it will be ignored
    void CloseMailslots();

    // Receive message by mailslot, increment spinlock
    // and emit NewMessage() signal (if received successfully)
    bool ReceiveMailslot(volatile unsigned long *spinlock);

    // Receive message by mailslot and emit NewMessage() signal
    // (if received successfully)
    bool ReceiveMailslot();

    // Send message oMsg by mailslot
    bool SendMailslot(MessageData<> &msg);

    // Receive message by socket, increment spinlock
    // and emit NewMessage() signal (if received successfully)
    bool ReceiveSocket(volatile unsigned long *spinlock);

    // Receive message by socket and emit NewMessage() signal
    // (if received successfully)
    bool ReceiveSocket();

    // Send message oMsg by socket
    bool SendSocket(MessageData<> &msg);

signals:
    // Signal to inform about adding new message to queue
    void NewMessage();
};

#endif // CLIENT_H
