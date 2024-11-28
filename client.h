#ifndef CLIENT_H
#define CLIENT_H

#include "datalib.h"

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>

#include <QTextEdit>
#include <QListWidgetItem>

#define MAILSLOT_SERVER_NAME "\\\\.\\Mailslot\\Server"
#define IP_ADDRESS "127.0.0.1"
#define PORT 1111

class Client : public QObject
{
    Q_OBJECT

public:
    Client();
    Client(QListWidgetItem *listItem, const QString &name, QTextEdit *textEdit, SOCKET socket);
    ~Client();

    void OpenSocket();
    void CloseSocket();
    bool ReceiveSocket(MessageData<> *input);
    bool SendSocket(const MessageData<> *output);

signals:
    void NewSocket();

private:
    QListWidgetItem *listItem;
    QString name;
    QTextEdit *textEdit;
    SOCKET connection;
};

#endif // CLIENT_H
