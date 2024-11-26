#ifndef CLIENT_H
#define CLIENT_H

#include "datalib.h"
#include <windows.h>

#include <QObject>
#include <QString>

class Client : public QObject
{
    Q_OBJECT

public:
    Client();

};

#endif // CLIENT_H
