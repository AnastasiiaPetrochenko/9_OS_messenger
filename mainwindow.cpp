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

void MainWindow::on_connectButton_clicked()
{
    client = new Client();

    client->OpenSocket();

    int res = client->SendSocketMessage("Hello");
    ui->TextTest->setText(QString::number(res));

    // action
}


void MainWindow::on_pushButton_clicked()
{
    std::string message = (ui->lineEdit->text()).toStdString();
    int res = client->SendSocketMessage(message);
}

