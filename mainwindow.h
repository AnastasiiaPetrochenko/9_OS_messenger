#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"

#include <QMainWindow>
#include <Qtimer>
#include <QProcess>
#include <QTextEdit>
#include <vector>

#include <QThread>

#include "ui_mainwindow.h"


#include <QMetaObject>
#include <QDebug>
#include <algorithm>

#define DELAY 1000

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT
    SOCKET serverSocket;

    QThread *WaitMessageS;
    QThread *WaitConnectS;

    QListWidget *usersList;
    QTextEdit *generalChat;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_launchButton_clicked();

signals:
    void messageReceived(MessageData<> *input);
public slots:
    void sendMessage(MessageData<> *input) {
        int rowCount = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rowCount);
        //ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(input->name));
        ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(input->msg));

    }

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
