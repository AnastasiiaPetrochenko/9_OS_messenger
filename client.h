#ifndef CLIENT_H
#define CLIENT_H

#include "datalib.h"

#include <winsock2.h>
#include <iostream>

#include <Ws2tcpip.h>
#include <winsock.h>
#include <winsock2.h>
#include <windows.h>

#include <QObject>
#include <QString>
#include <QQueue>

#define MAILSLOT_SERVER_NAME "\\\\.\\Mailslot\\Server"
#define LOCAL_IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 1111

class Client : public QObject
{
    Q_OBJECT
private:
    MessageData<> *iMsgSocket;  // залишаємо лише сокетне повідомлення

    QQueue<MessageData<>> inputMessages;  // черга повідомлень
    SOCKET connection;  // сокет для зв'язку
    QString name;  // ім'я клієнта

public:
    Client(const QString &name);
    ~Client();

    QString getName() const { return name; }

    // Відкриває сокет з заданими параметрами
    bool OpenSocket(const char *ipAddress = LOCAL_IP_ADDRESS, int port = DEFAULT_PORT);

    // Закриває сокет
    void CloseSocket();

    // Відправляє повідомлення через сокет
    int SendSocketMessage(MessageData<> &msg);

    // Отримує повідомлення через сокет
    bool ReceiveSocketMessage(MessageData<> *&msg);

signals:
    // Сигнал про отримання нового повідомлення
    void NewMessage();
};

#endif // CLIENT_H
