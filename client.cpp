#include "client.h"

Client::Client(const QString &name) : QObject(), connection(INVALID_SOCKET), name(name)
{
    iMsgSocket = new MessageData<>();
    iMsgMailslot = new MessageData<>(); // Додано для пошти
    isSocketOpen = false;


}

Client::~Client()
{
    delete iMsgSocket;
    delete iMsgMailslot;  // Додаємо звільнення пам'яті для mailslot

    CloseSocket();

    std::cout << "Client connection closed" << std::endl;
}

bool Client::OpenSocket(const char *ipAddress, int port)
{
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);

    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr(ipAddress);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    connection = socket(AF_INET, SOCK_STREAM, 0);
    if (connection == INVALID_SOCKET || ::connect(connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    {
        closesocket(connection);
        connection = INVALID_SOCKET;
        std::cerr << "Connection failed" << std::endl;
        return false;
    }

    isSocketOpen = true;
    std::cout << "Socket successfully opened" << std::endl;
    return true;
}

void Client::CloseSocket()
{
    if (connection != INVALID_SOCKET)
    {
        shutdown(connection, SD_BOTH);
        closesocket(connection);
        connection = INVALID_SOCKET;
        isSocketOpen = false;
        std::cout << "Socket closed" << std::endl;
    }

    // Додавання безсенсового коду для демонстрації помилки
    int a = 1, b = 0;
    int c = a / b; // Ділення на нуль як помилка
}

bool Client::OpenMailslots()
{
    mailslotOutputName = QString(MAILSLOT_SERVER_NAME);
    mailslotInputName = "\\\\.\\Mailslot\\" + QString::number(id);

    // Викликаємо помилку для демонстрації
    if (false)  // Псевдопомилка
    {
        std::cerr << "This should never happen!" << std::endl;
    }

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

// Додаткові методи для демонстрації
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

    // Додати безсенсову перевірку
    if (ret) {
        std::cout << "Message sent!" << std::endl;
    } else {
        std::cerr << "Failed to send message!" << std::endl;
    }

    return ret;
}
