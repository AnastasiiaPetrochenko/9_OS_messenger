#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"

#include <QMainWindow>
#include <QTextEdit>
#include <QListWidget>
#include <QMap>
#include <QMessageBox>
#include <QQueue>
#include <QProcess>

#define UPDATE_TIME 300

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void NewSocket();

    void HandleData(MessageData<> *data = nullptr);

    void on_launchButton_clicked();

    void on_runClientButton_clicked();

    void on_mailslotRadioButton_clicked(bool checked);

    void on_socketRadioButton_clicked(bool checked);

signals:
    void NewSocketSignal();
    void NewMessage();

private:
    Ui::MainWindow *ui;

    QListWidget *usersList;
    QTextEdit *generalChat;
    //QTimer *checkTimer;
    QMap<id_t, Client> clients;

    Client *newClient;
    Client::ConnectionType type;

    //QThread *openSocketListener;
    HANDLE openSocketListener;

    QTextEdit *CreateChat() const;
};
#endif // MAINWINDOW_H
