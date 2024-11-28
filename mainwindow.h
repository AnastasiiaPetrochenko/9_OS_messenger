#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include <QMainWindow>
#include <QThread>
#include <QListWidget>
#include <QTextEdit>
#include <QDebug>
#include <vector>

#include "ui_mainwindow.h"

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
    QListWidget *usersList;  // Якщо потрібно для списку користувачів
    QTextEdit *generalChat;  // Якщо потрібно для чату

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
        // Додавання повідомлення в таблицю
        ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(input->msg));
    }

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
