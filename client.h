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

#include <QTextEdit>
#include <QListWidgetItem>

#define MAILSLOT_SERVER_NAME "\\\\.\\Mailslot\\Server"
#define IP_ADDRESS "127.0.0.1"
#define PORT 1111

class Client : public QObject
{
    Q_OBJECT
private:
    static id_t idCounter;
    static HANDLE hMailslotInput;

    QString name;
    HANDLE hMailslotOutput;
    HANDLE hThread;
    SOCKET connection;
    QTextEdit *textEdit;
    QListWidgetItem *listItem;

public:
    enum ConnectionType
    {
        MAILSLOT_CONNECTION,
        SOCKET_CONNECTION
    };
    static const QString CLIENT_PATH;

    Client();
    Client(QListWidgetItem *listItem, const QString &name, QTextEdit *textEdit, SOCKET socket = 0);
    Client(const Client &other);
    Client &operator=(const Client &other);
    ~Client();

    inline void SetListItem(QListWidgetItem *listItem) { this->listItem = listItem; }
    inline QListWidgetItem *GetListItem() const { return listItem; }

    inline void SetName(const QString &name) { this->name = name; }
    inline QString GetName() const { return name; }

    inline void SetTextEdit(QTextEdit *textEdit) { this->textEdit = textEdit; }
    inline QTextEdit *GetTextEdit() const { return textEdit; }

    inline void SetSocket(SOCKET &socket) { connection = socket; }
    inline SOCKET GetSocket() const { return connection; }

    static id_t GetNewId();

    static void OpenServerMailslot();
    static void CloseServerMailslot();
    void OpenMailslot(id_t id);
    void CloseMailslot();

    void CloseSocket();

    void AppendToChat(const QString &senderName, const QString &message);

    bool Send(ConnectionType type, const MessageData<> *output);

    static bool ReceiveMailslot(MessageData<> *input);
    bool SendMailslot(const MessageData<> *output);

    void StartThread(void (*func)(void*), const QObject *receiver, const char *member);
    void StopThread();
    bool ReceiveSocket(MessageData<> *input);
    bool SendSocket(const MessageData<> *output);

public slots:
    void OpenSocket();

signals:
    void NewMessage();
    void NewSocket();
};

#endif // CLIENT_H
