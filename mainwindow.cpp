#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow *mainWindow;
std::vector<Client *> clientSockets;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainWindow = this;
    connect(this, &MainWindow::messageReceived, this, &MainWindow::sendMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

SOCKET OpenSocket()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        return NULL;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        return NULL;
    }

    return serverSocket;
}

void WaitConnectionS(SOCKET server)
{
    while (true) {
        listen(server, SOMAXCONN);

        SOCKET socket_ = accept(server, nullptr, nullptr);
        if (socket_ != INVALID_SOCKET) {
            Client* newClient = new Client();
            newClient->SetSocket(socket_);
            clientSockets.push_back(newClient);
            qDebug() << "New client connected. Total clients: " << clientSockets.size();
        }
    }
}

void WaitMessageSocket()
{
    while (true) {
        for (auto client : clientSockets) {
            SOCKET clientSocket = client->GetSocket();

            if (clientSocket == INVALID_SOCKET) {
                continue;
            }

            MessageData<> *input = new MessageData<>; // Ініціалізуємо пам'ять для input
            int bytesReceived = recv(clientSocket, (char*)input, sizeof(*input), 0);

            if (bytesReceived > 0) {
                emit mainWindow->messageReceived(input);
            } else if (bytesReceived == 0) {
                closesocket(clientSocket);
                clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), client), clientSockets.end());
                delete client;
                qDebug() << "Client disconnected. Total clients: " << clientSockets.size();
            }
        }
    }
}

void MainWindow::on_launchButton_clicked()
{
    SOCKET serverSocket = OpenSocket();
    if (serverSocket == NULL) {
        qDebug() << "Failed to open server socket.";
        return;
    }

    QThread* connectThread = new QThread(this);
    QObject::connect(connectThread, &QThread::started, [this, serverSocket]() {
        WaitConnectionS(serverSocket);
    });
    connectThread->start();

    // Створення потоку для функції обробки повідомлень
    QThread* messageThread = new QThread(this);
    QObject::connect(messageThread, &QThread::started, [this]() {
        WaitMessageSocket();
    });
    messageThread->start();

    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount);
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem("ready"));
}
