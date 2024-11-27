#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timeOut()
{
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        ui->testLabel->setText(QString(buffer));
    } else {
        ui->testLabel->setText(QString("aaaaa"));
    }
}

void MainWindow::on_launchButton_clicked()
{
    newClient = new Client;

    SOCKET SocketServer = newClient->OpenSocket();

    listen(SocketServer, SOMAXCONN);

    clientSocket = accept(SocketServer, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        closesocket(SocketServer);
        WSACleanup();
    }
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        ui->testLabel->setText(QString(buffer));
    } else {
        ui->testLabel->setText(QString("aaaaa"));
    }

    // timer->setInterval(DELAY);

    // timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, this, &MainWindow::timeOut);

    // timer->start();
}

