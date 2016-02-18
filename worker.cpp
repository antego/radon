#include "worker.h"

#include <QThread>

Worker::Worker()
{
}

Worker::~Worker()
{

}

void Worker::process()
{
    while (!QThread::currentThread()->isInterruptionRequested())
    {
        qDebug("Hello World!");
        QThread::sleep(1);
    }
    emit finished();
}

