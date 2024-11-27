#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->oMsg = new MessageData<>;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    client = new Client(ui->nameLine->text());


    client->OpenSocket();

    oMsg->senderId = 0;
    oMsg->com = NEW_USER;
    oMsg->SetMessage(client->getName());
    client->SendSocketMessage(*oMsg);

    // action
}


void MainWindow::on_pushButton_clicked()
{
/*    std::string message = (ui->lineEdit->text()).toStdString();
    int res = client->SendSocketMessage(message)*/;
}

