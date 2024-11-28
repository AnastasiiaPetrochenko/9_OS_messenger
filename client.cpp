#include "client.h"
#include <QMessageBox>

const QString Client::CLIENT_PATH =
    "..\\build-MessengerClient-Desktop_Qt_5_14_1_MinGW_32_bit-Debug\\MessengerClient.exe";
id_t Client::idCounter = 1;
HANDLE Client::hMailslotInput = INVALID_HANDLE_VALUE;

Client::Client()
{
    this->listItem = nullptr;
    this->textEdit = nullptr;
    this->hMailslotOutput = NULL;
    this->hThread = NULL;
}

Client::Client(QListWidgetItem *listItem, const QString &name, QTextEdit *textEdit, SOCKET socket)
{
    this->listItem = listItem;
    this->name = name;
    this->textEdit = textEdit;
    this->connection = socket;
    this->hMailslotOutput = NULL;
    this->hThread = NULL;
}

Client::Client(const Client &other)
    : QObject()
{
    this->listItem = other.listItem;
    this->name = other.name;
    DuplicateHandle(GetCurrentProcess(),
                    other.hMailslotOutput,
                    GetCurrentProcess(),
                    &this->hMailslotOutput,
                    0,
                    FALSE,
                    DUPLICATE_SAME_ACCESS);
    this->connection = other.connection;
    this->hThread = NULL;
    if (other.textEdit == nullptr)
    {
        this->textEdit = nullptr;
    }
    else
    {
        this->textEdit = new QTextEdit();
        this->textEdit->setFont(other.textEdit->font());
        this->textEdit->setReadOnly(other.textEdit->isReadOnly());
    }
    if (other.listItem == nullptr)
    {
        this->listItem = nullptr;
    }
    else
    {
        this->listItem = new QListWidgetItem(*other.listItem);
    }
}

Client &Client::operator=(const Client &other)
{
    this->name = other.name;
    DuplicateHandle(GetCurrentProcess(),
                    other.hMailslotOutput,
                    GetCurrentProcess(),
                    &this->hMailslotOutput,
                    0,
                    FALSE,
                    DUPLICATE_SAME_ACCESS);
    this->connection = other.connection;
    this->hThread = NULL;
    if (other.textEdit == nullptr)
    {
        this->textEdit = nullptr;
    }
    else
    {
        delete this->textEdit;
        this->textEdit = new QTextEdit();
        this->textEdit->setFont(other.textEdit->font());
        this->textEdit->setReadOnly(other.textEdit->isReadOnly());
    }
    if (other.listItem == nullptr)
    {
        this->listItem = nullptr;
    }
    else
    {
        if (this->listItem != nullptr)
        {
            delete this->listItem;
        }
        this->listItem = new QListWidgetItem(*other.listItem);
    }
    StopThread();
    return *this;
}

Client::~Client()
{
    if (textEdit != nullptr) delete textEdit;
    if (listItem != nullptr) delete listItem;
    StopThread();
}

id_t Client::GetNewId()
{
    ++idCounter;
    return idCounter - 1;
}

void Client::OpenServerMailslot()
{
    if ((hMailslotInput = CreateMailslotA(MAILSLOT_SERVER_NAME,
                                          0,
                                          0,
                                          NULL))
        == INVALID_HANDLE_VALUE)
    {
        throw QString("Cannot create server mailslot");
    }
}

void Client::CloseServerMailslot()
{
    CloseHandle(hMailslotInput);
}

void Client::OpenMailslot(id_t id)
{
    if ((this->hMailslotOutput = CreateFileA(("\\\\.\\Mailslot\\" + QString::number(id)).toLocal8Bit(),
                                             GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                             NULL)) == INVALID_HANDLE_VALUE)
    {
        throw QString("Cannot connect to mailslot");
    }
}

void Client::CloseMailslot()
{
    if (hMailslotOutput != NULL)
    {
        CloseHandle(hMailslotOutput);
        hMailslotOutput = NULL;
    }
}

void Client::OpenSocket()
{
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        throw QString("WSAStartup failed");
    }

    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, 0);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    while ((connection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr)) == INVALID_SOCKET) {}
    emit NewSocket();
}

void Client::CloseSocket()
{
    if (connection != NULL)
    {
        shutdown(connection, SD_BOTH);
        closesocket(connection);
        connection = NULL;
    }
}

void Client::AppendToChat(const QString &senderName, const QString &message)
{
    textEdit->append("From " + senderName + ": " + message);
}

bool Client::Send(Client::ConnectionType type, const MessageData<> *output)
{
    switch (type)
    {
    case MAILSLOT_CONNECTION:
        return SendMailslot(output);
    case SOCKET_CONNECTION:
        return SendSocket(output);
    default:
        return SendMailslot(output);
    }
}

bool Client::ReceiveMailslot(MessageData<> *input)
{
    DWORD bytesRead;
    return ReadFile(hMailslotInput,
                    (char*)input,
                    sizeof(*input),
                    &bytesRead,
                    NULL);
}

bool Client::SendMailslot(const MessageData<> *output)
{
    DWORD bytesWritten;
    return WriteFile(hMailslotOutput,
                     (char*)output,
                     sizeof(*output),
                     &bytesWritten,
                     NULL);
}

void Client::StartThread(void (*func)(void*), const QObject *receiver, const char *member)
{
    connect(this, SIGNAL(NewMessage()), receiver, member);
    hThread = (HANDLE)_beginthread(func, 0, this);
}

void Client::StopThread()
{
    disconnect(this, SIGNAL(NewMessage()));
    if (hThread != NULL)
    {
        WaitForSingleObject(hThread, INFINITE);
        //CloseHandle(hThread);
        hThread = NULL;
    }
}

bool Client::ReceiveSocket(MessageData<> *input)
{
    return recv(connection,
                (char*)input,
                sizeof(*input),
                0) != SOCKET_ERROR;
}

bool Client::SendSocket(const MessageData<> *output)
{
    return send(connection,
                (char*)output,
                sizeof(*output),
                0) != SOCKET_ERROR;
}
