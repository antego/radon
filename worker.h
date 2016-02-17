#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject {
    Q_OBJECT

public:
    Worker();
    ~Worker();

public slots:
    void process();

signals:
    void finished();

public:
    void stop();

private:
    QAtomicInt workerStatus;
};

#endif // WORKER_H
