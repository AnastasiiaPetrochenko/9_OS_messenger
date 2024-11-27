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

// void MainWindow::WaitMessageSocket() {
//     while (true) {
//         for (auto client : clientSockets) {
//             SOCKET clientSocket = client->GetSocket();

//             if (clientSocket == INVALID_SOCKET) {
//                 continue;
//             }

//             char buffer[1024];
//             int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

//             if (bytesReceived > 0) {
//                 buffer[bytesReceived] = '\0';
//                 QString message = QString(buffer);

//                 // add message to ui
//                 int rowCount = ui->tableWidget->rowCount();
//                 ui->tableWidget->insertRow(rowCount);

//                 ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(message));

//             } else if (bytesReceived == 0) {
//                 closesocket(clientSocket);
//                 clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), client), clientSockets.end());
//                 delete client;
//                 qDebug() << "Client disconnected. Total clients: " << clientSockets.size();
//             }
//         }
//         QThread::msleep(1000);
//     }
// }

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
    serverAddr.sin_addr.s_addr = inet_addr(ADDRESS);
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        return NULL;
    }

    return serverSocket;
}



void WaitConnectionS(SOCKET Server){
    qDebug() << "1";
    while (true){
    listen(Server, SOMAXCONN);

    SOCKET socket_ = accept(Server, nullptr, nullptr);
    qDebug() << "n";
    if (socket_ != INVALID_SOCKET) {
        qDebug() << "nggggg";
        Client* newClient = new Client();
        newClient->SetSocket(socket_);
        clientSockets.push_back(newClient);
        qDebug() << "New client connected. Total clients: " << clientSockets.size();
    }
    } }


void WaitMessageSocket() {
    qDebug() << "2";
    while (true){
    for (auto client : clientSockets) {
        qDebug() << "3";
        SOCKET clientSocket = client->GetSocket();

        if (clientSocket == INVALID_SOCKET) {
            continue;
        }

        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            qDebug() << "МЕСИДЖ";
            buffer[bytesReceived] = '\0';
            QString message = QString(buffer);

            // Викликаємо сигнал, щоб сповістити UI
            emit mainWindow->messageReceived(message);  // Викликаємо сигнал
        } else if (bytesReceived == 0) {
            closesocket(clientSocket);
            clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), client), clientSockets.end());
            delete client;
            qDebug() << "Client disconnected. Total clients: " << clientSockets.size();
        }
    } }
}


void MainWindow::on_launchButton_clicked() {

    serverSocket = OpenSocket();

    QThread* connectThread = new QThread(this);
    QObject::connect(connectThread, &QThread::started, [this](){
        WaitConnectionS(serverSocket);
    });
    connectThread->start();

    // Створення потоку для функції обробки повідомлень
    QThread* messageThread = new QThread(this);
    QObject::connect(messageThread, &QThread::started, [this](){
        WaitMessageSocket();
    });
    messageThread->start();

    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount);
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem("ready"));
}

