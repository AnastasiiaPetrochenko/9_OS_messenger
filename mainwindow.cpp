#include "mainwindow.h"
#include "ui_mainwindow.h"

volatile unsigned long spinlock = 0;

__cdecl void MailslotListener(void *parameters)
{
    Client *client = (Client*)parameters;

    while (true)
    {
        client->ReceiveMailslot(&spinlock);
        while (spinlock);
        Sleep(100);
    }
}

__cdecl void SocketListener(void *parameters)
{
    Client *client = (Client*)parameters;

    while (true)
    {
        client->ReceiveSocket(&spinlock);
        while (spinlock);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->oMsg = new MessageData<>;
    this->mailslotListener = NULL;
    this->socketListener = NULL;
    this->setFixedSize(this->width(), this->height());
    ui->messageLineEdit->setMaxLength(oMsg->GetMessageSize());
    ui->ipLineEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression(R"(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b)"),
                                     ui->ipLineEdit
                                     ));
    ui->portLineEdit->setValidator(new QIntValidator(0, 65535, ui->portLineEdit));

    bool ok;
    QString text = QInputDialog::getText(this, "Create client",
                                         "Enter user name:", QLineEdit::Normal,
                                         "", &ok);

    if (!ok) exit(0);

    this->client = new Client(0, (text.isEmpty() ? "User" : text));
    this->setWindowTitle("Messenger - " + this->client->GetName());

    connect(client, SIGNAL(NewMessage()), this, SLOT(HandleData()));

    auto general = this->users.insert(0, User("General", CreateChat()));
    ui->tabWidget->addTab(general->GetChat(), "General");

    ui->usernameLabel->setText("Name: " + client->GetName());
    //ui->idLabel->setText("ID: " + QString::number(client->GetId()));
}

MainWindow::~MainWindow()
{
    if (ui->connectButton->text() == "Disconnect")
    {
        on_connectButton_clicked();
    }

    delete oMsg;
    WSACleanup();
    delete ui;
}

void MainWindow::HandleData()
{
    MessageData<> *iMsg = nullptr;
    if (client->GetInputMessage(iMsg))
    {
        switch (iMsg->com)
        {
        case NEW_ID:
            client->SetId(iMsg->recvId);
            break;

        case NEW_USER:
            if (!users.contains(iMsg->recvId))
            {
                auto user = users.insert(iMsg->recvId,
                                         User(QString(iMsg->msg),
                                              CreateChat()));
                ui->tabWidget->addTab(user->GetChat(), QString(iMsg->msg));
            }
            break;

        case MESSAGE:
            if (users.contains(iMsg->senderId))
            {
                QTextEdit *chat = users[iMsg->recvId == 0
                                    ? 0
                                    : iMsg->senderId].GetChat();
                chat->append(users[iMsg->senderId].GetName()
                        + ": "
                        + QString(iMsg->msg)
                        + "\n");
                ui->tabWidget->setCurrentWidget(chat);
            }
            break;

        case REMOVE_USER:
        {
            auto user = users.find(iMsg->senderId);
            if (user != users.end())
            {
                users.erase(user);
            }
        }
            break;

        default:
            QMessageBox::information(this,
                                     "Undefined message",
                                     "Unrecognizable incoming message");
            break;
        }
        delete iMsg;
    }
    InterlockedDecrement(&spinlock);
}

void MainWindow::CriticalError(const QString &msg)
{
    QMessageBox::critical(this, "Critical error", msg);
    exit(-1);
}

void MainWindow::TerminateThreads()
{
    if (mailslotListener != NULL)
    {
        TerminateThread(mailslotListener, 0);
        WaitForSingleObject(mailslotListener, INFINITE);
        CloseHandle(mailslotListener);
        mailslotListener = NULL;
    }

    if (socketListener != NULL)
    {
        TerminateThread(socketListener, 0);
        WaitForSingleObject(socketListener, INFINITE);
        CloseHandle(socketListener);
        socketListener = NULL;
    }
}

QMap<id_t, User>::iterator MainWindow::GetCurrentUser()
{
    QTextEdit *currentTextEdit = (QTextEdit*)ui->tabWidget->currentWidget();
    for (auto user = users.begin(); user != users.end(); ++user)
    {
        if (user->GetChat() == currentTextEdit)
        {
            return user;
        }
    }
    return users.end();
}

QTextEdit *MainWindow::CreateChat() const
{
    QTextEdit *chat = new QTextEdit();
    chat->setReadOnly(true);
    chat->setFont(QFont("Verdana", 12, 12, false));
    return chat;
}

void MainWindow::on_sendButton_clicked()
{
    QString msg = ui->messageLineEdit->text();
    if ((quint32)msg.size() > oMsg->GetMessageSize())
    {
        QMessageBox::information(this,
                                 "Too long message",
                                 "Your message is too long ("
                                 + QString::number(msg.size())
                                 + " characters). Maximum message size: "
                                 + QString::number(oMsg->GetMessageSize())
                                 + " characters.");
        return;
    }

    if (Client::ContainsFlood(msg))
    {
        QMessageBox::warning(this,
                             "Flood",
                             "Your message contains a repeating sequence. "
                             "Sending was cancelled.");
        return;
    }

    oMsg->com = MessageCommand::MESSAGE;
    oMsg->SetMessage(msg);
    oMsg->recvId = GetCurrentUser().key();
    oMsg->senderId = client->GetId();

    bool ret = false;
    if (ui->mailslotRadioButton->isChecked())
    {
        ret = client->SendMailslot(*oMsg);
    }
    else if (ui->socketRadioButton->isChecked())
    {
        ret = client->SendSocket(*oMsg);
    }

    if (ret)
    {
        users[oMsg->recvId].GetChat()->append(client->GetName() + ": " + msg + "\n");
        ui->messageLineEdit->clear();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Cannot send message");
    }
}

void MainWindow::on_connectButton_clicked()
{
    if (ui->connectButton->text() == "Connect")
    {
        if (client->OpenSocket(ui->ipLineEdit->text().toLocal8Bit(),
                               ui->portLineEdit->text().toUShort()))
        {
            client->ReceiveSocket(&spinlock);
            while (spinlock);

            if (!client->OpenMailslots())
            {
                oMsg->com = REMOVE_USER;
                oMsg->senderId = client->GetId();
                client->SendSocket(*oMsg);
                client->CloseSocket();
                return;
            }

            oMsg->senderId = client->GetId();
            oMsg->com = NEW_USER;
            oMsg->SetMessage(client->GetName());
            client->SendSocket(*oMsg);

            mailslotListener = (HANDLE)_beginthread(MailslotListener, 0, client);
            socketListener = (HANDLE)_beginthread(SocketListener, 0, client);

            ui->ipLineEdit->setEnabled(false);
            ui->portLineEdit->setEnabled(false);
            ui->sendButton->setEnabled(true);
            ui->connectButton->setText("Disconnect");
            ui->idLabel->setText("ID: " + QString::number(client->GetId()));
            ui->mailslotLabel->setText("Server: " + client->GetMailslotOutputName()
                                       + "\nClient: " + client->GetMailslotInputName());
        }
    }
    else
    {
        TerminateThreads();

        oMsg->com = REMOVE_USER;
        oMsg->senderId = client->GetId();
        client->SendSocket(*oMsg);

        client->CloseMailslots();
        client->CloseSocket();

        users.clear();
        auto general = this->users.insert(0, User("General", CreateChat()));
        ui->tabWidget->addTab(general->GetChat(), "General");

        ui->ipLineEdit->setEnabled(true);
        ui->portLineEdit->setEnabled(true);
        ui->sendButton->setEnabled(false);
        ui->connectButton->setText("Connect");
        ui->idLabel->setText("ID: Unknown");
        ui->mailslotLabel->setText("Server: Unknown\nClient: Unknown");
    }
}
