// Worker.cpp
#include "worker.h"
#include <QThread>
#include <QDebug>

Worker::Worker() {}

void Worker::doWork()
{
    // Логіка виконання роботи
    QString result = "Work completed";

    // Передаємо результат у вигляді сигналу
    emit workDone(result);
}
