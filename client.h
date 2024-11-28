#ifndef CLIENT_H
#define CLIENT_H

#include "datalib.h"

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>

#include <QTextEdit>
#include <QListWidgetItem>

#define IP_ADDRESS "127.0.0.1"
#define PORT 1111

class Client : public QObject
{
    Q_OBJECT
private:
    QString name;
    SOCKET connection;
    QTextEdit *textEdit;
    QListWidgetItem *listItem;

public:
    enum ConnectionType
    {
        SOCKET_CONNECTION
    };

    Client();
    Client(QListWidgetItem *listItem, const QString &name, QTextEdit *textEdit, SOCKET socket = 0);
    ~Client();

    inline void SetListItem(QListWidgetItem *listItem) { this->listItem = listItem; }
    inline QListWidgetItem *GetListItem() const { return listItem; }

    inline void SetSocket(SOCKET &socket) { connection = socket; }
    inline SOCKET GetSocket() const { return connection; }

    void CloseSocket();

    bool Send(ConnectionType type, const MessageData<> *output);
    bool ReceiveSocket(MessageData<> *input);
    bool SendSocket(const MessageData<> *output);

public slots:
    void OpenSocket();

signals:
    void NewMessage();
    void NewSocket();
};

#endif // CLIENT_H
