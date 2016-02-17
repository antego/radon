#include "worker.h"

#include <QThread>

Worker::Worker() {
}

Worker::~Worker() {

}

void Worker::process() {
    workerStatus.storeRelease(1);
    while (workerStatus.loadAcquire() == 1) {
        qDebug("Hello World!");
        QThread::sleep(1);
    }
    emit finished();
}

void Worker::stop() {
    workerStatus.storeRelease(0);
}
