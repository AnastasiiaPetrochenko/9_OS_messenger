#include "client.h"

Client::Client(id_t id, const QString &name)
    : QObject()
{
    this->id              = id;
    this->name            = name;
    this->iMsgMailslot    = new MessageData<>();
    this->iMsgSocket      = new MessageData<>();
    this->connection      = NULL;
    this->hQueueMutex     = CreateMutexA(0, false, 0);
    this->hMailslotInput  = NULL;
    this->hMailslotOutput = NULL;
}

Client::Client(const Client &other)
    : QObject()
{
    DuplicateHandle(GetCurrentProcess(),
                    other.hMailslotInput,
                    GetCurrentProcess(),
                    &this->hMailslotInput,
                    0,
                    FALSE,
                    DUPLICATE_SAME_ACCESS);
    DuplicateHandle(GetCurrentProcess(),
                    other.hMailslotOutput,
                    GetCurrentProcess(),
                    &this->hMailslotOutput,
                    0,
                    FALSE,
                    DUPLICATE_SAME_ACCESS);
    this->id              = other.id;
    this->name            = other.name;
    this->hQueueMutex     = CreateMutexA(0, false, 0);
    this->iMsgMailslot    = new MessageData<>(*other.iMsgMailslot);
    this->iMsgSocket      = new MessageData<>(*other.iMsgSocket);
    this->connection      = other.connection;
    this->inputMessages   = other.inputMessages;
}

Client &Client::operator=(const Client &other)
{
    DuplicateHandle(GetCurrentProcess(),
                    other.hMailslotInput,
                    GetCurrentProcess(),
                    &this->hMailslotInput,
                    0,
                    FALSE,
                    DUPLICATE_SAME_ACCESS);
    DuplicateHandle(GetCurrentProcess(),
                    other.hMailslotOutput,
                    GetCurrentProcess(),
                    &this->hMailslotOutput,
                    0,
                    FALSE,
                    DUPLICATE_SAME_ACCESS);
    this->id            = other.id;
    this->name          = other.name;
    this->hQueueMutex   = CreateMutexA(0, false, 0);
    *this->iMsgMailslot = *other.iMsgMailslot;
    *this->iMsgSocket   = *other.iMsgSocket;
    this->connection    = other.connection;
    this->inputMessages = other.inputMessages;
    return *this;
}

Client::~Client()
{
    CloseHandle(hQueueMutex);

    delete iMsgMailslot;
    delete iMsgSocket;
}

bool Client::GetInputMessage(MessageData<> *&msg)
{
    WaitForSingleObject(hQueueMutex, INFINITE);
    if (inputMessages.isEmpty())
    {
        ReleaseMutex(hQueueMutex);
        return false;
    }
    msg = new MessageData<>(inputMessages.front());
    inputMessages.pop_front();
    ReleaseMutex(hQueueMutex);
    return true;
}

bool Client::ContainsFlood(const QString &text)
{
    int size = text.size();

    for (int i = 1; i <= FLOOD_MAX_FRAGMENT_SIZE; ++i)
    {
        int repIndex = 0;
        int counter = i;
        int offset = 0;

        for (int j = i; j < size && repIndex + offset < size; ++j)
        {
            if (text[j] == text[repIndex + offset])
            {
                if ((++counter) == FLOOD_COUNT) return true;
                if ((++offset) % i == 0) offset = 0;
            }
            else
            {
                j = (++repIndex) + i - 1;
                counter = i;
                offset = 0;
            }
        }
    }
    return false;
}

bool Client::OpenSocket(const char *ipAddress,
                        unsigned short port)
{
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);

    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        return false;
    }

    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr(ipAddress);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    connection = socket(AF_INET, SOCK_STREAM, 0);
    if (::connect(connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    {
        closesocket(connection);
        connection = NULL;
        return false;
    }
    return true;
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

bool Client::OpenMailslots()
{
    mailslotOutputName = QString(MAILSLOT_SERVER_NAME);
    mailslotInputName = "\\\\.\\Mailslot\\" + QString::number(id);

    if ((this->hMailslotOutput = CreateFileA(this->mailslotOutputName.toLocal8Bit(),
                                             GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                             NULL)) == INVALID_HANDLE_VALUE)
    {
        this->hMailslotOutput = NULL;
        return false;
    }

    if ((this->hMailslotInput = CreateMailslotA(this->mailslotInputName.toLocal8Bit(),
                                                0,
                                                0,
                                                NULL)) == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hMailslotOutput);
        hMailslotOutput = NULL;
        hMailslotInput = NULL;
        return false;
    }
    return true;
}

void Client::CloseMailslots()
{
    if (hMailslotInput != NULL)
    {
        CloseHandle(hMailslotInput);
        hMailslotInput = NULL;
    }

    if (hMailslotOutput != NULL)
    {
        CloseHandle(hMailslotOutput);
        hMailslotOutput = NULL;
    }
}

bool Client::ReceiveMailslot(volatile unsigned long *spinlock)
{
    if (hMailslotInput == NULL) return false;
    DWORD bytesRead;
    if (ReadFile(hMailslotInput,
                 (char*)iMsgMailslot,
                 sizeof(*iMsgMailslot),
                 &bytesRead,
                 NULL))
    {
        WaitForSingleObject(hQueueMutex, INFINITE);
        inputMessages.push_back(*iMsgMailslot);
        ReleaseMutex(hQueueMutex);

        InterlockedIncrement(spinlock);
        NewMessage();
        return true;
    }
    return false;
}

bool Client::ReceiveMailslot()
{
    if (hMailslotInput == NULL) return false;
    DWORD bytesRead;
    if (ReadFile(hMailslotInput,
                 (char*)iMsgMailslot,
                 sizeof(*iMsgMailslot),
                 &bytesRead,
                 NULL))
    {
        WaitForSingleObject(hQueueMutex, INFINITE);
        inputMessages.push_back(*iMsgMailslot);
        ReleaseMutex(hQueueMutex);
        NewMessage();
        return true;
    }
    return false;
}

bool Client::SendMailslot(MessageData<> &msg)
{
    if (hMailslotOutput == NULL) return false;
    DWORD bytesWritten;
    bool ret = WriteFile(hMailslotOutput,
                         (char*)&msg,
                         sizeof(msg),
                         &bytesWritten,
                         NULL);
    return ret;
}

bool Client::ReceiveSocket(volatile unsigned long *spinlock)
{
    if (connection == NULL) return false;
    if (recv(connection,
             (char*)iMsgSocket,
             sizeof(*iMsgSocket),
             0) != SOCKET_ERROR)
    {
        WaitForSingleObject(hQueueMutex, INFINITE);
        inputMessages.push_back(*iMsgSocket);
        ReleaseMutex(hQueueMutex);
        InterlockedIncrement(spinlock);
        NewMessage();
        return true;
    }
    return false;
}

bool Client::ReceiveSocket()
{
    if (connection == NULL) return false;
    if (recv(connection,
             (char*)iMsgSocket,
             sizeof(*iMsgSocket),
             0) != SOCKET_ERROR)
    {
        WaitForSingleObject(hQueueMutex, INFINITE);
        inputMessages.push_back(*iMsgSocket);
        ReleaseMutex(hQueueMutex);
        NewMessage();
        return true;
    }
    return false;
}

bool Client::SendSocket(MessageData<> &msg)
{
    if (connection == NULL) return false;
    bool ret = send(connection,
                    (char*)&msg,
                    sizeof(msg),
                    0) != SOCKET_ERROR;
    return ret;
}
