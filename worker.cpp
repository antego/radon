#include "worker.h"

Worker::Worker() {
    sum = 2;
}

Worker::~Worker() {

}

void Worker::process() {
    qDebug("Hello World!");
    emit finished();
}
