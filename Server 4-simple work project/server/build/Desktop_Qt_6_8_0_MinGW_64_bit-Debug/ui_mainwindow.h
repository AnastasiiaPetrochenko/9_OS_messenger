/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *groupBox;
    QPushButton *launchButton;
    QPushButton *runClientButton;
    QRadioButton *mailslotRadioButton;
    QRadioButton *socketRadioButton;
    QTableWidget *tableWidget;
    QLabel *testLabel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(210, 20, 371, 181));
        launchButton = new QPushButton(groupBox);
        launchButton->setObjectName("launchButton");
        launchButton->setGeometry(QRect(50, 40, 261, 41));
        QFont font;
        font.setFamilies({QString::fromUtf8("Times New Roman")});
        font.setPointSize(12);
        font.setBold(true);
        font.setItalic(false);
        launchButton->setFont(font);
        launchButton->setStyleSheet(QString::fromUtf8("font: 700 12pt \"Times New Roman\";\n"
"background-color: rgb(123, 193, 255);"));
        runClientButton = new QPushButton(groupBox);
        runClientButton->setObjectName("runClientButton");
        runClientButton->setEnabled(false);
        runClientButton->setGeometry(QRect(50, 90, 261, 41));
        runClientButton->setFont(font);
        runClientButton->setStyleSheet(QString::fromUtf8("font: 700 12pt \"Times New Roman\";\n"
"background-color: rgb(123, 193, 255);"));
        mailslotRadioButton = new QRadioButton(groupBox);
        mailslotRadioButton->setObjectName("mailslotRadioButton");
        mailslotRadioButton->setGeometry(QRect(10, 140, 181, 26));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mailslotRadioButton->sizePolicy().hasHeightForWidth());
        mailslotRadioButton->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setPointSize(10);
        mailslotRadioButton->setFont(font1);
        mailslotRadioButton->setChecked(true);
        socketRadioButton = new QRadioButton(groupBox);
        socketRadioButton->setObjectName("socketRadioButton");
        socketRadioButton->setGeometry(QRect(190, 140, 171, 26));
        sizePolicy.setHeightForWidth(socketRadioButton->sizePolicy().hasHeightForWidth());
        socketRadioButton->setSizePolicy(sizePolicy);
        socketRadioButton->setFont(font1);
        tableWidget = new QTableWidget(centralwidget);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setGeometry(QRect(125, 211, 551, 331));
        testLabel = new QLabel(centralwidget);
        testLabel->setObjectName("testLabel");
        testLabel->setGeometry(QRect(670, 30, 71, 51));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Server", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Server", nullptr));
        launchButton->setText(QCoreApplication::translate("MainWindow", "Launch server", nullptr));
        runClientButton->setText(QCoreApplication::translate("MainWindow", "Run new client", nullptr));
        mailslotRadioButton->setText(QCoreApplication::translate("MainWindow", "Mailslot connection", nullptr));
        socketRadioButton->setText(QCoreApplication::translate("MainWindow", "Socket connection", nullptr));
        testLabel->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
