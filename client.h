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

#define MAILSLOT_SERVER_NAME "\\\\.\\Mailslot\\Server"
#define LOCAL_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 1111

#define FLOOD_COUNT 10              // Count of characters which are recognized as flood
#define FLOOD_MAX_FRAGMENT_SIZE 4   // Maximum count of characters in repeating fragment

class Client : public QObject
{
    Q_OBJECT
private:
    MessageData<> *iMsgMailslot, *iMsgSocket;  // Used for both mailslot and socket communication
    QQueue<MessageData<>> inputMessages;  // Queue to store incoming messages
    SOCKET connection;  // Socket for communication
    HANDLE hMailslotOutput;  // Handle for output mailslot
    HANDLE hMailslotInput;  // Handle for input mailslot
    HANDLE hQueueMutex;  // Mutex for synchronizing access to message queue
    QString mailslotOutputName;  // Name of the output mailslot
    QString mailslotInputName;  // Name of the input mailslot
    id_t id;  // Client ID
    QString name;  // Client's name
    bool isSocketOpen;  // Flag to track socket connection status
    bool isMailslotOpen;  // Flag to track mailslot connection status

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

    // Socket-related methods
    bool OpenSocket(const char *ipAddress = LOCAL_IP_ADDRESS, unsigned short port = DEFAULT_PORT);
    void CloseSocket();
    bool ReceiveSocket(MessageData<> *&msg);
    int SendSocket(MessageData<> &msg);

    // Mailslot-related methods
    bool OpenMailslots();  // Open input and output mailslots
    void CloseMailslots();  // Close input and output mailslots
    bool ReceiveMailslot();  // Receive message from mailslot
    bool SendMailslot(MessageData<> &msg);  // Send message via mailslot

    // Method to handle socket or mailslot communication
    bool SendMessage(MessageData<> &msg, bool isMailslot);

    // Check if a specific Mailslot exists
    bool CheckMailslotExistence(const QString &mailslotName);

    // Method to handle received messages
    void ProcessReceivedMessage(const MessageData<> &msg);

signals:
    // Signal to inform about adding new message to queue
    void NewMessage();
};

#endif // CLIENT_H
