#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "User.h"

#include <windows.h>
#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextEdit>
#include <QTimer>
#include <QMap>
#include <QVector>
#include <QFile>
#include <QValidator>

#define UPDATE_TIME 100

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
    void HandleData();

    void on_sendButton_clicked();

    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui;

    Client *client;
    MessageData<> *oMsg;

    QMap<id_t, User> users; // <tab_index, id>

    HANDLE mailslotListener;
    HANDLE socketListener;

    void CriticalError(const QString &msg);
    void TerminateThreads();
    QMap<id_t, User>::iterator GetCurrentUser();
    QTextEdit *CreateChat() const;
};
#endif // MAINWINDOW_H
