// Worker.h
#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker();
    void doWork();

signals:
    void workDone(QString result);  // Сигнал, який буде передавати результат

};

#endif // WORKER_H
