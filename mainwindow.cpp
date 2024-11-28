#include "mainwindow.h"
#include "ui_mainwindow.h"

MessageData<> *iMsg, *oMsg;
QQueue<MessageData<>> inputMessages;

HANDLE queueMutex;

__cdecl void SocketListener(void *parameters)
{
    auto client = (Client*)parameters;
    MessageData<> *data = new MessageData<>;

    while (true)
    {
        if (client->ReceiveSocket(data))
        {
            WaitForSingleObject(queueMutex, INFINITE);
            inputMessages.push_back(*data);
            if (inputMessages.back().com == REMOVE_USER)
            {
                delete data;
                ReleaseMutex(queueMutex);
                client->NewMessage();
                return;
            }
            ReleaseMutex(queueMutex);
            client->NewMessage();
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    iMsg = new MessageData<>();
    oMsg = new MessageData<>();
    newClient = new Client();
    this->openSocketListener = NULL;
    queueMutex = CreateMutexA(0, false, 0);
    type = Client::SOCKET_CONNECTION;

    usersList = new QListWidget();
    usersList->setFont(QFont("Arial", 12, 12, false));
    ui->tabWidget->addTab(usersList, "All Users");

    generalChat = CreateChat();
    ui->tabWidget->addTab(generalChat, "General");

    ui->socketLabel->setText("IP: " IP_ADDRESS "\nPort: " + QString::number(PORT));
}

MainWindow::~MainWindow()
{
    for (auto it : clients)
    {
        it.CloseSocket();
    }

    delete usersList;
    delete generalChat;
    delete iMsg;
    delete oMsg;

    if (openSocketListener != NULL)
    {
        TerminateThread(openSocketListener, 0);
        CloseHandle(openSocketListener);
    }

    CloseHandle(queueMutex);

    delete newClient;
    WSACleanup();

    delete ui;
}

__cdecl void OpenSocketListener(void *parameters)
{
    Client *client = (Client*)parameters;

    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        QMessageBox::critical(nullptr, "Error", "WSAStartup failed");
        return;
    }

    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, 0);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    SOCKET connection;

    while (true)
    {
        WaitForSingleObject(queueMutex, INFINITE);
        if ((connection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr)) != INVALID_SOCKET)
        {
            client->SetSocket(connection);
            client->NewSocket();
        }
        else ReleaseMutex(queueMutex);
    }
}

void MainWindow::NewSocket()
{
    MessageData<> *oMsg = new MessageData<>;

    oMsg->com = NEW_ID;
    oMsg->recvId = Client::GetNewId();
    newClient->SendSocket(oMsg);

    newClient->ReceiveSocket(iMsg);
    HandleData(iMsg);
    ReleaseMutex(queueMutex);
    delete oMsg;
}

void MainWindow::on_launchButton_clicked()
{
    connect(newClient, SIGNAL(NewSocket()), this, SLOT(NewSocket()));
    openSocketListener = (HANDLE)_beginthread(OpenSocketListener, 0, (void*)newClient);

    connect(this, SIGNAL(NewMessage()), this, SLOT(HandleData()));
    _beginthread(SocketListener, 0, (void*)this);

    ui->launchButton->setEnabled(false);
    ui->runClientButton->setEnabled(true);
}

void MainWindow::on_runClientButton_clicked()
{
    QProcess::startDetached(Client::CLIENT_PATH);
}

QTextEdit *MainWindow::CreateChat() const
{
    QTextEdit *chat = new QTextEdit();
    chat->setReadOnly(true);
    chat->setFont(QFont("Verdana", 12, 12, false));
    return chat;
}

void MainWindow::HandleData(MessageData<> *data)
{
    MessageData<> *iMsg;

    if (data != nullptr)
    {
        iMsg = data;
    }
    else
    {
        WaitForSingleObject(queueMutex, INFINITE);

        if (inputMessages.isEmpty())
        {
            ReleaseMutex(queueMutex);
            return;
        }
        iMsg = new MessageData<>(inputMessages.front());
        inputMessages.pop_front();

        ReleaseMutex(queueMutex);
    }

    switch (iMsg->com)
    {
    case NEW_USER:
        if (!clients.contains(iMsg->senderId))
        {
            auto client = clients.insert(iMsg->senderId,
                                         Client(new QListWidgetItem("Username: "
                                                                    + QString(iMsg->msg)
                                                                    + "\nID: "
                                                                    + QString::number(iMsg->senderId)),
                                                QString(iMsg->msg),
                                                CreateChat(),
                                                newClient->GetSocket()));
            client->OpenSocket();

            usersList->addItem(client->GetListItem());
            ui->tabWidget->addTab(client->GetTextEdit(), client->GetName());

            iMsg->recvId = iMsg->senderId;
            MessageData<> *oMsg = new MessageData<>;

            oMsg->com = MessageCommand::NEW_USER;
            for (auto it = clients.begin(); it != clients.end(); ++it)
            {
                if (it.key() != iMsg->senderId)
                {
                    oMsg->recvId = it.key();
                    oMsg->SetMessage(it.value().GetName());
                    client->Send(type, oMsg);
                    it->Send(type, iMsg);
                }
            }
            delete oMsg;
            client->StartThread(SocketListener, this, SLOT(HandleData()));
        }
        break;

    case MESSAGE:
        if (iMsg->recvId == 0)
        {
            generalChat->append(clients[iMsg->senderId].GetName() + ": " + QString(iMsg->msg));
            for (auto it = clients.begin(); it != clients.end(); ++it)
            {
                if (it.key() != iMsg->senderId)
                {
                    if (!it->Send(type, iMsg))
                    {
                        QMessageBox::warning(this, "Warning", "Message was not sent");
                        break;
                    }
                }
            }
        }
        else
        {
            auto receiver = clients.find(iMsg->recvId);
            if (receiver != clients.end())
            {
                auto sender = clients.find(iMsg->senderId);
                receiver->AppendToChat(sender == clients.end()
                                           ? "Unknown"
                                           : clients[iMsg->senderId].GetName(),
                                       iMsg->msg);
                if (!receiver->Send(type, iMsg))
                {
                    QMessageBox::warning(this, "Warning", "Message was not sent");
                    break;
                }
            }
        }
        break;

    case REMOVE_USER:
    {
        auto client = clients.find(iMsg->senderId);
        if (client != clients.end())
        {
            client->StopThread();
            client->CloseSocket();
            clients.erase(client);
            for (Client &cl : clients)
            {
                cl.Send(type, iMsg);
            }
        }
    }
    break;

    default:
        QMessageBox::warning(this,
                             "Undefined command",
                             "Message contains undefined command and was ignored");
        break;
    }
    if (data == nullptr) delete iMsg;
}

void MainWindow::on_socketRadioButton_clicked(bool checked)
{
    if (checked) type = Client::SOCKET_CONNECTION;
}
